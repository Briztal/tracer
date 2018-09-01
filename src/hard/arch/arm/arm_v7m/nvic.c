//
// Created by root on 8/10/18.
//

#include "nvic.h"


/*
 * The ARMV7 NVIC support 8 bit priorities. The number of bits evaluated depends on the implementation, but there
 * 	are at least 3 bits evaluated, which allows the library to implement all core priority levels;
 */

//The lowest priority level;
const uint8_t ic_priority_0 = 0xE0;
const uint8_t ic_priority_1 = 0xC0;
const uint8_t ic_priority_2 = 0xA0;
const uint8_t ic_priority_3 = 0x80;
const uint8_t ic_priority_4 = 0x60;
const uint8_t ic_priority_5 = 0x40;
const uint8_t ic_priority_6 = 0x20;
const uint8_t ic_priority_7 = 0x00;


//TODO NUMBER OF INTERRUPTS INITIALISED BY THE ARM LIB;
//TODO NUMBER OF INTERRUPTS INITIALISED BY THE ARM LIB;
//TODO NUMBER OF INTERRUPTS INITIALISED BY THE ARM LIB;
//TODO NUMBER OF INTERRUPTS INITIALISED BY THE ARM LIB;
//TODO NUMBER OF INTERRUPTS INITIALISED BY THE ARM LIB;
//TODO NUMBER OF INTERRUPTS INITIALISED BY THE ARM LIB;
//TODO NUMBER OF INTERRUPTS INITIALISED BY THE ARM LIB;
//TODO NUMBER OF INTERRUPTS INITIALISED BY THE ARM LIB;
//TODO NUMBER OF INTERRUPTS INITIALISED BY THE ARM LIB;
//TODO NUMBER OF INTERRUPTS INITIALISED BY THE ARM LIB;
//TODO NUMBER OF INTERRUPTS INITIALISED BY THE ARM LIB;
//TODO NUMBER OF INTERRUPTS INITIALISED BY THE ARM LIB;
//TODO NUMBER OF INTERRUPTS INITIALISED BY THE ARM LIB;
//TODO NUMBER OF INTERRUPTS INITIALISED BY THE ARM LIB;
//TODO NUMBER OF INTERRUPTS INITIALISED BY THE ARM LIB;
//TODO NUMBER OF INTERRUPTS INITIALISED BY THE ARM LIB;
//TODO NUMBER OF INTERRUPTS INITIALISED BY THE ARM LIB;
//TODO NUMBER OF INTERRUPTS INITIALISED BY THE ARM LIB;


//-------------------------------------------- Interrupt handling ---------------------------------------------

/**
 * vector_table_in_ram : determines whether the NVIC vector table is in flash or in RAM; At init, it is located in
 * 	flash;
 */

static bool vector_table_in_ram = false;


/**
 * in_flash_handlers : the array of isr handlers when the vector table is in flash. Obsolete when the vector table
 * 	is in RAM;
 *
 * 	When the vector table is in flash, null function pointers are authorised, as a dedicated isr_handler will cache
 * 	the function pointer and execute it if it is not null;
 * 	At startup, the irq handlers contains only null pointers;
 *
 * 	When the vector table is in RAM, handlers are called automatically by the NVIC module. Null pointers will be
 * 	dereferenced, and are as a consequence forbidden; They should be replaced by the nop_function defined behind;
 *
 * 	When relocating the vector table in RAM, the handlers array will be filled with nop_function references;
 */

void (*irq_handlers[256])(void) = {0};


/*
 * When the vector table is located in flash memory (at startup at least), it cannot be altered.
 *
 * A supplementary mechanism is required in order to update the handler for a particular interrupt channel.
 */



//------------------------------------------------- Handlers Management ------------------------------------------------


//All un-handled interrupts will trigger this empty function;
static void nop_function() {};


/**
 * update_handler : update the handler of the absolute interrupt channel, system interrupts comprised;
 *
 * @param channel : the channel to update;
 * @param handler : the function to handle the interrupt. Can be null if interrupt can be disabled;
 */

static void update_handler(const uint16_t channel, void (*const handler)()) {

	//If the vector table is located in flash, no null pointer check required :
	if (!vector_table_in_ram) {

		//Update the correct handler;
		irq_handlers[channel] = handler;

	} else {
		//If the vector table is located in RAM, a null pointer check is required;

		//If the handler is null, save the empty handler; If not, save the handler;
		irq_handlers[channel] = (handler) ? handler : nop_function;
	}

}



//----------------------------------------------- IC standard interrupts -----------------------------------------------


/**
 * ic_enable_interrupts : enables the interrupt control;
 */

void ic_enable_interrupts() {
	__asm__ volatile("cpsie i":::"memory");
}


/**
 * ic_disable_interrupts : enables the interrupt control;
 */

void ic_disable_interrupts() {
	__asm__ volatile("cpsid i":::"memory");
}



/**
 * ic_enable_interrupt : enables the required non-system interrupt channel;
 * @param channel : the channel to enable;
 */

void ic_enable_interrupt(uint16_t channel) {
	armv7m_nvic_enable_interrupt(channel);
}

/**
 * ic_disable_interrupt : disables the required non-system interrupt channel;
 * @param channel : the channel to disable;
 */

void ic_disable_interrupt(uint16_t channel){
	armv7m_nvic_disable_interrupt(channel);
}


/**
 * ic_set_interrupt_pending : sets the required non-system interrupt in the pending state;
 * @param channel : the channel to set in the pending state;
 */

void ic_set_interrupt_pending(uint16_t channel) {
	armv7m_nvic_set_interrupt_pending(channel);
}



/**
 * ic_set_interrupt_pending : sets the required non-system interrupt in the not-pending state;
 * @param channel : the channel to set in the not-pending state;
 */

void ic_clear_interrupt_pending(uint16_t channel) {
	armv7m_nvic_clear_interrupt_pending(channel);
}

/**
 * ic_is_interrupt_pending : sets the required non-system interrupt in the not-pending state;
 * @param channel : the channel to set in the not-pending state;
 */

void ic_is_interrupt_pending(uint16_t channel) {
	armv7m_nvic_clear_interrupt_pending(channel);
}


/**
 * ic_set_interrupt_priority : applies the provided priority to the required non-system interupt channel;
 *
 * @param channel : the channel to apply the priority;
 * @param priority : the priority to apply
 */

void ic_set_interrupt_priority(uint16_t channel, uint8_t priority) {
	armv7m_nvic_set_priority(channel, priority);
}

/**
 * ic_get_interrupt_priority : applies the provided priority to the required non-system interupt channel;
 *
 * @param channel : the channel to apply the priority;
 * @param priority : the priority to apply
 */

uint8_t ic_get_interrupt_priority(uint16_t channel, uint8_t priority) {
	return armv7m_nvic_get_priority(channel);
}


/**
 * ic_set_interrupt_handler : updates the required non-system interrupt handler;
 *
 * @param non_system_channel : the channel to update. must be inferior to 240;
 *
 * @param handler : the handler. Can be null if interrupt must be disabled;
 */

void ic_set_interrupt_handler(const uint16_t non_system_channel, void (*const handler)()) {

	//If the channel is invalid :
	if (non_system_channel >= 240) {

		//Do nothing, errors can't be triggered from here; TODO USAGE FAULT;
		return;

	}

	//Update the handler, for the interrupt channel;
	update_handler(non_system_channel + (uint8_t) 16, handler);

}


/**
 * ic_reset_interrupt_handler : resets the handler of the required channel to 0, isr will return immediately;
 * @param channel : the interrupt channel to update;
 */

void ic_reset_interrupt_handler(uint16_t channel) {
	ic_set_interrupt_handler(channel, 0);
}


/**
 * ic_in_handler_mode :
 *
 * @return true is a handler is curently in execution;
 */
bool ic_in_handler_mode() {
	return (bool) armv7m_get_exception_number();
}


//-------------------------------------------- SysendSV exceptiontem Interrupts Management --------------------------------------------

//Enable an exception;
void nvic_enable_exception(enum nvic_exception exception) {

	//Evaluate the exception type :
	switch (exception) {

		//Channel 2 : The Non Maskable Interrupt;
		case NVIC_NMI :
			//The NMI is always enabled;
			break;

			//Channel 3 : The Hardware Fault Interrupt;
		case NVIC_HARD_FAULT:
			//The HardFault exception is always enabled;
			break;

			//Channel 4 : The Memory Fault Interrupt;
		case NVIC_MEM_FAULT :
			armv7m_enable_mem_fault();
			break;

			//Channel 5 : The Bus Fault Interrupt;
		case NVIC_BUS_FAULT :
			armv7m_enable_bus_fault();
			break;

			//Channel 6 : The Usage Fault Interrupt;
		case NVIC_USAGE_FAULT :
			armv7m_enable_usage_fault();
			break;

			//Channel 11 : The Supervisor Call Interrupt;
		case NVIC_SVC :
			//Always enabled;
			break;

			//Channel 14 : The PendSV Interrupt;
		case NVIC_PENDSV :
			//Always enabled;
			break;

			//Channel 15 : The Systick Interrupt;
		case NVIC_SYSTICK :
			//TODO USE SYSTICK LIBRARY;
			break;

	}

}


//Disable an exception;
void nvic_disable_exception(enum nvic_exception exception) {

	//Evaluate the exception type :
	switch (exception) {

		//Channel 2 : The Non Maskable Interrupt;
		case NVIC_NMI :
			//The NMI can't be disabled;
			break;

			//Channel 3 : The Hardware Fault Interrupt;
		case NVIC_HARD_FAULT:
			//The Hard Fault exception can't be disabled;
			break;

			//Channel 4 : The Memory Fault Interrupt;
		case NVIC_MEM_FAULT :
			armv7m_disable_mem_fault();
			break;

			//Channel 5 : The Bus Fault Interrupt;
		case NVIC_BUS_FAULT :
			armv7m_disable_bus_fault();
			break;

			//Channel 6 : The Usage Fault Interrupt;
		case NVIC_USAGE_FAULT :
			armv7m_disable_usage_fault();
			break;

			//Channel 11 : The Supervisor Call Interrupt;
		case NVIC_SVC :
			//Always enabled;
			break;

			//Channel 14 : The PendSV Interrupt;
		case NVIC_PENDSV :
			//Always enabled;
			break;

			//Channel 15 : The Systick Interrupt;
		case NVIC_SYSTICK :
			//TODO USE SYSTICK LIBRARY;
			break;

	}

}


//Set an exception pending;
void nvic_set_exception_pending(enum nvic_exception exception) {

	//Evaluate the exception type :
	switch (exception) {

		//Channel 2 : The Non Maskable Interrupt;
		case NVIC_NMI :
			armv7m_trigger_nmi();
			break;

			//Channel 3 : The Hardware Fault Interrupt;
		case NVIC_HARD_FAULT:
			//Can't be triggered;
			break;

			//Channel 4 : The Memory Fault Interrupt;
		case NVIC_MEM_FAULT :
			//Can't be triggered;
			break;

			//Channel 5 : The Bus Fault Interrupt;
		case NVIC_BUS_FAULT :
			//Can't be triggered;
			break;

			//Channel 6 : The Usage Fault Interrupt;
		case NVIC_USAGE_FAULT :
			//Can't be triggered;
			break;

			//Channel 11 : The Supervisor Call Interrupt;
		case NVIC_SVC :
			//i = 5;//Will be removed, the compiler cringes when there is no statement...
			//asm __volatile__ ("svc");
			break;

			//Channel 14 : The PendSV Interrupt;
		case NVIC_PENDSV :
			armv7m_set_pendsv_pending();
			break;

			//Channel 15 : The Systick Interrupt;
		case NVIC_SYSTICK :
			armv7m_set_systick_pending();
			break;

	}

}


//Clear an exception's pending state;
void nvic_clear_exception_pending(enum nvic_exception exception) {

	//Evaluate the exception type :
	switch (exception) {

		//Channel 2 : The Non Maskable Interrupt;
		case NVIC_NMI :
			//Non maskable;
			break;

			//Channel 3 : The Hardware Fault Interrupt;
		case NVIC_HARD_FAULT:
			//Can't be cleared;
			break;

			//Channel 4 : The Memory Fault Interrupt;
		case NVIC_MEM_FAULT :
			//Can't be cleared;
			break;

			//Channel 5 : The Bus Fault Interrupt;
		case NVIC_BUS_FAULT :
			//Can't be cleared;
			break;

			//Channel 6 : The Usage Fault Interrupt;
		case NVIC_USAGE_FAULT :
			//Can't be cleared;
			break;

			//Channel 11 : The Supervisor Call Interrupt;
		case NVIC_SVC :
			//Can't be cleared;
			break;

			//Channel 14 : The PendSV Interrupt;
		case NVIC_PENDSV :
			armv7m_clr_pendsv_pending();
			break;

			//Channel 15 : The Systick Interrupt;
		case NVIC_SYSTICK :
			armv7m_clr_systick_pending();
			break;

	}

}


//Is an exception in the pending state;
bool nvic_is_exception_pending(enum nvic_exception exception) {

	//Evaluate the exception type :
	switch (exception) {

		//Channel 2 : The Non Maskable Interrupt;
		case NVIC_NMI :
			//Non maskable;
			return false;

			//Channel 3 : The Hardware Fault Interrupt;
		case NVIC_HARD_FAULT:
			//Non maskable;
			return false;

			//Channel 4 : The Memory Fault Interrupt;
		case NVIC_MEM_FAULT :
			return armv7m_is_mem_fault_pending();

			//Channel 5 : The Bus Fault Interrupt;
		case NVIC_BUS_FAULT :
			return armv7m_is_bus_fault_pending();

			//Channel 6 : The Usage Fault Interrupt;
		case NVIC_USAGE_FAULT :
			return armv7m_is_usage_fault_pending();

			//Channel 11 : The Supervisor Call Interrupt;
		case NVIC_SVC :
			return armv7m_is_svc_pending();

			//Channel 14 : The PendSV Interrupt;
		case NVIC_PENDSV :
			return armv7m_is_pendsv_pending();

			//Channel 15 : The Systick Interrupt;
		case NVIC_SYSTICK :
			return armv7m_is_systick_pending();

	}

	//Default, return false;
	return false;
}


//Update the nmi handler;
void nvic_set_exception_priority(enum nvic_exception exception, uint8_t priority) {

	//Evaluate the exception type :
	switch (exception) {

		//Channel 2 : The Non Maskable Interrupt;
		case NVIC_NMI :
			//Non maskable;
			break;

			//Channel 3 : The Hardware Fault Interrupt;
		case NVIC_HARD_FAULT:
			//Fixed Priority;
			break;

			//Channel 4 : The Memory Fault Interrupt;
		case NVIC_MEM_FAULT :
			armv7m_set_mem_fault_priority(priority);
			break;

			//Channel 5 : The Bus Fault Interrupt;
		case NVIC_BUS_FAULT :
			armv7m_set_bus_fault_priority(priority);
			break;

			//Channel 6 : The Usage Fault Interrupt;
		case NVIC_USAGE_FAULT :
			armv7m_set_usage_fault_priority(priority);
			break;

			//Channel 11 : The Supervisor Call Interrupt;
		case NVIC_SVC :
			armv7m_set_svcall_priority(priority);
			break;

			//Channel 14 : The PendSV Interrupt;
		case NVIC_PENDSV :
			armv7m_set_pendsv_priority(priority);
			break;

			//Channel 15 : The Systick Interrupt;
		case NVIC_SYSTICK :
			armv7m_set_systick_priority(priority);
			break;

	}

}

/**
 * nvic_set_exception_handler : updates an exception handler.
 * 	For safety, those channels are accessed indirectly, via the system channel enum;
 *
 * @param system_channel : the system channel to update;
 * @param handler : the function to handler the IRQ;
 */

void nvic_set_exception_handler(const enum nvic_exception system_channel, void (*const handler)()) {

	//Use the value of the enum to update the handler;
	update_handler(system_channel, handler);

}


/**
 * nvic_reset_exception_handler : resets the handler of the required system interrupt to 0, isr will return
 * 	immediately;
 *
 * @param channel : the interrupt channel to update;
 */

void nvic_reset_exception_handler(enum nvic_exception channel) {
	nvic_set_exception_handler(channel, 0);
}



//----------------------------------------------------- Preemption -----------------------------------------------------

//Set the handler of the preemption exception;
extern void core_preemption_set_handler(void (*handler)(void)) {
	nvic_set_exception_handler(NVIC_PENDSV, handler);
}

//Set the priority of the preemption exception;
extern void core_preemption_set_priority(uint8_t priority) {
	armv7m_set_pendsv_priority(priority);
}

//Enable the preemption exception;
extern void core_preemption_enable() {
	//PendSV is always enabled;
}

//Trigger the preemption;
extern void core_preemption_trigger() {
	armv7m_set_pendsv_pending();
}


//----------------------------------------------------- Syscall -----------------------------------------------------

//Set the handler of the syscall exception;
extern void core_syscall_set_handler(void (*handler)(void)) {
	nvic_set_exception_handler(NVIC_SVC, handler);
}

//Set the priority of the syscall exception;
extern void core_syscall_set_priority(uint8_t priority) {
	armv7m_set_svcall_priority(priority);
}

//Enable the syscall exception;
extern void core_syscall_enable() {
	//Always enabled;
}

//Trigger the syscall;
extern void core_syscall_trigger() {
	__asm__ __volatile ("");//TODO SVC ??? NOT WORKING...
}


//TODO SEPARATE AGAIN;
//TODO SEPARATE AGAIN;
//TODO SEPARATE AGAIN;
//TODO SEPARATE AGAIN;
//TODO SEPARATE AGAIN;
//TODO SEPARATE AGAIN;

//

//
// Created by root on 8/20/18.
//


#include <stdint.h>
#include <core/core.h>


/*
 * The following section regroups data that is required to start the processor properly. Namely :
 * 	- The initial stack pointer, provided by the linker;
 * 	- The first function to execute, defined in another piece of code;
 * 	- The array of IRQ handlers, provided by the nvic;
 */

extern void __entry_point();
extern uint32_t _ram_highest;
//extern void (*irq_handlers[])(void);



/**
 * isr_generic_handler : in order to support handler update, all functions of the in-flash vector table will
 * 	lead to this function, that selects the appropriate function to execute, and executes it if it exists.
 *
 * @param i : the interrupt channel. 0 to 240;
 */

static void isr_generic_flash_handler(uint8_t i) {


	//Cache the isr handler;
	void (*handler)(void) = irq_handlers[i];

	//If the handler is not null :
	if (handler) {

		//Execute the handler;
		(*handler)();

	}

}


/*
 * Generate an ISR for each interrupt channel; Done using XMacro;
 */
//TODO OPTIMISATION FOR FLASH RELOCATION : ONLY COPY THE TWO FIST BYTES AND DO NOT DEFINE THOSE FUNCTIONS.

//The handler link : a function that calls the handler link with a specific value;
#define channel(i) static void isr_##i() {isr_generic_flash_handler(i);}

//Define all isrs;
#include "nvic_channel_list.h"

#undef channel


/*
 * In order to start the processor properly, we define here the vector table, that is hard-copied in the firmware
 * 	as it, at the link section .vector. This section can be found in the link script, and is located at address 0;
 */

//TODO OPTIMISATION FOR FLASH RELOCATION : ONLY COPY THE TWO FIST BYTES;
void *vtable[256] __attribute__ ((section(".vectors"))) = {

	//0 : Initial SP Value; In ARM Architecture, the stack pointer decreases;
	(void (*)(void))((unsigned long)&_ram_highest),

	//1 : Reset : call the program's entry point;
	&__entry_point,


	//2->255 : empty handler (240 times, 240 = 3 * 8 * 10);

	//In order to avoid writing 254 times the function name, we will use macros that will write it for us;
#define channel(i) &isr_##i,

//Redirect all isrs to the empty one;
#include "nvic_channel_list.h"

#undef channel

	//Adding another "&empty_isr" will cause a compiler warning "excess array initializer. Try it, it is funny !

};

