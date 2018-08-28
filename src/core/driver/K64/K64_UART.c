/*
  K64_UART.c Part of TRACER

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

#include "K64_UART.h"


#include <kernel/syscall.h>

#include <string.h>


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
static void initialise_peripheral(const struct K64_UART_hw *peripheral_data);

//Start the peripheral;
static void start_peripheral(const struct K64_UART_hw *hw_specs);

//Reset the peripheral in the stopped state;
static void stop_peripheral(const struct K64_UART_hw *driver_data);


//-------------------------- Peripheral config --------------------------

//Configure the packet format;
static void configure_packet_format(const struct K64_UART_hw *hw_specs, const struct UART_config_t *);

//Configure the state;
static void configure_modem(const struct K64_UART_hw *hw_specs, const struct UART_config_t *);

//Configure the transmission layer;
static void configure_transmission_layer(const struct K64_UART_hw *hw_specs, const struct UART_config_t *);



//-------------------------- Stream functions TODO NOP --------------------------

//TODO;
void K64_UART_rx_read(const struct K64_UART_driver_t *driver);
void K64_UART_tx_write(const struct K64_UART_driver_t *driver);


//Get the memory map;
//static void get_memory_map(const types K64_UART_stream_t *stream, types mem_map_t *map);

//determine the number of spaces available in the input (tx) stream;
//static size_t input_spaces_available(const struct K64_UART_stream_t *stream);

//determine the number of bytes available in the output (rx) stream;
//static size_t output_bytes_available(const struct K64_UART_stream_t *stream);


//Read data from rx;
//static void UART_read(const types K64_UART_stream_t *stream, const types mem_map_t *src_map,
					  //const struct blocks_desc_t *descriptor);

//Write data in tx;
//static void UART_write(const types K64_UART_stream_t *stream, const types mem_map_t *src_map,
					   //const struct blocks_desc_t *descriptor);
/*
//The UART stream doesn't store any dynamic data;
static void UART_stream_delete(struct K64_UART_stream_t *stream) {
	kernel_free(stream);
}*/


//--------------------------------------------------- Pipe functions ---------------------------------------------------

//Delete the interrupt pipe;
//static void K64_interrupt_pipe_destroy(const struct K64_UART_interrupt_pipe_t *data) {};


//Enable the rx interrupt;
static void enable_rx_interrupt(struct K64_UART_net21 *iface);

//Disable the rx trigger;
//static void disable_rx_trigger(const struct K64_UART_interrupt_pipe_t *pipe);


//Enable the tx interrupt;
static void enable_tx_interrupt(struct K64_UART_net21 *iface);

//Disable the rx interrupt;
//static void disable_tx_trigger(const struct K64_UART_interrupt_pipe_t *pipe);


//Update the rx watermark;
//static void update_rx_watermark(const struct K64_UART_interrupt_pipe_t *pipe, size_t nb_bytes);

//Update the tx watermark;
//static void update_tx_watermark(const struct K64_UART_interrupt_pipe_t *pipe, size_t nb_spaces);



//-------------------------------------------------- Hardware Settings -------------------------------------------------

/*
 * initialise_peripheral : the initialisation function has to : 
 * 	- set all required perhipheral registers in a safe, stopped and initialised state;
 * 	- disable related interrupts and DMA;
 * 	- update all required interrupt functions and setup DMA if necessary;
 */

void initialise_peripheral(const struct K64_UART_hw *const peripheral_data) {

	//Cache the memory pointer;
	struct K64_UART_registers *const registers = peripheral_data->registers;

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

void start_peripheral(const struct K64_UART_hw *hw_specs) {

	//Cache memory register;
	struct K64_UART_registers *const registers = hw_specs->registers;

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

	//Disable interrupts;
	CLEAR(registers->C2, UART_C2_TIE | UART_C2_RIE, 8);

	//Enable Rx and TX
	SET(registers->C2, UART_C2_RE | UART_C2_TE, 8);

	//Enable the status interrupt;
	core_IC_enable(hw_specs->status_int_channel);

	//Disable the status interrupt;
	core_IC_enable(hw_specs->error_int_channel);

}


/*
 * stop_peripheral : Stops the peripheral, resets fifos, disable interrupts;
 */

void stop_peripheral(const struct K64_UART_hw *driver_data) {

	//Cache memory register;
	struct K64_UART_registers *const registers = driver_data->registers;

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
 * K64_UART_create : creates an instance of a K64 UART driver from hardware driver specs;
 */

struct K64_UART_driver_t *K64_UART_create(struct K64_UART_hw *const specs) {

	//Initialise the peripheral;
	initialise_peripheral(specs);

	//Create the driver struct initializer;
	struct K64_UART_driver_t init = {

		//Copy the hardware data set;
		.hw_specs = *specs,

		//The interface will be created at startup;
		.iface = 0,

	};

	//Allocate, initialise and return the driver data structure;
	return kernel_malloc_copy(sizeof(struct K64_UART_driver_t), &init);

}


/*
 * K64_UART_delete : deinitializes the peripheral and deletes the peripheral data structure;
 */

void K64_UART_delete(struct K64_UART_driver_t *driver_data) {

	//Reset the peripheral;
	stop_peripheral(&driver_data->hw_specs);

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

void configure_packet_format(const struct K64_UART_hw *const hw_specs, const struct UART_config_t *const config) {

	//TODO MSB FIRST
	//TODO STOP BITS 1 OR 2 FIRST BYTE OF BAUDRATE REGISTER

	//To avoid implicit double pointer access, we will cache hw_specs;
	struct K64_UART_registers *const registers = hw_specs->registers;

	/*
	 * Number of bits; This peripheral only supports 8 or 9 hw_specs bits;
	 *
	 * The number of bits is defined by bit 4 of C1;
	 */

	//Check the number of bits :
	if (config->nb_data_bits != 8) {

		//Error, for instance only 8 bits are supported;
		kernel_error("K64_UART.c : configure_packet_format : invalid number of hw_specs bits.");

	}


	//8 hw_specs bits, bit M cleared;
	CLEAR(registers->C1, UART_C1_M, 8);

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

		//Finally, if the parity is enabled and there are 9 hw_specs bits, the 10-th bit must be declared;
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

void configure_modem(const struct K64_UART_hw *const hw_specs, const struct UART_config_t *const config) {

	struct K64_UART_registers *const registers = hw_specs->registers;

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
configure_transmission_layer(const struct K64_UART_hw *const hw_specs, const struct UART_config_t *const config) {

	//Cache the hw_specs pointer to avoid permanent implicit double pointer access;
	struct K64_UART_registers *registers = hw_specs->registers;

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
	uint32_t Kb = ((uint32_t) ((uint32_t) (hw_specs->clock_frequency << 1)) / config->baudrate);

	//Set the 5 MSB of Kb in BDH;
	registers->BDH = (uint8_t) ((Kb >> 13) & 0x1F);

	//Set the 8 next MSB of Kb in BDL
	registers->BDL = (uint8_t) ((Kb >> 5));

	//Set 5 LSF of Kb in C4
	registers->C4 = (uint8_t) (Kb & 0x1F);

}


//----------------------------------------------------- Init - exit ----------------------------------------------------

/*
 * K64_UART_start : configures the transmission stack, creates streams, and starts the UART;
 */

void K64_UART_start(struct K64_UART_driver_t *driver_data, const struct UART_config_t *config) {

	//Cache the hardware struct;
	const struct K64_UART_hw *hw_specs = &driver_data->hw_specs;

	//Initialise different parts of the UART;
	configure_packet_format(hw_specs, config);
	configure_modem(hw_specs, config);
	configure_transmission_layer(hw_specs, config);

	//Cache the register struct pointer;
	struct K64_UART_registers *const registers = hw_specs->registers;

	//Cache pointer to C2
	volatile uint8_t *const C2_reg = &registers->C2;


	//Configure C5 so that RX full - TX empty flags generate interrupt; clear C5's TDMAS and RDMAS;
	registers->C5 &= ~(UART_C5_TDMAS | UART_C5_RDMAS);

	//Declare both FIFOs sizes;
	uint8_t rx_fifo_size, tx_fifo_size;

	//Get both FIFOs sizes;
	sizes_from_PFIFO(registers->PFIFO, &rx_fifo_size, &tx_fifo_size);

	//Initialise the ifaces struct;
	struct K64_UART_net21 interface_init = {

		//Initialise the l2 adapter
		.iface = {

			//The l2 interface will be initialised right after;
			.iface = {},

			//Transfer the ownership of the framer;
			.framer = config->framer,

		},

		//Save the address of C2 for interrupt management;
		.C2 = C2_reg,

		.tx_fifo_size = tx_fifo_size,

	};

	//Initialise the layer 2 interface;
	netf2_init(
		&interface_init.iface.iface,
		config->nb_frames,
		config->max_frame_size,
		(void (*)(struct netf2 *)) enable_rx_interrupt,
		(void (*)(struct netf2 *)) enable_tx_interrupt,
		(void (*)(struct netf2 *)) netf21_destruct
	);

	//Allocate and initialise the interface;
	driver_data->iface = kernel_malloc_copy(sizeof(struct K64_UART_net21), &interface_init);

	//Initialise the hardware in a safe state;
	start_peripheral(hw_specs);

	//Attempt to enable both interrupts;
	enable_rx_interrupt(driver_data->iface);
	enable_tx_interrupt(driver_data->iface);

}


/*
 * K64_UART_start : this function stops the UART and resets the hardware;
 */

void K64_UART_stop(const struct K64_UART_driver_t *driver_data) {

	//Cache the hardware struct;
	const struct K64_UART_hw *hw_specs = &driver_data->hw_specs;

	//Delete the interface, calling the superclass deleter;
	netf2_delete((struct netf2 *) driver_data->iface);

	//Reset the hardware in a safe state;
	stop_peripheral(hw_specs);

}


//--------------------------------------------------- Pipe functions ---------------------------------------------------

//Enable the rx trigger;
static void enable_rx_interrupt(struct K64_UART_net21 *const iface) {

	//Initialise the net21 interface decoding;
	bool decoding_authorised = netf21_init_decoding(&iface->iface);

	//If the decoding is not authorised :
	if (!decoding_authorised) {

		//Disable the rx interrupt : clear the RIE bit in C2;
		*(iface->C2) &= ~UART_C2_RIE;

	} else {

		//Enable the rx interrupt : set the RIE bit in C2;
		*(iface->C2) |= UART_C2_RIE;

	}


}


//Enable the tx trigger;
static void enable_tx_interrupt(struct K64_UART_net21 *const iface) {

	//Initialise the net21 interface encoding;
	bool encoding_authorised = netf21_init_encoding(&iface->iface);//TODO ERROR HERE

	//If the decoding is not authorised :
	if (!encoding_authorised) {

		//Disable the tx interrupt : clear the TIE bit in C2;
		*(iface->C2) &= ~UART_C2_TIE;


	} else {

		//Enable the tx interrupt : set the TIE bit in C2;
		*(iface->C2) |= UART_C2_TIE;

	}

}


//----------------------------------------------------- Exceptions -----------------------------------------------------

/*
 * interrupt : this function processes UART's interrupts : Rx, and Tx;
 */

void K64_UART_status_interrupt(const struct K64_UART_driver_t *driver_data) {

	//Cache the register pointer;
	struct K64_UART_registers *registers = driver_data->hw_specs.registers;

	//Cache C2, S1, and C5;
	uint8_t C2 = registers->C2, S1 = registers->S1, C5 = registers->C5;

	//TODO IDLE LINE;

	//TODO TRANSMISSION COMPLETE;

	//If the rx interrupt is enabled, if its flag is asserted, and interrupt request should be made :
	if ((C2 & UART_C2_RIE) && (S1 & UART_S1_RDRF) &&  (!(C5 &UART_C5_RDMAS))) {


		//Read from rx;
		K64_UART_rx_read(driver_data);

	}

	//If the tx interrupt is enabled, if its flag is asserted, and interrupt request should be made :
	if ((C2 & UART_C2_TIE) && (S1 & UART_S1_TDRE) && (!(C5 &UART_C5_TDMAS))) {

		//teensy35_led_count(4);

		//Read from tx;
		K64_UART_tx_write(driver_data);

	}

	//teensy35_led_count(6);

}


/*
 * error : this function is called on an interrupt basis each time an error is detected by the hardware.
 *
 *  It supports different errors, that are described in the code below.
 */

void K64_UART_error_interrupt(const struct K64_UART_driver_t *driver_data) {

	/*
	 * Errors supported are :
	 *  - framing errors (locking) - read and forget last element;
	 *  - receiver overrun (locking) - receiver overflow (same ones) -> receiver flush;
	 *  - receiver underflow (misalignment) -> receiver flush;
	 *  - transmitter overflow (misalignment) -> transmitter flush;
	 *  - noise flag - parity error (informative) -> read and forget last element;
	 */

	//Cache the peripheral data pointer;
	const struct K64_UART_hw *const peripheral_data = &driver_data->hw_specs;

	//Cache the register pointer;
	struct K64_UART_registers *const registers = peripheral_data->registers;

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


	//Restore C2's previous state (will re-update rx or tx if necessary;
	registers->C2 = C2;

}



//-------------------------------------------------- Data transfer --------------------------------------------------


void K64_UART_tx_write(const struct K64_UART_driver_t *const driver) {

	//Cache the net21 interface;
	struct K64_UART_net21 *iface = driver->iface;

	//Cache the hardware specs struct;
	const struct K64_UART_hw *hw_specs = &driver->hw_specs;

	//Cache the tx fifo size;
	uint8_t tx_fifo_size = iface->tx_fifo_size;

	//As checking the packet mode takes more processing time than just send the 9-th bit, we won't check it.
	struct K64_UART_registers *const registers = hw_specs->registers;

	//While there are spaces in the tx buffer;
	while (registers->TCFIFO != tx_fifo_size) {

		//Cache a space for the byte to write;
		uint8_t data;

		//Initialise the byte to write and get a stop request;
		bool data_available = netf_21_get_encoded_byte((struct netf21 *) iface, &data);

		//Read S1;
		registers->S1;

		//Now, copy the byte in D;
		registers->D = data;

		//If no more data is available :
		if (!data_available) {

			//Disable the tx interrupt : clear the TIE bit in C2;
			registers->C2 &= ~UART_C2_TIE;

			//Complete;
			return;

		}

	}

	//All data has been read, the interrupt can remain set;

}


void K64_UART_rx_read(const struct K64_UART_driver_t *const driver) {

	//Cache the net21 interface;
	struct K64_UART_net21 *iface = driver->iface;

	//Cache the hardware specs struct;
	const struct K64_UART_hw *hw_specs = &driver->hw_specs;

	//As checking the packet mode takes more processing time than just send the 9-th bit, we won't check it.
	struct K64_UART_registers *const registers = hw_specs->registers;

	//While there are bytes in the rx buffer;
	while (registers->RCFIFO) {

		//Read S1;
		registers->S1;

		//Get a received byte;
		uint8_t data = registers->D;

		//Initialise the byte to write and get a stop request;
		bool space_available = netf_21_decode_byte((struct netf21 *) iface, data);


		//If no more data is available :
		if (!space_available) {


			//Disable the rx interrupt : clear the TIE bit in C2;
			registers->C2 &= ~UART_C2_RIE;

			//Complete;
			return;

		}

	}

	//All data has been read, the interrupt can remain set;

}