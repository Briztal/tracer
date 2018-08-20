//
// Created by root on 8/10/18.
//

#ifndef TRACER_NVIC_H
#define TRACER_NVIC_H

#include <stdint.h>

#include "arm_v7m.h"


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

enum ic_system_exception {

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


//--------------------------------------------- Global interrupts settings ---------------------------------------------


/**
 * ic_enable_interrupts : enables the interrupt control;
 */

static inline void ic_enable_interrupts() {
	__asm__ volatile("cpsie i":::"memory");
}


/**
 * ic_disable_interrupts : enables the interrupt control;
 */

static inline void ic_disable_interrupts() {
	__asm__ volatile("cpsid i":::"memory");
}


//-------------------------------------------------- Interrupts enable -------------------------------------------------

//Enable a system interrupt;
void ic_enable_system_exception(enum ic_system_exception channel);


/**
 * ic_enable_interrupt : enables the required non-system interrupt channel;
 * @param channel : the channel to enable;
 */

static inline void ic_enable_interrupt(uint8_t channel) {
	armv7m_nvic_enable_interrupt(channel);
}


//-------------------------------------------------- Interrupts disable ------------------------------------------------

//Disable a system interrupt;
void ic_disable_system_exception(enum ic_system_exception channel);


/**
 * ic_disable_interrupt : disables the required non-system interrupt channel;
 * @param channel : the channel to disable;
 */

static inline void ic_disable_interrupt(uint8_t channel){
	armv7m_nvic_disable_interrupt(channel);
}


//------------------------------------------------ Set interrupt pending -----------------------------------------------

//Set a system interrupt pending;
void ic_set_system_exception_pending(enum ic_system_exception channel);


/**
 * ic_set_interrupt_pending : sets the required non-system interrupt in the pending state;
 * @param channel : the channel to set in the pending state;
 */

static inline void ic_set_interrupt_pending(uint8_t channel) {
	armv7m_nvic_set_interrupt_pending(channel);
}


//----------------------------------------------- Clear interrupt pending ----------------------------------------------

//Clear a system interrupt's pending state;
void ic_clear_system_exception_pending(enum ic_system_exception channel);


/**
 * ic_set_interrupt_pending : sets the required non-system interrupt in the not-pending state;
 * @param channel : the channel to set in the not-pending state;
 */

static inline void ic_clear_interrupt_pending(uint8_t channel) {
	armv7m_nvic_clear_interrupt_pending(channel);
}


//----------------------------------------------- Are interrupt pending ----------------------------------------------

//Is a system interrupt in the pending state;
bool ic_is_system_exception_pending(enum ic_system_exception channel);


/**
 * ic_is_interrupt_pending : sets the required non-system interrupt in the not-pending state;
 * @param channel : the channel to set in the not-pending state;
 */

static inline void ic_is_interrupt_pending(uint8_t channel) {
	armv7m_nvic_clear_interrupt_pending(channel);
}





//------------------------------------------ System interrupts priority update -----------------------------------------

//Update the nmi handler;//TODO CORRECT COOMMENTS
//Update the nmi handler;//TODO CORRECT COOMMENTS
//Update the nmi handler;//TODO CORRECT COOMMENTS
//Update the nmi handler;//TODO CORRECT COOMMENTS
//Update the nmi handler;//TODO CORRECT COOMMENTS
//Update the nmi handler;//TODO CORRECT COOMMENTS
//Update the nmi handler;//TODO CORRECT COOMMENTS
//Update the nmi handler;//TODO CORRECT COOMMENTS


void ic_set_system_exception_priority(enum ic_system_exception channel, uint8_t priority);


/**
 * ic_set_interrupt_priority : applies the provided priority to the required non-system interupt channel;
 *
 * @param channel : the channel to apply the priority;
 * @param priority : the priority to apply
 */

static inline void ic_set_interrupt_priority(uint8_t channel, uint8_t priority) {
	armv7m_nvic_set_priority(channel, priority);
}


//------------------------------------------ System interrupts priority query -----------------------------------------

//Get a system exception priotity;
void ic_get_system_exception_priority(enum ic_system_exception channel, uint8_t priority);


/**
 * ic_get_interrupt_priority : applies the provided priority to the required non-system interupt channel;
 *
 * @param channel : the channel to apply the priority;
 * @param priority : the priority to apply
 */

static inline uint8_t ic_get_interrupt_priority(uint8_t channel, uint8_t priority) {
	return armv7m_nvic_get_priority(channel);
}


//--------------------------------------------------- Handler update ---------------------------------------------------

//Update a system interrupt handler;
void ic_set_system_exception_handler(enum ic_system_exception channel, void (*handler)());

//Update a non-system interrupt handler;
void ic_set_interrupt_handler(uint8_t channel, void (*handler)());


//---------------------------------------------------- Handler reset ----------------------------------------------------

/**
 * ic_reset_system_exception_handler : resets the handler of the required system interrupt to 0, isr will return
 * 	immediately;
 *
 * @param channel : the interrupt channel to update;
 */

static inline void ic_reset_system_exception_handler(enum ic_system_exception channel) {
	ic_set_system_exception_handler(channel, 0);
}


/**
 * ic_reset_interrupt_handler : resets the handler of the required channel to 0, isr will return immediately;
 * @param channel : the interrupt channel to update;
 */

static inline void ic_reset_interrupt_handler(uint8_t channel) {
	ic_set_interrupt_handler(channel, 0);
}




#endif //TRACER_NVIC_H
