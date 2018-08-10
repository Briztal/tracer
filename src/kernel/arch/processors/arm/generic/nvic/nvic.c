//
// Created by root on 8/10/18.
//

#include "nvic.h"


//-------------------------------------------------- System interrupts -------------------------------------------------

/*
 * To handle system interrupts, following functions will be used;
 */


//All un-handled interrupts will trigger this empty function;
void empty_isr() {};


/*
 * System Interrupt handlers declarations;
 */

void nmi_handler();
void hard_fault_isr();
void memmanage_fault_isr();
void bus_fault_isr();
void usage_fault_isr();
void svcall_isr();
void pendsv_isr();
void systick_isr();



//-------------------------------------------- System Interrupts Management --------------------------------------------

//Enable a system interrupt;
void nvic_enable_system_interrupt(enum nvic_system_interrupt channel);


//Disable a system interrupt;
void nvic_disable_system_interrupt(enum nvic_system_interrupt channel);


//Set a system interrupt pending;
void nvic_set_system_interrupt_pending(enum nvic_system_interrupt channel);


//Clear a system interrupt's pending state;
void nvic_clear_system_interrupt_pending(enum nvic_system_interrupt channel);


//Update the nmi handler;
void nvic_set_system_interrupt_priority(enum nvic_system_interrupt channel, uint8_t priority);



//------------------------------------------------- Handlers Management ------------------------------------------------

//Update a system interrupt handler;
void nvic_set_system_interrupt_handler(enum nvic_system_interrupt channel, void (*handler)());


//Update a non-system interrupt handler;
void nvic_set_interrupt_handler(uint8_t channel, void (*handler)());


//-------------------------------------------- In-flash Interrupt handling ---------------------------------------------

/*
 * When the vector table is located in flash memory (at startup at least), it cannot be altered.
 *
 * A supplementary mechanism is required in order to update the handler for a particular interrupt channel.
 */


/**
 * isr_handlers : an array of procedures.
 *
 * Index i contains the handler function for interrupt channel i;
 *
 * @return
 */

void (**isr_handlers)();


/**
 * isr_generic_handler : in order to support handler update, all functions of the in-flash vector table will
 * 	lead to this function, that selects the appropriate function to execute, and executes it if it exists.
 *
 * @param i : the interrupt channel. 0 to 240;
 */

void isr_generic_handler(uint8_t i) {

	//Cache the isr handler;
	void (*handler)(void) = isr_handlers[i];

	//If the handler is not null :
	if (handler) {

		//Execute the handler;
		(*handler)();

	}

}


/*
 * Generate an ISR for each interrupt channel; Done using XMacro;
 */

//The handler link : a function that calls the handler link with a specific value;
#define handler_link(i) static void isr_##i() {isr_generic_handler(i);}

//Bind the handler link to the channel generator;
#define channel(i) handler_link(i);

//Define all isrs;
#include "channel_list.h"

#undef channel
#undef handler_link


//---------------------------------------------- In-RAM Interrupt handling ---------------------------------------------



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
 * In order to start the processor properly, we define here the vector table, that is hard-copied in the firmware
 * 	as it, at the link section .vector. This section can be found in the link script, and is located at address 0;
 */

__attribute__ ((section(".vectors"), used))
void (*const flash_vector_table[256])(void) = {

	//0 : Initial SP Value;
	(void (*)(void)) ((unsigned long) &initial_stack_pointer),

	//1 : Reset : call the program's entry point;
	&__entry_point,

	//2 : NMI;
	&nmi_handler,

	//3 : Hard fault;
	&hard_fault_isr,

	//4 : Memory Management Fault;
	&memmanage_fault_isr,

	//5 : Bus Fault;
	&bus_fault_isr,

	//6 : Usage Fault;
	&usage_fault_isr,

	//7 : Reserved;
	&empty_isr,

	//8 : Reserved;
	&empty_isr,

	//9 : Reserved;
	&empty_isr,

	//10 : Reserved;
	&empty_isr,

	//11 : SVCall;
	svcall_isr,

	//12 : Reserved for Debug;
	&empty_isr,

	//13 : Reserved;
	&empty_isr,

	//14 : PendSV;
	pendsv_isr,

	//15 : Systick ISR;
	systick_isr,

	//16->255 : empty handler (240 times, 240 = 3 * 8 * 10);

	//In order to avoid writing 240 times the function name, we will use macros that will write it for us;
#define t3(i) i i i
#define t8(i) i i i i i i i i
#define t10(i) i i i i i i i i i i
#define cm(i) i,

	t3(t8(t10(cm(&empty_isr))))

	//Adding another "&empty_isr" will cause a compiler warning "excess array initializer. Try it, it is funny !

//Undef macros for safety;
#undef t3
#undef t8
#undef t10

};


void (*_VectorsRam[256])(void);
