//
// Created by root on 8/10/18.
//

#ifndef TRACER_NVIC_H
#define TRACER_NVIC_H

#include <stdint.h>

#include "arm_v7m.h"

//Update the nmi handler;//TODO CORRECT COOMMENTS
//Update the nmi handler;//TODO CORRECT COOMMENTS
//Update the nmi handler;//TODO CORRECT COOMMENTS
//Update the nmi handler;//TODO CORRECT COOMMENTS
//Update the nmi handler;//TODO CORRECT COOMMENTS
//Update the nmi handler;//TODO CORRECT COOMMENTS
//Update the nmi handler;//TODO CORRECT COOMMENTS
//Update the nmi handler;//TODO CORRECT COOMMENTS
//Update the nmi handler;//TODO CORRECT COOMMENTS


/*
 * This library contains startup code and interrupt management function for ARM chip built with the
 * 	Nested Vector Interrupt Controller module;
 *
 * 	The role of the NVIC is to manage interrupt function handlers, and, as a consequence, to define the code's entry
 * 	point.
 *
 * 	The library offers :
 * 		- all standard in-flash NVIC capabilities (prio management);
 * 		- ISR update in real time, for all interrupts except the reset interrupts that is fixed;
 * 		- NVIC relocation in RAM, allows more efficient ISR modifications;
 *
 * 	To avoid missuses, a distinction is made between system (pendSV, systick, SVC, faults, etc...) and other
 * 	interrupts.
 *
 * 	System interrupts have their own functions, that only concerns them;
 *
 * 	Non-system interrupts are updated using unified functions;
 *
 */


/*
 * This enum presents all system interrupts that can be parametrised;
 */

enum nvic_exception {

	//Channel 2 : The Non Maskable Interrupt;
	NVIC_NMI = 2,

	//Channel 3 : The Hardware Fault Interrupt;
	NVIC_HARD_FAULT = 3,

	//Channel 4 : The Memory Fault Interrupt;
	NVIC_MEM_FAULT = 4,

	//Channel 5 : The Bus Fault Interrupt;
	NVIC_BUS_FAULT = 5,

	//Channel 6 : The Usage Fault Interrupt;
	NVIC_USAGE_FAULT = 6,

	//Channel 11 : The Supervisor Call Interrupt;
	NVIC_SVC = 11,

	//Channel 14 : The PensSV Interrupt;
	NVIC_PENDSV = 14,

	//Channel 15 : The Systick Interrupt;
	NVIC_SYSTICK = 15,

};

//------------------------------------- Interrupts -------------------------------------

/*
 * Priority levels : these are the standard interrupt priorities for chip;
 *
 * Cortex-M4: 0,16,32,48,64,80,96,112,128,144,160,176,192,208,224,240
 */


//The standard priority for status interrupt;
#define DRIVER_STARUS_INTERRUPT_PRIORITY 32

//The standard priority for error interrupt;
#define DRIVER_ERROR_INTERRUPT_PRIORITY 16


//------------------------------------------------- System interrupts --------------------------------------------------

//Enable a system interrupt;
void nvic_enable_exception(enum nvic_exception channel);

//Disable a system interrupt;
void nvic_disable_exception(enum nvic_exception channel);

//Set an exception pending;
void nvic_set_exception_pending(enum nvic_exception channel);

//Clear a system interrupt's pending state;
void nvic_clear_exception_pending(enum nvic_exception channel);

//Is an exception in the pending state;
bool nvic_is_exception_pending(enum nvic_exception channel);

void nvic_set_exception_priority(enum nvic_exception channel, uint8_t priority);

//Get a system exception priority;
void nvic_get_exception_priority(enum nvic_exception channel, uint8_t priority);

//Update an exception handler;
void nvic_set_exception_handler(enum nvic_exception channel, void (*handler)());



//---------------------------------------------------- Handler reset ----------------------------------------------------
//TODO RESET HEADERS






#endif //TRACER_NVIC_H
