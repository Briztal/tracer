/*
  kinetis_UART.c Part of TRACER

  Copyright (c) 2017 Raphaël Outhier

  TRACER is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  TRACER is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  aint32_t with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <kernel/arch/arch.h>

#include "kinetis_UART.h"

#include <kernel/kernel.h>
#include <HardwareSerial.h>
#include <string.h>


//TODO RECEIVE OVERRUN


/*
 * SET : will set [data]'s bits that are set to 1 in [mask]. [data] is of size [size];
 */

#define SET(data, mask, size) (data) |= (uint##size##_t)(mask)


/*
 * CLEAR : will clear [data]'s bits that are set to 1 in [mask]. [data] is of size [size];
 */

#define CLEAR(data, mask, size) (data) &= ~(uint##size##_t)(mask)


/*
 * SET_BIT : will set the [bit_id]-th LS bit of [data]. [data] is of size [size];
 */

#define SET_BIT(data, bit_id, size) SET(data, 1 << (bit_id), size)


/*
 * CLEAR_BIT : will clear the [bit_id]-th LS bit of [data]. [data] is of size [size];
 */

#define CLEAR_BIT(data, bit_id, size) CLEAR(data, 1 << (bit_id), size)


//--------------------------------------------------- Private headers --------------------------------------------------

//-------------------------- Peripheral init --------------------------

//Initialise the peripheral;
static void initialise_peripheral(const struct kinetis_UART_hw_t *peripheral_data);

//Start the peripheral;
static void start_peripheral(const struct kinetis_UART_hw_t *driver_data);

//Reset the peripheral in the stopped state;
static void stop_peripheral(const struct kinetis_UART_hw_t *driver_data);


//-------------------------- Peripheral config --------------------------

//Configure the packet format;
static void configure_packet_format(const struct kinetis_UART_hw_t *data, const struct UART_config_t *);

//Configure the state;
static void configure_modem(const struct kinetis_UART_hw_t *data, const struct UART_config_t *);

//Configure the transmission layer;
static void configure_transmission_layer(const struct kinetis_UART_hw_t *data, const struct UART_config_t *);



//-------------------------- Stream functions --------------------------

//Get the memory map;
static void get_memory_map(const struct kinetis_UART_stream_t *stream, struct mem_map_t *map);

//determine the number of spaces available in the input (tx) stream;
static size_t input_spaces_available(const struct kinetis_UART_stream_t *stream);

//determine the number of bytes available in the output (rx) stream;
static size_t output_bytes_available(const struct kinetis_UART_stream_t *stream);


//Read data from rx;
static void UART_read(const struct kinetis_UART_stream_t *stream, const struct mem_map_t *src_map,
					  const struct blocks_desc_t *descriptor);

//Write data in tx;
static void UART_write(const struct kinetis_UART_stream_t *stream, const struct mem_map_t *src_map,
					   const struct blocks_desc_t *descriptor);

//The UART stream doesn't store any dynamic data;
static void UART_stream_delete(struct kinetis_UART_stream_t *stream) {
	kernel_free(stream);
}


//--------------------------------------------------- Pipe functions ---------------------------------------------------

//Delete the interrupt pipe;
static void kinetis_interrupt_pipe_destroy(const struct kinetis_UART_interrupt_pipe_t *data) {};


//Enable the rx trigger;
static void enable_rx_trigger(const struct kinetis_UART_interrupt_pipe_t *pipe);

//Disable the rx trigger;
static void disable_rx_trigger(const struct kinetis_UART_interrupt_pipe_t *pipe);

//Enable the tx trigger;
static void enable_tx_trigger(const struct kinetis_UART_interrupt_pipe_t *pipe);

//Disable the rx trigger;
static void disable_tx_trigger(const struct kinetis_UART_interrupt_pipe_t *pipe);


//Update the rx watermark;
static void update_rx_watermark(const struct kinetis_UART_interrupt_pipe_t *pipe, size_t nb_bytes);

//Update the tx watermark;
static void update_tx_watermark(const struct kinetis_UART_interrupt_pipe_t *pipe, size_t nb_spaces);



//-------------------------------------------------- Hardware Settings -------------------------------------------------

/*
 * initialise_peripheral : the initialisation function has to : 
 * 	- set all required perhipheral registers in a safe, stopped and initialised state;
 * 	- disable related interrupts and DMA;
 * 	- update all required interrupt functions and setup DMA if necessary;
 */

void initialise_peripheral(const struct kinetis_UART_hw_t *const peripheral_data) {

	//Cache the memory pointer;
	struct kinetis_UART_registers_t *const registers = peripheral_data->registers;

	//Turn on the UART0's clock;//TODO MAKE A SIM DRIVER;
	SIM_SCGC4 |= SIM_SCGC4_UART0;

	//Clear C2 to disable all interrupts;
	registers->C2 = 0;

	//Enable both FIFOs;
	registers->PFIFO = (uint8_t) UART_PFIFO_TXFE | (uint8_t) UART_PFIFO_RXFE;

	//Flush FIFOs;
	SET(registers->CFIFO, UART_CFIFO_RXFLUSH & UART_CFIFO_TXFLUSH, 8);

	//Set the tx watermark to 0, and the rx to 1.
	registers->TWFIFO = 0;
	registers->RWFIFO = 1;


	/*
	 * Supported interrupts are :
	 *  - transmission complete; TODO INTERRUPT SUPPORT FOR HALF DUPLEX
	 *  - transmission buffer empty (below water);
	 *  - reception buffer empty (beyond water);
	 *  - idle line; TODO INTERRUPT SUPPORT FOR HALF DUPLEX
	 *
	 *  All those are located in the C2 register;
	 */


	//Cache the status interrupt index;
	const uint8_t status_id = peripheral_data->status_int_channel;

	//Disable the status interrupt;
	core_IC_disable(status_id);

	//Set the status interrupt priority;
	core_IC_set_priority(status_id, DRIVER_STARUS_INTERRUPT_PRIORITY);

	//Eventually de-activate a pending status interrupt;
	core_IC_clear_pending(status_id);

	//Set the provided interrupt link as the interrupt function;
	core_IC_set_handler(status_id, peripheral_data->status_link);



	/*
	 * Errors supported are :
	 *  - framing errors (locking) - read and forget last element;
	 *  - receiver overrun (locking) - receiver overflow (same ones) -> receiver flush;
	 *  - receiver underflow (misalignment) -> receiver flush;
	 *  - transmitter overflow (misalignment) -> transmitter flush;
	 *  - noise flag - parity error (informative) -> read and forget last element;
	 */

	//Enable framing, overrun, noise and parity errors exceptions;
	SET(registers->C3, UART_C3_FEIE | UART_C3_ORIE | UART_C3_NEIE | UART_C3_PEIE, 8);

	//Enable receiver overflow, receiver underflow and transmitter overflow exceptions;
	SET(registers->CFIFO, UART_CFIFO_RXOFE | UART_CFIFO_RXUFE | UART_CFIFO_TXOFE, 8);


	//Cache the error interrupt index;
	const uint8_t error_id = peripheral_data->error_int_channel;

	//Disable the status interrupt, will be activated at startup;
	core_IC_disable(error_id);

	//Set the error interrupt priority;
	core_IC_set_priority(error_id, DRIVER_ERROR_INTERRUPT_PRIORITY);

	//Eventually de-activate a pending status interrupt;
	core_IC_clear_pending(error_id);

	//Set the provided interrupt link as the interrupt function;
	core_IC_set_handler(error_id, peripheral_data->error_link);


}

//TODO DEINIT : DISABLE ALL INTERRUPTS, RESETS INTERRUPT FUNCTION

/*
 * start_peripheral : starts the peripheral, resets fifos, enables interrupts;
 */

void start_peripheral(const struct kinetis_UART_hw_t *driver_data) {

	//Cache memory register;
	struct kinetis_UART_registers_t *const registers = driver_data->registers;

	//Set the tx watermark to 0, and the rx to 1.
	registers->TWFIFO = 0;
	registers->RWFIFO = 1;

	//Flush FIFOs;
	SET(registers->CFIFO, UART_CFIFO_RXFLUSH | UART_CFIFO_TXFLUSH, 8);


	/*
	 * Errors supported are :
	 *  - framing errors (locking) - read and forget last element;
	 *  - receiver overrun (locking) - receiver overflow (same ones) -> receiver flush;
	 *  - receiver underflow (misalignment) -> receiver flush;
	 *  - transmitter overflow (misalignment) -> transmitter flush;
	 *  - noise flag - parity error (informative) -> read and forget last element;
	 */

	//Enable framing, overrun, noise and parity errors exceptions;
	SET(registers->C3, UART_C3_FEIE | UART_C3_ORIE | UART_C3_NEIE | UART_C3_PEIE, 8);

	//Enable receiver overflow, receiver underflow and transmitter overflow exceptions;
	SET(registers->CFIFO, UART_CFIFO_RXOFE | UART_CFIFO_RXUFE | UART_CFIFO_TXOFE, 8);


	//Enable Rx and TX
	SET(registers->C2, UART_C2_RE | UART_C2_TE, 8);

	//Enable the status interrupt;
	core_IC_enable(driver_data->status_int_channel);

	//Disable the status interrupt;
	core_IC_enable(driver_data->error_int_channel);

}


/*
 * stop_peripheral : Stops the peripheral, resets fifos, disable interrupts;
 */

void stop_peripheral(const struct kinetis_UART_hw_t *driver_data) {

	//Cache memory register;
	struct kinetis_UART_registers_t *const registers = driver_data->registers;

	//Disable the status interrupt;
	core_IC_disable(driver_data->status_int_channel);

	//Disable the status interrupt;
	core_IC_disable(driver_data->error_int_channel);

	//Disable Rx and TX
	CLEAR(registers->C2, UART_C2_RE | UART_C2_TE, 8);

	//Disable interrupts;
	CLEAR(registers->C2, UART_C2_TIE | UART_C2_RIE, 8);

	//Flush FIFOs;
	registers->CFIFO |= UART_CFIFO_RXFLUSH;
	registers->CFIFO |= UART_CFIFO_TXFLUSH;

}



//------------------------------------------------- Creation - Deletion ------------------------------------------------

/*
 * kinetis_UART_create : creates an instance of a kinetis UART driver from hardware driver specs;
 */

struct kinetis_UART_driver_t *kinetis_UART_create(struct kinetis_UART_hw_t *const peripheral_data) {


	//Initialise the peripheral;
	initialise_peripheral(peripheral_data);

	//Create the driver struct initializer;
	struct kinetis_UART_driver_t init = {

		//Copy the peripheral data set;
		.peripheral_data = *peripheral_data,

		//Mark the driver not initialised;
		.initialised = false,

		//Streams are not created;
		.input_stream = 0,
		.output_stream = 0,

	};

	//Allocate, initialise and return the driver data structure;
	return kernel_malloc_copy(sizeof(struct kinetis_UART_driver_t), &init);

}


/*
 * kinetis_UART_delete : deinitializes the peripheral and deletes the peripheral data structure;
 */

void kinetis_UART_delete(struct kinetis_UART_driver_t *driver_data) {

	//Reset the peripheral;
	stop_peripheral(&driver_data->peripheral_data);

	//Delete the driver data structure;
	kernel_free(driver_data);

}


//--------------------------------------------------- Hardware setup ---------------------------------------------------


uint8_t index_to_size(uint8_t index) {

	//If the index is null :
	if (!index) {

		//FIFO of size 1;
		return 1;

	} else if (index < 8) {
		//If the index is lesser than 8 :

		//FIFO of size 2^(index + 1);
		return (uint8_t) ((uint8_t) 1 << (index + (uint8_t) 1));

	} else {
		//If the index is greater or equal than 8 :

		//NO FIFO, ERROR, BAD INDEX; TODO KERNEL ERROR.
		return 0;

	}

}

void sizes_from_PFIFO(const uint8_t pfifo, uint8_t *const rx_fifo_size, uint8_t *const tx_fifo_size) {

	//Cache the 3 msb of PFIFO to get rxindex;
	uint8_t index = pfifo & (uint8_t) 7;

	//Determine and save the rx size;
	*rx_fifo_size = index_to_size(index);

	//Shift of 4 bits and cache the 3 msb to get txindex;
	index = (pfifo >> 4) & (uint8_t) 7;

	//Determine and save the tx size;
	*tx_fifo_size = index_to_size(index);

}


/*
 * Packet format configuration : sets :
 *  - The number of data bits;
 *  - The parity bit (enables of disables);
 *  - The parity type (Even or Odd) if it is enabled;
 */

void configure_packet_format(const struct kinetis_UART_hw_t *const data, const struct UART_config_t *const config) {

	//TODO MSB FIRST
	//TODO STOP BITS 1 OR 2 FIRST BYTE OF BAUDRATE REGISTER

	//To avoid implicit double pointer access, we will cache data;
	struct kinetis_UART_registers_t *const registers = data->registers;

	/*
	 * Number of bits; This peripheral only supports 8 or 9 data bits;
	 *
	 * The number of bits is defined by bit 4 of C1;
	 */

	//Check the number of bits :
	switch (config->nb_data_bits) {

		case 8:
			//8 data bits, bit M cleared;
			CLEAR(registers->C1, UART_C1_M, 8);
			break;

		case 9:
			//9 data bits, bit 4 set;
			SET(registers->C1, UART_C1_M, 8);
			break;

		default:
			//Other number, not suported, driver panic;
			//TODO DRIVER PANIC;
			break;

	}

	/*
	 * Parity update and type : Parity (update and type) is defined by (resp) bits 1 and 0 of C1;
	 */

	//By default, disable the 10-th bit by clearing bit 5 of C4. Will be re-enabled if necessary;
	CLEAR(registers->C4, UART_C4_M10, 8);


	//If the parity bit must be enabled :
	if (config->parity_bit_enabled) {

		//Bit 1 of C1 is set;
		SET(registers->C1, UART_C1_PE, 8);

		//We must set the parity type too;

		//If the parity is of type EVEN (default) :
		if (config->parity_type == EVEN_PARITY) {

			//Bit PT of C1 is cleared;
			CLEAR(registers->C1, UART_C1_PT, 8);


		} else {

			//If the parity is odd, bit 0 of C1 is set;
			SET(registers->C1, UART_C1_PT, 8);

		}

		//Finally, if the parity is enabled and there are 9 data bits, the 10-th bit must be declared;
		if (config->nb_data_bits == 9) {

			//Set M10, bit 5 of C4;
			SET(registers->C4, UART_C4_M10, 8);

		}

	} else {

		//Parity bit disabled, bit 1 of C1 is cleared;
		CLEAR(registers->C1, UART_C1_PE, 8);

	}

}


/*
 * configure_modem : enables or disables CTS or RTS support;
 */

void configure_modem(const struct kinetis_UART_hw_t *const data, const struct UART_config_t *const config) {

	struct kinetis_UART_registers_t *const registers = data->registers;

	/*
	 * Rx_RTS_enable and Tx_Cts_enable are (resp) bits 3 and 0 of MODEM;
	 */

	//If RTS must be enabled :
	if (config->rts_enabled) {

		//Set bit 3 of MODEM;
		SET(registers->MODEM, UART_MODEM_RXRTSE, 8);

	} else {
		//If RTS must be disabled, clear bit 3 of MODEM;
		CLEAR(registers->MODEM, UART_MODEM_RXRTSE, 8);

	}

	//If CTS must be enabled :
	if (config->cts_enabled) {

		//Set bit 0 of MODEM;
		SET(registers->MODEM, UART_MODEM_TXCTSE, 8);

	} else {

		//If CTS must be disabled, clear bit 0 of MODEM;
		CLEAR(registers->MODEM, UART_MODEM_TXCTSE, 8);

	}

}


/*
 * configure_transmission_layer : this function will setup the transmission configuration according to the required :
 *
 *  - Mode (full or half duplex);
 *  - BaudRate;
 *
 */

void
configure_transmission_layer(const struct kinetis_UART_hw_t *const data, const struct UART_config_t *const config) {

	//Cache the data pointer to avoid permanent implicit double pointer access;
	struct kinetis_UART_registers_t *registers = data->registers;

	/*
	 * We will configure the transmission type : Full or Half duplex;
	 */

	if (config->transmission_type == FULL_DUPLEX) {

		//To achieve a full-duplex configuration, the MSB of C1 must be cleared;
		CLEAR(registers->C1, UART_C1_LOOPS, 8);

		//TODO IF HALF DUPLEX, TXDIR MUST BE MANAGED CORRECTLY;

	} else {

		//To achieve a half duplex configuration, we must set bits 5 and 7 of C1 : OR with 10100000 (160)
		SET(registers->C1, UART_C1_LOOPS | UART_C1_RSRC, 8);

	}


	/*
	 * BaudRate Configuration :
	 *
	 * The Baudrate is adjusted by two parameters, SBR (Coarse adjust) and BRFD (Fine adjust),
	 *  that verify :
	 *
	 *      Kb = 2 * UART_CLOCK / BaudRate
	 *
	 *      SBR (uint13_t) = Kb / 32 (integer division);
	 *      BRFA (uint5_t) = Kb % 32;
	 */

	//Determine Kb;
	uint32_t Kb = ((uint32_t) ((uint32_t) (data->clock_frequency << 1)) / config->baudrate);

	//Set the 5 MSB of Kb in BDH;
	registers->BDH = (uint8_t) ((Kb >> 13) & 0x1F);

	//Set the 8 next MSB of Kb in BDL
	registers->BDL = (uint8_t) ((Kb >> 5));

	//Set 5 LSF of Kb in C4
	registers->C4 = (uint8_t) (Kb & 0x1F);

}


//----------------------------------------------------- Init - exit ----------------------------------------------------

/*
 * kinetis_UART_start : configures the transmission stack, creates streams, and starts the UART;
 */

void kinetis_UART_start(struct kinetis_UART_driver_t *driver_data, const struct UART_config_t *config) {

	//Cache the hardware struct;
	struct kinetis_UART_hw_t *peripheral_data = &driver_data->peripheral_data;

	//Initialise different parts of the UART;
	configure_packet_format(peripheral_data, config);
	configure_modem(peripheral_data, config);
	configure_transmission_layer(peripheral_data, config);

	//Cache the register struct pointer;
	struct kinetis_UART_registers_t *const registers = peripheral_data->registers;

	//Cache all required registers pointer;
	volatile uint8_t *const D_reg = &registers->D;
	volatile uint8_t *const S1_reg = &registers->S1;
	volatile uint8_t *const C2_reg = &registers->C2;
	volatile uint8_t *const RXWATER_reg = &registers->RWFIFO;
	volatile uint8_t *const TXWATER_reg = &registers->TWFIFO;
	volatile uint8_t *const RCFIFO_reg = &registers->RCFIFO;
	volatile uint8_t *const TCFIFO_reg = &registers->TCFIFO;


	//TODO SET WATERMARK TO TRIGGER AN INTERRUPT;
	//TODO SET WATERMARK TO TRIGGER AN INTERRUPT;
	//TODO SET WATERMARK TO TRIGGER AN INTERRUPT;
	//TODO SET WATERMARK TO TRIGGER AN INTERRUPT;
	//TODO SET WATERMARK TO TRIGGER AN INTERRUPT;
	//TODO SET WATERMARK TO TRIGGER AN INTERRUPT;
	//TODO SET WATERMARK TO TRIGGER AN INTERRUPT;
	//TODO SET WATERMARK TO TRIGGER AN INTERRUPT;
	//TODO SET WATERMARK TO TRIGGER AN INTERRUPT;
	//TODO SET WATERMARK TO TRIGGER AN INTERRUPT;
	//TODO SET WATERMARK TO TRIGGER AN INTERRUPT;

	//Configure C5 so that RX full - TX empty flags generate interrupt; clear C5's TDMAS and RDMAS;
	registers->C5 &= ~(UART_C5_TDMAS | UART_C5_RDMAS);

	//Declare both FIFOs sizes;
	uint8_t rx_fifo_size, tx_fifo_size;

	//Get both FIFOs sizes;
	sizes_from_PFIFO(registers->PFIFO, &rx_fifo_size, &tx_fifo_size);


	/*
	 * Pipes and stream const-reference each other. We must use a trick to properly initialise both;
	 */

	//------------------ RX (OUTPUT) STREAM - PIPE INIT ------------------

	//Pre-allocate memory for the pipe struct;
	struct kinetis_UART_interrupt_pipe_t *UART_rx_pipe = kernel_malloc(sizeof(struct kinetis_UART_interrupt_pipe_t));

	//Create the output (rx) stream;
	struct kinetis_UART_stream_t output_stream_init = {

		//Set the stream to always write to D, and read S1 before the end;
		.stream = {

			//Data query must read S1, but DMA is supported;
			.type = STREAM_DMA_COMPLIANT_MEMORY,

			//Output stream;
			.input_stream = false,

			//Blocs are 1 byte long;
			.block_size = 1,

			//Transfer the ownership of the allocated (but not initialised) pipe;
			.owned_pipe = (struct stream_pipe_t *const) UART_rx_pipe,

			//No pipe registered;
			.registered_pipe = 0,

			//Transfer access to uart streams functions;
			.get_memory_map = (void (*)(const struct stream_t *, struct mem_map_t *)) &get_memory_map,

			.get_available_blocks = (size_t (*)(const struct stream_t *)) output_bytes_available,

			.transfer_blocks = (size_t (*)(struct stream_t *, const struct mem_map_t *, const struct blocks_desc_t *))
				&UART_read,

			.discard_blocks = (void (*)(struct stream_t *, size_t)) 0,

			.deleter = (void (*)(struct stream_t *)) UART_stream_delete,

		},

		//Save the register to read before the last byte copy;
		//Save registers;
		.S1 = S1_reg,
		.D = D_reg,
		.CFIFO = RCFIFO_reg,

		//Save the rx fifo size;
		.fifo_size = rx_fifo_size,

	};

	//Initialise the rx stream;
	driver_data->output_stream = kernel_malloc_copy(sizeof(struct kinetis_UART_stream_t), &output_stream_init);

	//Create the rx pipe initializer;
	struct kinetis_UART_interrupt_pipe_t output_pipe_init = {

		//Leave uninitialised for instance;
		.pipe = {},

		//Copy C2's address;
		.C2 = C2_reg,

		//Copy the address of RXWATER;
		.WATER = RXWATER_reg,

		//Save the rx fifo size;
		.fifo_size = rx_fifo_size,

	};

	//Initialise pipe part of the initializer, providing the rx stream's reference;
	interrupt_pipe_initialise(

		//The pipe to init;
		&output_pipe_init.pipe,

		//The owner stream reference;
		(struct stream_t *) driver_data->output_stream,

		//The watermark updater;
		(void (*)(struct interrupt_pipe_t *, size_t)) &update_rx_watermark,

		//The trigger enabler;
		(void (*)(struct interrupt_pipe_t *)) &enable_rx_trigger,

		//The trigger disabler;
		(void (*)(struct interrupt_pipe_t *)) &disable_rx_trigger,

		//The destructor's reference;
		(void (*)(struct interrupt_pipe_t *)) &kinetis_interrupt_pipe_destroy

	);

	//Initialise the rx pipe;
	memcpy(UART_rx_pipe, &output_pipe_init, sizeof(struct kinetis_UART_interrupt_pipe_t));


	//------------------ TX (OUTPUT) STREAM - PIPE INIT ------------------
	//Pre-allocate memory for the pipe struct;
	struct kinetis_UART_interrupt_pipe_t *UART_tx_pipe = kernel_malloc(sizeof(struct kinetis_UART_interrupt_pipe_t));

	//Create the input (tx) stream;
	struct kinetis_UART_stream_t input_stream_init = {

		//Set the stream to always write to D, and read S1 before the end;
		.stream = {

			//Data query must read S1, but DMA is supported;
			.type = STREAM_DMA_COMPLIANT_MEMORY,

			//Input stream;
			.input_stream = true,

			//Blocs are 1 byte long;
			.block_size = 1,

			//No pipe owned;
			.owned_pipe = (struct stream_pipe_t *const) UART_tx_pipe,

			//No pipe registered;
			.registered_pipe = 0,


			//Provide access to stream functions;
			.get_memory_map = (void (*)(const struct stream_t *, struct mem_map_t *)) &get_memory_map,

			.get_available_blocks = (size_t (*)(const struct stream_t *)) input_spaces_available,

			.transfer_blocks =
			(size_t (*)(struct stream_t *, const struct mem_map_t *, const struct blocks_desc_t *))
				&UART_write,

			.discard_blocks = (void (*)(struct stream_t *, size_t)) 0,

			.deleter = (void (*)(struct stream_t *)) UART_stream_delete,

		},

		//Save registers;
		.S1 = S1_reg,
		.D = D_reg,
		.CFIFO = TCFIFO_reg,

		//Save the tx fifo size;
		.fifo_size = tx_fifo_size,

	};

	//Initialise the tx stream;
	driver_data->input_stream = kernel_malloc_copy(sizeof(struct kinetis_UART_stream_t), &input_stream_init);

	//Create the tx pipe;
	struct kinetis_UART_interrupt_pipe_t input_pipe_init = {

		//Leave uninitialised for instance;
		.pipe = {},

		//Copy C2's address;
		.C2 = C2_reg,

		//Copy the address of TXWATER;
		.WATER = TXWATER_reg,

		//Save the fifo size;
		.fifo_size = tx_fifo_size,

	};

	//Initialise pipe part of the initializer, providing the tx stream's reference;
	interrupt_pipe_initialise(

		//The pipe to init;
		&input_pipe_init.pipe,

		//The owner stream reference;
		(struct stream_t *) driver_data->input_stream,

		//The watermark updater;
		(void (*)(struct interrupt_pipe_t *, size_t)) &update_tx_watermark,

		//The trigger enabler;
		(void (*)(struct interrupt_pipe_t *)) &enable_tx_trigger,

		//The trigger disabler;
		(void (*)(struct interrupt_pipe_t *)) &disable_tx_trigger,

		//The destructor's reference;
		(void (*)(struct interrupt_pipe_t *)) &kinetis_interrupt_pipe_destroy

	);

	//Initialise the tx pipe;
	memcpy(UART_tx_pipe, &input_pipe_init, sizeof(struct kinetis_UART_interrupt_pipe_t));

	//Initialise the hardware in a safe state;
	start_peripheral(peripheral_data);

}


/*
 * kinetis_UART_start : this function stops the UART and resets the hardware;
 */

void kinetis_UART_stop(const struct kinetis_UART_driver_t *driver_data) {

	//Cache the hardware struct;
	const struct kinetis_UART_hw_t *peripheral_data = &driver_data->peripheral_data;

	//Delete both streams;
	stream_delete((struct stream_t *) driver_data->input_stream);
	stream_delete((struct stream_t *) driver_data->output_stream);

	//Initialise the hardware in a safe state;
	stop_peripheral(peripheral_data);
}


//---------------------------------------------------- Stream sizes ----------------------------------------------------

/*
 * get_memory_map : provides access to the D register;
 */

static void get_memory_map(const struct kinetis_UART_stream_t *const stream, struct mem_map_t *const map) {

	//Create the initializer of memory map;
	struct mem_map_t init = {
		.block_spacing = 0,
		.start_address = stream->D,
	};

	//Initialise the memory map;
	memcpy(map, &init, sizeof(struct mem_map_t));

}


/*
 * input_spaces_available : determines the number of spaces available in the input (tx) stream;
 */

size_t input_spaces_available(const struct kinetis_UART_stream_t *const stream) {

	//Return the number of spaces available in the hardware fifo;
	return stream->fifo_size - *(stream->CFIFO);
}


/*
 * output_bytes_available : determines the number of bytes available in the output (rx) stream;
 */

size_t output_bytes_available(const struct kinetis_UART_stream_t *const stream) {

	//Return the number of bytes available in the hardware fifo;
	return *(stream->CFIFO);

}


//-------------------------------------------------- Stream transfers --------------------------------------------------


/*
 * UART_read : this function copies the required amount of data from the hardware rx buffer to the provided
 * 	memory zone;
 */

void UART_read(const struct kinetis_UART_stream_t *stream, const struct mem_map_t *src_map,
			   const struct blocks_desc_t *descriptor) {

	//Cache the stream's D register;
	volatile uint8_t *volatile const D = stream->D;

	//Cache the number of bytes to copy;
	size_t transfer_size = descriptor->nb_blocks;

	//Cache the memory start;
	volatile uint8_t *volatile src = src_map->start_address;

	//Cache the memory spacing;
	size_t spacing = src_map->block_spacing;

	//For each element to transfer to the hardware buffer :
	while (transfer_size--) {

		//If the current element is the last one that can be transferred, read S1 to clear the interrupt;
		if (!transfer_size) {

			//Read S1;
			volatile uint8_t s1 = *(stream->S1);

		}

		//Copy D's content into src;
		*src = *D;

		//Update src;
		src += spacing;

	}

}


/*
 * UART_write : this function copies the required amount of data from the src memory zone to
 *  the hardware tx buffer;
 */

void UART_write(const struct kinetis_UART_stream_t *stream, const struct mem_map_t *src_map,
				const struct blocks_desc_t *descriptor) {

	//Cache the stream's D register;
	volatile uint8_t *volatile const D = stream->D;

	//Cache the number of bytes to copy;
	size_t transfer_size = descriptor->nb_blocks;

	//Cache the memory start;
	volatile uint8_t *volatile src = src_map->start_address;

	//Cache the memory spacing;
	size_t spacing = src_map->block_spacing;

	//For each element to transfer to the hardware buffer :
	while (transfer_size--) {

		//If the current element is the last one that can be transferred, read S1 to clear the interrupt;
		if (!transfer_size) {

			//Read S1;
			volatile uint8_t s1 = *(stream->S1);

		}

		//Copy the byte in D;
		*D = *src;

		//Update src;
		src += spacing;

	}

}


//--------------------------------------------------- Pipe functions ---------------------------------------------------

//Enable the rx trigger;
static void enable_rx_trigger(const struct kinetis_UART_interrupt_pipe_t *const pipe) {

	//Set the RIE bit in C2;
	*(pipe->C2) |= UART_C2_RIE;

}

//Disable the rx trigger;
static void disable_rx_trigger(const struct kinetis_UART_interrupt_pipe_t *const pipe) {

	//Set the RIE bit in C2;
	*(pipe->C2) &= ~UART_C2_RIE;

}

//Enable the tx trigger;
static void enable_tx_trigger(const struct kinetis_UART_interrupt_pipe_t *const pipe) {

	//Set the RIE bit in C2;
	*(pipe->C2) |= UART_C2_TIE;

}

//Disable the rx trigger;
static void disable_tx_trigger(const struct kinetis_UART_interrupt_pipe_t *const pipe) {

	//Set the RIE bit in C2;
	*(pipe->C2) &= ~UART_C2_TIE;

}


//Update the rx watermark;
static void update_rx_watermark(const struct kinetis_UART_interrupt_pipe_t *const pipe, const size_t nb_bytes) {

	//The rx fifo watermark register determines the number of bytes that trigger an INT or DMA; Just update it;
	*(pipe->WATER) = (uint8_t) nb_bytes;

}


//Update the tx watermark;
static void update_tx_watermark(const struct kinetis_UART_interrupt_pipe_t *const pipe, const size_t nb_spaces) {

	//The tx fifo watermark register determines the number of bytes that trigger an INT or DMA;
	//To determine the number of bytes associated with the required number of spaces, subtract it to the fifo size;
	*(pipe->WATER) = pipe->fifo_size - (uint8_t) nb_spaces;


}
//----------------------------------------------------- Exceptions -----------------------------------------------------

/*
 * interrupt : this function processes UART's interrupts : Rx, and Tx;
 */

void kinetis_UART_status_interrupt(const struct kinetis_UART_driver_t *driver_data) {

	//Cache the register pointer;
	struct kinetis_UART_registers_t *registers = driver_data->peripheral_data.registers;

	//Cache C2, S1, and C5;
	uint8_t C2 = registers->C2, S1 = registers->S1, C5 = registers->C5;

	//TODO IDLE LINE;

	//TODO TRANSMISSION COMPLETE;

	//If the rx interrupt is enabled, if its flag is asserted, and interrupt request should be made :
	if ((C2 & UART_C2_RIE) && (S1 & UART_S1_RDRF) &&  (!(C5 &UART_C5_RDMAS))) {

		//teensy35_led_count(3);

		//Execute the driver's output pipe's transfer;
		interrupt_pipe_transfer((struct interrupt_pipe_t *) driver_data->output_stream->stream.owned_pipe);

	}

	//If the tx interrupt is enabled, if its flag is asserted, and interrupt request should be made :
	if ((C2 & UART_C2_TIE) && (S1 & UART_S1_TDRE) && (!(C5 &UART_C5_TDMAS))) {

		//teensy35_led_count(4);

		//Execute the driver's input pipe's transfer;
		interrupt_pipe_transfer((struct interrupt_pipe_t *) driver_data->input_stream->stream.owned_pipe);

	}

	//teensy35_led_count(6);

}


/*
 * error : this function is called on an interrupt basis each time an error is detected by the hardware.
 *
 *  It supports different errors, that are described in the code below.
 */

void kinetis_UART_error_interrupt(const struct kinetis_UART_driver_t *driver_data) {

	/*
	 * Errors supported are :
	 *  - framing errors (locking) - read and forget last element;
	 *  - receiver overrun (locking) - receiver overflow (same ones) -> receiver flush;
	 *  - receiver underflow (misalignment) -> receiver flush;
	 *  - transmitter overflow (misalignment) -> transmitter flush;
	 *  - noise flag - parity error (informative) -> read and forget last element;
	 */

	//Cache the peripheral data pointer;
	const struct kinetis_UART_hw_t *const peripheral_data = &driver_data->peripheral_data;

	//Cache the register pointer;
	struct kinetis_UART_registers_t *const registers = peripheral_data->registers;

	//Unlock reception by checking the framing error flag. At the same time, check noise or parity;
	if (registers->C2 & (UART_S1_FE | UART_S1_NF | UART_S1_PF)) {

		//In case of framing - parity - noise error, we read and discard;

		//TODO VOLATILE CHECK
		//Read D, to discard and clear flags;
		registers->D;

	}

	//Cache C2 (contains the state of tx and rx);
	uint8_t C2 = registers->C2;

	//If there was a receiver overrun or overflow :
	if (registers->SFIFO & (UART_SFIFO_RXOF | UART_SFIFO_RXUF)) {

		//Read D to clear the flag;

		//TODO VOLATILE CHECK
		//Read S1 and Dto discard and clear flags;
		registers->S1;
		registers->D;

		//Turn off rx;
		CLEAR(registers->C2, UART_C2_RE, 8);

		//Then, flush the rx fifo;
		registers->CFIFO |= UART_CFIFO_RXFLUSH;


	}

	//If there was a transmitter overflow :
	if (registers->SFIFO | UART_SFIFO_TXOF) {

		//Turn off tx;
		CLEAR(registers->C2, UART_C2_TE, 8);

		//Then, flush the tx fifo;
		registers->CFIFO |= UART_CFIFO_TXFLUSH;


	}


	//Finally, clear RXOF and RXUF flags by writing a 1 to them
	registers->SFIFO = UART_SFIFO_RXOF | UART_SFIFO_RXUF | UART_SFIFO_TXOF;

	//If SFIFO flags are not cleared :
	if (registers->SFIFO & (UART_SFIFO_RXOF | UART_SFIFO_RXUF | UART_SFIFO_TXOF)) {
		teensy35_led_count(2);
	}

	//If the s1 flag is not cleared :
	if (registers->S1 & UART_S1_OR) {
		teensy35_led_count(4);

	}

	//Restore C2's previous state (will re-update rx or tx if necessary;
	registers->C2 = C2;

}

//-------------------------------------------------- watermarks --------------------------------------------------

/*
 * update_tx_water :
 *
 *  - Enables the reception interrupt. If called by a connection, will allow automatic processing. If called by an
 *      interrupt, won't change anything;
 *  - Determines the safest number of transmissions to execute;
 *  - Determines the final number of elements in the transmission buffer;
 *  - Determines the final watermark;
 */

/*
uint8_t update_tx_watermark(const struct kinetis_UART_hw_t *const data, uint8_t nb_transmissions) {

	//To have a faster access, cache the register struct's pointer;
	struct kinetis_UART_registers_t *const registers = data->registers;

	//Activate the reception interrupt;
	SET(registers->C2, UART_C2_TIE, 8);

	//Cache the number of elements in the hardware buffer;
	const uint8_t hw_tx_nb_elements = registers->TCFIFO;

	//Cache the number of spaces in the hardware buffer;
	const uint8_t hw_tx_nb_spaces = data->tx_size - hw_tx_nb_elements;

	//If we must transmit more characters that we can, major the size to transfer;
	if (nb_transmissions > hw_tx_nb_spaces) nb_transmissions = hw_tx_nb_spaces;

	//Determine the final number of uint16_t in the hw buffer;
	const uint8_t final_nb = hw_tx_nb_elements + nb_transmissions;

	//Call the watermark specific function;
	registers->TWFIFO = data->tx_water_function(final_nb);

	//Return the correct number of transmissions to execute;
	return nb_transmissions;

}
*/

//-------------------------- Reception methods --------------------------


/*
 * update_tx_water :
 *  - Enabled the reception interrupt;
 *  - Determines the safest maximum number of readings to perform;
 *  - Determine the final number of elements in the hardware rx buffer;
 *  - Ca
 *
 * his function updates the rx watermark in prevision of a determined number of readings.
 *  It also corrects and returns the number of readings so that there are no underflows;
 */

/*
uint8_t update_rx_watermark(const struct kinetis_UART_hw_t *const data, uint8_t nb_readings) {

	//To have a faster access, cache the register struct's pointer;
	struct kinetis_UART_registers_t *const registers = data->registers;

	//Activate the reception interrupt;
	SET(registers->C2, UART_C2_RIE, 8);

	//Cache the number of uint16_t in the rx hardware buffer;
	const uint8_t hw_rx_nb = registers->RCFIFO;

	//Major the transfer size by the number of uint16_t in the hardware buffer;
	if (hw_rx_nb < nb_readings) nb_readings = hw_rx_nb;

	//Determine the final number of uint16_t in the hw buffer;
	const uint8_t final_nb = hw_rx_nb - nb_readings;

	//Call the watermark specific function;
	registers->RWFIFO = data->rx_water_function(final_nb, data->rx_size);

	//Return the safest maxmimum number of readings to perform;
	return nb_readings;

}*/


//-------------------------------------------------- uint16_tansfer --------------------------------------------------

/*
 *

void UART_write(const struct kinetis_UART_stream_t *const stream, struct mem_desc_t *src) {

	//Cache src in the correct type;
	const uint16_t *src = src_c;

	//As checking the packet mode takes more processing time than just send the 9-th bit, we won't check it.
	struct kinetis_UART_registers_t *const registers = instance->memory;

	//Update the watermark and determine the number of transmissions to execute;
	uint8_t nb_transmissions = update_tx_watermark(instance, (uint8_t) size);

	//For each element to transfer to the hardware buffer :
	while (nb_transmissions--) {

		//If the current element is the last one that can be transferred, read S1 to clear the interrupt;
		if (!nb_transmissions) {

			//TODO VOLATILE CHECK
			//Read S1;
			registers->S1;

		}

		//Get an element from the buffer;
		uint16_t element = *(src++);

		//If we must send the 9-th bit (ie the 9-th bit is set, b100000000 = 256
		if (element & (uint16_t) 256) {

			//Set T8, bit 6 of C3;
			SET(registers->C3, UART_C3_T8, 8);

		} else {

			//Set T8, bit 6 of C3;
			CLEAR(registers->C3, UART_C3_T8, 8);

		}

		//Now, copy the 8 first bits in D to complete the transmission;
		registers->D = (uint8_t) element;

	}

}


void kinetis_UART_copy_from_rx(const struct kinetis_UART_hw_t *const instance, void *dest_c, size_t size) {

	//Cache src in the correct type;
	uint16_t *dest = dest_c;

	//To have a faster access, cache the register struct's pointer;
	struct kinetis_UART_registers_t *const registers = instance->memory;

	//Update the rw watermark according to the transfer size, and get the safe number of readings to do;
	uint8_t nb_readings = update_rx_watermark(instance, (uint8_t) size);

	//For each element to transfer from the hw buffer :
	while (nb_readings--) {

		//If this cycle is the last one, read S1 to clear the interrupt;
		if (!nb_readings) {

			//TODO VOLATILE CHECK
			//Read S1;
			registers->S1;

		}

		//First, cache the return value;
		uint16_t ret_value = 0;

		//If the transmission mode is set to almost 9 bits and R8 is set :
		if ((registers->C1 & UART_C1_M) && (registers->C3 & UART_C3_R8)) {

			//Set the bit 8 of ret_value
			SET_BIT(ret_value, 8, 16);

		}

		//Add D's value to the return value;
		SET(ret_value, (uint16_t) (uint8_t) registers->D, 16);

		//Return the received value;
		*(dest++) = ret_value;

	}

}
 */
