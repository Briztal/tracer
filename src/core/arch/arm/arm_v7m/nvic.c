//
// Created by root on 8/10/18.
//

#include "nvic.h"



//-------------------------------------------------- System interrupts -------------------------------------------------

/*
 * To handle system interrupts, following functions will be used;
 */


//All un-handled interrupts will trigger this empty function;
static void nop_function() {};

//-------------------------------------------- SysendSV exceptiontem Interrupts Management --------------------------------------------

//Enable a system interrupt;
void ic_enable_system_interrupt(enum ic_system_exception exception) {

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

			//Channel 14 : The PensSV Interrupt;
		case NVIC_PENDSV :
			//Always enabled;
			break;

			//Channel 15 : The Systick Interrupt;
		case NVIC_SYSTICK :
			//TODO USE SYSTICK LIBRARY;
			break;

	}

}


//Disable a system interrupt;
void ic_disable_system_interrupt(enum ic_system_exception exception) {

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

			//Channel 14 : The PensSV Interrupt;
		case NVIC_PENDSV :
			//Always enabled;
			break;

			//Channel 15 : The Systick Interrupt;
		case NVIC_SYSTICK :
			//TODO USE SYSTICK LIBRARY;
			break;

	}

}


//Set a system interrupt pending;
void ic_set_system_interrupt_pending(enum ic_system_exception exception) {

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

			//Channel 14 : The PensSV Interrupt;
		case NVIC_PENDSV :
			armv7m_set_pendsv_pending();
			break;

			//Channel 15 : The Systick Interrupt;
		case NVIC_SYSTICK :
			armv7m_set_systick_pending();
			break;

	}

}


//Clear a system interrupt's pending state;
void ic_clear_system_interrupt_pending(enum ic_system_exception exception) {

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

			//Channel 14 : The PensSV Interrupt;
		case NVIC_PENDSV :
			armv7m_clr_pendsv_pending();
			break;

			//Channel 15 : The Systick Interrupt;
		case NVIC_SYSTICK :
			armv7m_clr_systick_pending();
			break;

	}

}


//Is a system interrupt in the pending state;
bool ic_is_system_exception_pending(enum ic_system_exception exception) {

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

			//Channel 14 : The PensSV Interrupt;
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
void ic_set_system_interrupt_priority(enum ic_system_exception exception, uint8_t priority) {

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

/**
 * update_handler : update the handler of the absolute interrupt channel, system interrupts comprised;
 *
 * @param channel : the channel to update;
 * @param handler : the function to handle the interrupt. Can be null if interrupt can be disabled;
 */

static void update_handler(const uint8_t channel, void (*const handler)()) {

	//If the vector table is located in flash, no null pointer check required :
	if (!vector_table_in_ram) {

		//Update the correct non-system handler;
		irq_handlers[channel + 16] = handler;

	} else {
		//If the vector table is located in RAM, a null pointer check is required;

		//If the handler is null, save the empty handler; If not, save the handler;
		irq_handlers[channel + 16] = (handler) ? handler : nop_function;
	}

}


/**
 * ic_set_system_interrupt_handler : updates a system interrupt handler.
 * 	For safety, those channels are accessed indirectly, via the system channel enum;
 *
 * @param system_channel : the system channel to update;
 * @param handler : the function to handler the IRQ;
 */

void ic_set_system_interrupt_handler(const enum ic_system_exception system_channel, void (*const handler)()) {

	//Use the value of the enum to update the handler;
	update_handler(system_channel, handler);

}


/**
 * ic_set_interrupt_handler : updates the required non-system interrupt handler;
 *
 * @param non_system_channel : the channel to update. must be inferior to 240;
 *
 * @param handler : the handler. Can be null if interrupt must be disabled;
 */

void ic_set_interrupt_handler(const uint8_t non_system_channel, void (*const handler)()) {

	//If the channel is invalid :
	if (non_system_channel >= 240) {

		//Do nothing, errors can't be triggered from here; TODO USAGE FAULT;
		return;

	}

	//Update the handler, for the interrupt channel;
	update_handler(non_system_channel + (uint8_t) 16, handler);

}

