/*
  K64_UART.h Part of TRACER

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


//TODO COPYRIGHT PJRC.COM LLC EN PLUS
//

#ifndef TRACER_KINETIS_UART_H
#define TRACER_KINETIS_UART_H

#include <kernel/arch/drivers/uart.h>

#include <kernel/memory/memory_stream.h>

#include <kernel/memory/interrupt_pipe.h>
#include <kernel/net/netf.h>

//----------------------------------------------------- Memory Map -----------------------------------------------------

/*
 * First, we define the UART memory map. As all data related to a specific UART are in a continuous memory zone,
 *  We can assimilate them as a struct, whose address is known in memory;
 */

struct __attribute__ ((packed)) K64_UART_registers {
	volatile uint8_t BDH;
	volatile uint8_t BDL;
	volatile uint8_t C1;
	volatile uint8_t C2;
	volatile uint8_t S1;
	volatile uint8_t S2;
	volatile uint8_t C3;
	volatile uint8_t D;
	volatile uint8_t MA1;
	volatile uint8_t MA2;
	volatile uint8_t C4;
	volatile uint8_t C5;
	volatile uint8_t ED;
	volatile uint8_t MODEM;
	volatile uint8_t IR;
	volatile uint8_t unused1;
	volatile uint8_t PFIFO;
	volatile uint8_t CFIFO;
	volatile uint8_t SFIFO;
	volatile uint8_t TWFIFO;
	volatile uint8_t TCFIFO;
	volatile uint8_t RWFIFO;
	volatile uint8_t RCFIFO;
	volatile uint8_t unused2;
	volatile uint8_t C7816;
	volatile uint8_t IE7816;
	volatile uint8_t IS7816;
	union {
		volatile uint8_t WP7816T0;
		volatile uint8_t WP7816T1;
	};
	volatile uint8_t WN7816;
	volatile uint8_t WF7816;
	volatile uint8_t ET7816;
	volatile uint8_t TL7816;
	volatile uint8_t unused3;
	volatile uint8_t C6;
	volatile uint8_t PCTH;
	volatile uint8_t PCTL;
	volatile uint8_t B1T;
	volatile uint8_t SDTH;
	volatile uint8_t SDTL;
	volatile uint8_t PRE;
	volatile uint8_t TPL;
	volatile uint8_t IE;
	volatile uint8_t WB;
	volatile uint8_t S3;
	volatile uint8_t S4;
	volatile uint8_t RPL;
	volatile uint8_t RPREL;
	volatile uint8_t CPW;
	volatile uint8_t RIDT;
	volatile uint8_t TIDT;

};


/*
 * Kinetis UART peripheral : contains hardware specs of the UART;
 *
 * 	This set of data never changes for a given peripheral instance;
 */

struct K64_UART_hw {

	//The address of the peripheral register zone;
	struct K64_UART_registers *const registers;

	//The clock frequency;
	const uint32_t clock_frequency;

	//The status interrupt channel;
	const uint8_t status_int_channel;

	//The error interrupt channel;
	const uint8_t error_int_channel;
	
	//Interrupt link function;
	void (*const status_link)();
	void (*const error_link)();
	
};


/*
 * The K64 UART interface : complements the netf21 with interrupt config registers;
 */

struct K64_UART_net21 {

	//The OSI layer 1-2 interface;
	struct netf21 iface;

	//The address of C2, for interrupt management;
	volatile uint8_t *const C2;

	//The size of the tx hardware fifo;
	const uint8_t tx_fifo_size;

};


/*
 * Kinetis UART driver : contains a hardware specs set and a reference to an interface;
 */

struct K64_UART_driver_t {

	//Hardware specs;
	const struct K64_UART_hw hw_specs;
	
	//The network interface attached. Null if not initialised;
	struct K64_UART_net21 *iface;

};


/*
 * A K64 UART interrupt pipe is composed of the following elements :
 */

/*
struct K64_UART_netf21 {

	//The interrupt pipe base;
	struct netf21 pipe;

	//The address of C2;
	volatile uint8_t *const C2;

	//The address of the watermark register;
	volatile uint8_t *const WATER;

	//The size of the related hardware FIFO;
	const uint8_t fifo_size;

};
*/


/*
 * A K64 UART memory stream is composed of the following elements :
 */

/*
struct K64_UART_stream_t {

	//A stream memory, first for pointer cast;
	struct stream_t stream;

	//The S1 register;
	volatile uint8_t *const S1;

	//The D register;
	volatile uint8_t *const D;

	//The FIFO counter;
	volatile uint8_t *const CFIFO;

	//The FIFO size (used only in input for available spaces);
	const uint8_t fifo_size;

};
 */


//------------------------------------------------- Creation - Deletion ------------------------------------------------

//Create an instance of a K64 UART driver from hardware specs;
struct K64_UART_driver_t *K64_UART_create(struct K64_UART_hw *);

//Delete an instance of a K64 UART driver;
void K64_UART_delete(struct K64_UART_driver_t *);


//---------------------------------------------------- Start - Stop ----------------------------------------------------

//Initialise the UART; The internal network interface is created;
void K64_UART_start(struct K64_UART_driver_t *driver_data, const struct UART_config_t *config);

//De-initialise the UART; The internal network interface is deleted;
void K64_UART_stop(const struct K64_UART_driver_t *driver_data);


//----------------------------------------------------- Interrupts -----------------------------------------------------

/*
 * Exception functions : public to allow the creation of the interrupt link;
 */

//The interrupt function;
void K64_UART_status_interrupt(const struct K64_UART_driver_t *driver_data);

//The error function;
void K64_UART_error_interrupt(const struct K64_UART_driver_t *instance);


#endif //TRACER_TEENSY35_UART_H
