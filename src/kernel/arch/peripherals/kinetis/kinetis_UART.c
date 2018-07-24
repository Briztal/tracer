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
void initialise_peripheral(const struct kinetis_UART_hw_t *peripheral_data);

//Start the peripheral;
void start_peripheral(const struct kinetis_UART_hw_t *driver_data);

//Reset the peripheral in the stopped state;
void reset_peripheral(const struct kinetis_UART_hw_t *driver_data);


//-------------------------- Peripheral config --------------------------

//Configure the packet format;
void configure_packet_format(const struct kinetis_UART_hw_t *data, const struct UART_config_t *);

//Configure the sequences_initialised;
void configure_modem(const struct kinetis_UART_hw_t *data, const struct UART_config_t *);

//Configure the transmission layer;
void configure_transmission_layer(const struct kinetis_UART_hw_t *data, const struct UART_config_t *);



//-------------------------- Stream functions --------------------------

//determine the number of spaces available in the input (tx) stream;
size_t input_spaces_available(const struct kinetis_UART_stream_t *stream);


//determine the number of bytes available in the output (rx) stream;
size_t output_bytes_available(const struct kinetis_UART_stream_t *stream);

//The UART stream discard doesn't alter any memory data; No inactivity supported for instance;
bool discard_blocs(struct kinetis_UART_stream_t *stream) { return true; };

//The UART stream doesn't store any dynamic data;
void UART_stream_delete(struct kinetis_UART_stream_t *stream) {
	kernel_free(stream);
}


//-------------------------- Watermark policies --------------------------

//Update the tx watermark;
uint8_t update_tx_watermark(const struct kinetis_UART_hw_t *data, uint8_t nb_transmissions);

//Update the rx watermark;
uint8_t update_rx_watermark(const struct kinetis_UART_hw_t *data, uint8_t nb_readings);



//-------------------------------------------------- Hardware Settings -------------------------------------------------

/*
 * initialise_peripheral : the initialisation function has to : 
 * 	- set all required perhpheral registers in a safe, stopped and initialised state;
 * 	- disable related interrupts and DMA;
 * 	- update all required interrupt functions and setup DMA if necessary;
 */

void initialise_peripheral(const struct kinetis_UART_hw_t *const peripheral_data) {

	//Cache the memory pointer;
	struct kinetis_UART_registers_t *const registers = peripheral_data->registers;

	//Determine their indices; WARNING : SIZES MUST BE CORRECT;
	uint8_t rx_index = peripheral_data->rx_index;
	uint8_t tx_index = peripheral_data->tx_index;

	//Set both FIFOs enabled, and their size set with provided values,
	registers->PFIFO = (uint8_t) UART_PFIFO_TXFE | (uint8_t) UART_PFIFO_RXFE |
					   (uint8_t) UART_PFIFO_TXFIFOSIZE(tx_index) | (uint8_t) UART_PFIFO_RXFIFOSIZE(rx_index);


	//Disable rx and tx, and related interrupts;
	CLEAR(registers->C2, UART_C2_RE & UART_C2_TE & UART_C2_RIE & UART_C2_TIE, 8);


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
	registers->CFIFO |= UART_CFIFO_RXFLUSH;
	registers->CFIFO |= UART_CFIFO_TXFLUSH;


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

	//Disable receiver overflow, receiver underflow and transmitter overflow exceptions;
	SET(registers->CFIFO, UART_CFIFO_RXOFE | UART_CFIFO_RXUFE | UART_CFIFO_TXOFE, 8);


	//Enable Rx and TX
	SET(registers->C2, UART_C2_RE & UART_C2_TE, 8);

	//Enable the status interrupt;
	core_IC_enable(driver_data->status_int_channel);

	//Disable the status interrupt;
	core_IC_enable(driver_data->error_int_channel);

}


/*
 * reset_peripheral : Stops the peripheral, resets fifos, disable interrupts;
 */

void reset_peripheral(const struct kinetis_UART_hw_t *driver_data) {

	//Cache memory register;
	struct kinetis_UART_registers_t *const registers = driver_data->registers;

	//Disable the status interrupt;
	core_IC_disable(driver_data->status_int_channel);

	//Disable the status interrupt;
	core_IC_disable(driver_data->error_int_channel);

	//Disable Rx and TX
	CLEAR(registers->C2, UART_C2_RE & UART_C2_TE, 8);

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
	reset_peripheral(&driver_data->peripheral_data);

	//Delete the driver data structure;
	kernel_free(driver_data);

}


//--------------------------------------------------- Hardware setup ---------------------------------------------------


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

	//First, cache the data pointer to avoid permanent implicit double pointer access;
	struct kinetis_UART_registers_t *registers = data->registers;


	/*
	 * First, we will configure the transmission type : Full or Half duplex;
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


	//First, determine Kb;
	uint32_t Kb = ((uint32_t) ((uint32_t) (data->clock_frequency << 1)) / config->baudrate);

	//Then, determine SBR. A division by 32 is a shift by 5;
	uint16_t SBR = (uint16_t) (Kb >> 5);

	//Finally, determine SBR. A modulo by 32 is a AND by 32;
	uint8_t BRFA = (uint8_t) Kb & (uint8_t) 32;

	/*
	 * Now we can update SBR, located in the two Baudrate memory;
	 */

	//Copy the first 8 bits of SBR in BDL;
	registers->BDL = (uint8_t) SBR;

	//Clear the first 5 bits 00011111 (31) of BDH :
	CLEAR(registers->BDH, 31, 8);

	//Copy the 5 MSB of SBR in the 5 LSB of BDH;
	SET(registers->BDH, (SBR >> 8), 8);


	/*
	* Now we can update BRFA, located in the 5 first bits of C4;
	*/

	//Clear the 5 first bits 00011111 (31) of C4;
	CLEAR(registers->C4, 31, 8);

	//Copy the new BRFA;
	SET(registers->C4, BRFA, 8);

}


//----------------------------------------------------- Init - exit ----------------------------------------------------

/*
 * kinetis_UART_init : configures the transmission stack, creates streams, and starts the UART;
 */

void kinetis_UART_init(struct kinetis_UART_driver_t *const driver_data, const struct UART_config_t *const config) {

	//Cache the hardware struct;
	struct kinetis_UART_hw_t *peripheral_data = &driver_data->peripheral_data;

	//Initialise different parts of the UART;
	configure_packet_format(peripheral_data, config);
	configure_modem(peripheral_data, config);
	configure_transmission_layer(peripheral_data, config);

	//Cache the register struct pointer;
	struct kinetis_UART_registers_t *const registers = peripheral_data->registers;

	//Cache the D register pointer;
	volatile uint8_t *const D_reg = &registers->D;
	volatile uint8_t *const S1_reg = &registers->S1;

	//Create the input (tx) stream;
	struct kinetis_UART_stream_t input_init = {

		//Set the stream to always write to D, and read S1 before the end;
		.stream = {
			.start_address = D_reg,
			.clear_register = S1_reg,
			.bloc_size = 1,
			.bloc_spacing = 0,
			.increment_positve = true,
			.input_stream = true,
			.available_blocs = (size_t (*const)(struct stream_memory_t *)) input_spaces_available,
			.discard_blocs = (bool (*const)(struct stream_memory_t *, size_t)) discard_blocs,
			.destructor = (void (*const)(struct stream_memory_t *)) UART_stream_delete,
		},

		//Set the size
		.size_register = &registers->TCFIFO,
	};

	//Create the output (rx) stream;
	struct kinetis_UART_stream_t output_init = {

		//Set the stream to always write to D, and read S1 before the end;
		.stream = {
			.start_address = D_reg,
			.clear_register = S1_reg,
			.bloc_size = 1,
			.bloc_spacing = 0,
			.increment_positve = true,
			.input_stream = false,
			.available_blocs = (size_t (*const)(struct stream_memory_t *)) output_bytes_available,
			.discard_blocs = (bool (*const)(struct stream_memory_t *, size_t)) discard_blocs,
			.destructor = (void (*const)(struct stream_memory_t *)) UART_stream_delete,
		},

		//Set the size
		.size_register = &registers->RCFIFO,
	};


	//TODO CREATE STREAMS;

	//Initialise the hardware in a safe sequences_initialised;
	start_peripheral(peripheral_data);

}


/*
 * kinetis_UART_init : this function stops the UART and resets the hardware;
 */

void kinetis_UART_exit(const struct kinetis_UART_driver_t *const driver_data) {

	//Cache the hardware struct;
	struct kinetis_UART_hw_t *peripheral_data = &driver_data->peripheral_data;

	//TODO DELETE STREAMS.

	//Initialise the hardware in a safe sequences_initialised;
	reset_peripheral(peripheral_data);


}


//---------------------------------------------------- Stream sizes ----------------------------------------------------

/*
 * input_spaces_available : determines the number of spaces available in the input (tx) stream;
 */

size_t input_spaces_available(const struct kinetis_UART_stream_t *stream) {

	//Return the fifo size minus the current number of elements in the fifo;
	return stream->fifo_size - *(stream->size_register);

}


/*
 * output_bytes_available : determines the number of bytes available in the output (rx) stream;
 */

size_t output_bytes_available(const struct kinetis_UART_stream_t *const stream) {

	//Cache the number of uint16_t in the rx buffer;
	return *(stream->size_register);

}


//-------------------------- Transmission methods --------------------------

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

/*
 * kinetis_UART_copy_to_tx : this function copies the required amount of data from the src pointer to
 *  the hardware tx buffer;
 *
 *  It calls for watermark update, updates its transfer size, and copies the required amount of data;
 *
 *  If the uint16_t it copies is the last, it reads S1 to clear the interrupt flag;
 */

/*
void kinetis_UART_copy_to_tx(const struct kinetis_UART_hw_t *const instance, void *src_c, const size_t size) {

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


/*
 * kinetis_UART_copy_from_rx : this function copies the required amount of uint16_t from the hardware buffer to the
 *  provided destination;
 *
 *  It calls for watermark update, updates its transfer size, and copies the required amount of data;
 *
 *  Before transferring the last uint16_t, it reads S1 to clear the interrupt flag;
 */

/*
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

//-------------------------- Exceptions --------------------------

/*
 * interrupt : this function processes UART's interrupts : Rx, and Tx;
 */

void kinetis_UART_status_interrupt(const struct kinetis_UART_driver_t *data) {

	//First, cache the register pointer;
	struct kinetis_UART_registers_t *registers = data->peripheral_data.registers;

	//The logical AND of C2 and S1;
	uint8_t masked_flags = registers->C2 & registers->S1;

	//TODO IDLE LINE;

	//TODO TRANSMISSION COMPLETE;

	return ;

	//TODO INTERRUPT STREAM TRANSFER EXECUTION;

	//First, if the rx interrupt is enabled, and if its flag is asserted :
	if (masked_flags & UART_C2_RIE & UART_S1_RDRF) {

		//Execute the reception cflux;
		//flux_process(data->rx_flux);

		//If the interrupt is still active, the cflux didn't succeed in transferring data;
		if (registers->S1 & UART_S1_RDRF) {

			/*
			 * The flux had no data to proceed in spite of spaces in the transmission buffer;
			 * There is no data available in the other side of the flux, and only an external call will provide some;
			 * There is no reason to keep the interrupt active. We will disable the interrupt;
			 *
			 * We will keep the watermark like it is currently, if the interrupt is reactivated by mistake (lol)
			 *  it could provide some data;
			 */

			//Clear RIE bit of uint8_t C2 to disable the reading interrupt;
			CLEAR(registers->C2, UART_C2_RIE, 8);

		}

	}

	//Then, is the tx interrupt is enabled, and if its flag is asserted :
	if (masked_flags & UART_C2_TIE & UART_S1_TDRE) {

		//Execute the transmission cflux;
		//flux_process(data->tx_flux);

		//If the interrupt is still active, the stream didn't succeed in transferring data;
		if (registers->S1 & UART_S1_TDRE) {

			/*
			 * The flux had no data to proceed in spite of elements in the reception buffer;
			 * There is no data available in the other side of the flux, and only an external call will provide some;
			 * There is no reason to keep the interrupt active. We will disable the interrupt;
			 *
			 * We will keep the watermark like it is currently, if the interrupt is reactivated by mistake (lol)
			 *  it could provide some data;
			 */

			//Clear RIE bit of uint8_t C2 to disable the reading interrupt;
			CLEAR(registers->C2, UART_C2_TIE, 8);

		}

	}

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
	struct kinetis_UART_hw_t *const peripheral_data = &driver_data->peripheral_data;

	//Cache the register pointer;
	struct kinetis_UART_registers_t *registers = peripheral_data->registers;

	//Unlock reception by checking the framing error flag. At the same time, check noise or parity;
	if (registers->C2 & (UART_S1_FE | UART_S1_NF | UART_S1_PF)) {

		//In case of framing - parity - noise error, we read and discard;

		//TODO VOLATILE CHECK
		//Read D, to discard and clear flags;
		registers->D;

	}

	//Cache C2 (contains the sequences_initialised of tx and rx);
	uint8_t C2 = registers->C2;

	//If there was a receiver overrun or overflow :
	if ((registers->C2 & UART_S1_OR) | (registers->SFIFO & (UART_SFIFO_RXOF | UART_SFIFO_RXUF))) {

		//First, read D to clear the flag;

		//TODO VOLATILE CHECK
		//Read D, to discard and clear flags;
		registers->D;

		//Turn off rx;
		CLEAR(registers->C2, UART_C2_RE, 8);

		//Then, flush the rx fifo;
		registers->CFIFO |= UART_CFIFO_RXFLUSH;

		//Finally, clear RXOF and RXUF flags by writing a 1 to them
		registers->SFIFO = UART_SFIFO_RXOF | UART_SFIFO_RXUF;


	}

	//If there was a transmitter overflow :
	if (registers->SFIFO | UART_SFIFO_TXOF) {

		//Turn off tx;
		CLEAR(registers->C2, UART_C2_TE, 8);

		//Then, flush the tx fifo;
		registers->CFIFO |= UART_CFIFO_TXFLUSH;

		//Finally, clear the TXOF flag by writing a 1 to it;
		registers->SFIFO = UART_SFIFO_TXOF;

	}

	//Restore C2's previous sequences_initialised (will re-update rx or tx if necessary;
	registers->C2 = C2;

}
