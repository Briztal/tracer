//
// Created by root on 8/10/18.
//

#ifndef TRACER_NVIC_H
#define TRACER_NVIC_H

#include <stdint.h>


/*
 * TODO TRANSMIT REGISTERS ADDRESSES;
 */

// Nested Vectored Interrupt Controller, Table 3-4 & ARMv7 ref, appendix B3.4 (page 750)
#define NVIC_STIR			(*(volatile uint32_t *)0xE000EF00)
#define NVIC_ENABLE_IRQ(n)	(*((volatile uint32_t *)0xE000E100 + ((n) >> 5)) = (1 << ((n) & 31)))
#define NVIC_DISABLE_IRQ(n)	(*((volatile uint32_t *)0xE000E180 + ((n) >> 5)) = (1 << ((n) & 31)))
#define NVIC_SET_PENDING(n)	(*((volatile uint32_t *)0xE000E200 + ((n) >> 5)) = (1 << ((n) & 31)))
#define NVIC_CLEAR_PENDING(n)	(*((volatile uint32_t *)0xE000E280 + ((n) >> 5)) = (1 << ((n) & 31)))
#define NVIC_IS_ENABLED(n)	(*((volatile uint32_t *)0xE000E100 + ((n) >> 5)) & (1 << ((n) & 31)))
#define NVIC_IS_PENDING(n)	(*((volatile uint32_t *)0xE000E200 + ((n) >> 5)) & (1 << ((n) & 31)))
#define NVIC_IS_ACTIVE(n)	(*((volatile uint32_t *)0xE000E300 + ((n) >> 5)) & (1 << ((n) & 31)))


/*
 * TODO VARIABILISE THE PRIORITY DEPENDING ON THE ARCH;
 */

#define NVIC_SET_PRIORITY(irqnum, priority)  (*((volatile uint8_t *)0xE000E400 + (irqnum)) = (uint8_t)(priority))
#define NVIC_GET_PRIORITY(irqnum) (*((uint8_t *)0xE000E400 + (irqnum)))


/*
 * This library contains startup code and interrupt management function for ARM chips built with the
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

enum nvic_system_channel {

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
 * Priority levels : these are the standard interrupt priorities for drivers;
 *
 * Cortex-M4: 0,16,32,48,64,80,96,112,128,144,160,176,192,208,224,240
 */


//The standard priority for status interrupt;
#define DRIVER_STARUS_INTERRUPT_PRIORITY 32

//The standard priority for error interrupt;
#define DRIVER_ERROR_INTERRUPT_PRIORITY 16

//--------------------------------------------- Global interrupts settings ---------------------------------------------


/**
 * nvic_enable_interrupts : enables the interrupt control;
 */

static inline void nvic_enable_interrupts() {
	__asm__ volatile("cpsie i":::"memory");
}


/**
 * nvic_disable_interrupts : enables the interrupt control;
 */

static inline void nvic_disable_interrupts() {
	__asm__ volatile("cpsid i":::"memory");
}


//-------------------------------------------------- Interrupts enable -------------------------------------------------

//Enable a system interrupt;
void nvic_enable_system_interrupt(enum nvic_system_channel channel);


/**
 * nvic_enable_interrupt : enables the required non-system interrupt channel;
 * @param channel : the channel to enable;
 */

static inline void nvic_enable_interrupt(uint8_t channel) {
	NVIC_ENABLE_IRQ(channel);
}


//-------------------------------------------------- Interrupts disable ------------------------------------------------

//Disable a system interrupt;
void nvic_disable_system_interrupt(enum nvic_system_channel channel);


/**
 * nvic_disable_interrupt : disables the required non-system interrupt channel;
 * @param channel : the channel to disable;
 */

static inline void nvic_disable_interrupt(uint8_t channel){
	NVIC_DISABLE_IRQ(channel);
}


//------------------------------------------------ Set interrupt pending -----------------------------------------------

//Set a system interrupt pending;
void nvic_set_system_interrupt_pending(enum nvic_system_channel channel);


/**
 * nvic_set_interrupt_pending : sets the required non-system interrupt in the pending state;
 * @param channel : the channel to set in the pending state;
 */

static inline void nvic_set_interrupt_pending(uint8_t channel) {
	NVIC_SET_PENDING(channel);
}


//----------------------------------------------- Clear interrupt pending ----------------------------------------------

//Clear a system interrupt's pending state;
void nvic_clear_system_interrupt_pending(enum nvic_system_channel channel);


/**
 * nvic_set_interrupt_pending : sets the required non-system interrupt in the not-pending state;
 * @param channel : the channel to set in the not-pending state;
 */

static inline void nvic_clear_interrupt_pending(uint8_t channel) {
	NVIC_CLEAR_PENDING(channel);
}




//------------------------------------------ System interrupts priority update -----------------------------------------

//Update the nmi handler;
void nvic_set_system_interrupt_priority(enum nvic_system_channel channel, uint8_t priority);


/**
 * nvic_set_interrupt_priority : applies the provided priority to the required non-system interupt channel;
 *
 * @param channel : the channel to apply the priority;
 * @param priority : the priority to apply
 */

static inline void nvic_set_interrupt_priority(uint8_t channel, uint8_t priority) {
	NVIC_SET_PRIORITY(channel, priority);
}


//--------------------------------------------------- Handler update ---------------------------------------------------

//Update a system interrupt handler;
void nvic_set_system_interrupt_handler(enum nvic_system_channel channel, void (*handler)());

//Update a non-system interrupt handler;
void nvic_set_interrupt_handler(uint8_t channel, void (*handler)());


//---------------------------------------------------- Handler rest ----------------------------------------------------

/**
 * nvic_reset_system_interrupt_handler : resets the handler of the required system interrupt to 0, isr will return
 * 	immediately;
 *
 * @param channel : the interrupt channel to update;
 */

static inline void nvic_reset_system_interrupt_handler(enum nvic_system_channel channel) {
	nvic_set_system_interrupt_handler(channel, 0);
}


/**
 * nvic_reset_interrupt_handler : resets the handler of the required channel to 0, isr will return immediately;
 * @param channel : the interrupt channel to update;
 */

static inline void nvic_reset_interrupt_handler(uint8_t channel) {
	nvic_set_interrupt_handler(channel, 0);
}


#endif //TRACER_NVIC_H
