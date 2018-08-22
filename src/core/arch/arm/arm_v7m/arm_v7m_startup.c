//
// Created by root on 8/20/18.
//


#include <stdint.h>


/*
 * The following section regroups data that is required to start the processor properly. Namely :
 * 	- The initial stack pointer, provided by the linker;
 * 	- The first function to execute, defined in another piece of code;
 * 	- The array of IRQ handlers, provided by the nvic;
 */

extern void __entry_point();
extern uint32_t *_ram_highest;
extern void (*irq_handlers[])(void);



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
	(void (*)(void))((unsigned long)&_ram_highest - 10),

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

