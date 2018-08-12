//
// Created by root on 8/10/18.
//

#include "nvic.h"

#include <stdbool.h>


//-------------------------------------------------- System interrupts -------------------------------------------------

/*
 * To handle system interrupts, following functions will be used;
 */


//All un-handled interrupts will trigger this empty function;
static void nop_function() {};

//-------------------------------------------- System Interrupts Management --------------------------------------------

//Enable a system interrupt;
void nvic_enable_system_interrupt(enum nvic_system_channel channel);


//Disable a system interrupt;
void nvic_disable_system_interrupt(enum nvic_system_channel channel);


//Set a system interrupt pending;
void nvic_set_system_interrupt_pending(enum nvic_system_channel channel);


//Clear a system interrupt's pending state;
void nvic_clear_system_interrupt_pending(enum nvic_system_channel channel);


//Update the nmi handler;
void nvic_set_system_interrupt_priority(enum nvic_system_channel channel, uint8_t priority);




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

static void (*irq_handlers[256])(void) = {0};


/*
 * When the vector table is located in flash memory (at startup at least), it cannot be altered.
 *
 * A supplementary mechanism is required in order to update the handler for a particular interrupt channel.
 */

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
 * nvic_set_system_interrupt_handler : updates a system interrupt handler.
 * 	For safety, those channels are accessed indirectly, via the system channel enum;
 *
 * @param system_channel : the system channel to update;
 * @param handler : the function to handler the IRQ;
 */

void nvic_set_system_interrupt_handler(const enum nvic_system_channel system_channel, void (*const handler)()) {

	//Use the value of the enum to update the handler;
	update_handler(system_channel, handler);

}


/**
 * nvic_set_interrupt_handler : updates the required non-system interrupt handler;
 *
 * @param non_system_channel : the channel to update. must be inferior to 240;
 *
 * @param handler : the handler. Can be null if interrupt must be disabled;
 */

void nvic_set_interrupt_handler(const uint8_t non_system_channel, void (*const handler)()) {

	//If the channel is invalid :
	if (non_system_channel >= 240) {

		//Do nothing, errors can't be triggered from here; TODO USAGE FAULT;
		return;

	}

	//Update the handler, for the interrupt channel;
	update_handler(non_system_channel + (uint8_t) 16, handler);

}

//---------------------------------------------------- Startup data ----------------------------------------------------

/*
 * The following section regroups data that is required to start the processor properly. Namely :
 * 	- The initial stack pointer, provided by the linker;
 * 	- The first function to execute, defined in another piece of code;
 */

//The initial stack pointer;
extern uint32_t *initial_stack_pointer;

//The code's entry point;
extern void __entry_point();



/*
 * Generate an ISR for each interrupt channel; Done using XMacro;
 */

//The handler link : a function that calls the handler link with a specific value;
#define channel(i) static void isr_##i() {isr_generic_flash_handler(i);}

//Define all isrs;
#include "channel_list.h"

#undef channel


/*
 * In order to start the processor properly, we define here the vector table, that is hard-copied in the firmware
 * 	as it, at the link section .vector. This section can be found in the link script, and is located at address 0;
 */

__attribute__ ((section(".vectors"), used))
static void (*const flash_vector_table[256])(void) = {

	//0 : Initial SP Value;
	(void (*)(void)) ((unsigned long) &initial_stack_pointer),

	//1 : Reset : call the program's entry point;
	&__entry_point,


	//2->255 : empty handler (240 times, 240 = 3 * 8 * 10);

	//In order to avoid writing 254 times the function name, we will use macros that will write it for us;
#define channel(i) &isr_##i,

//Redirect all isrs to the empty one;
#include "channel_list.h"

#undef channel

	//Adding another "&empty_isr" will cause a compiler warning "excess array initializer. Try it, it is funny !

};

