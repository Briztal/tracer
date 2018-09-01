/*
  arm_cortex_m4f.c Part of TRACER

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


#include <stdint.h>

#include <core/mem/stack.h>
#include <core/core.h>
#include <core/arch/arm/arm_v7m/arm_v7m.h>
#include <core/arch/arm/arm_v7m/nvic.h>

static volatile uint32_t kill_me;

uint32_t get_msp() {

	//Save the current main stack pointer in psp's cache;
	__asm__ __volatile__ ("mrs %0, msp" : "=r" (kill_me):);

	return kill_me;

}

uint32_t get_psp() {

	//Save the current main stack pointer in psp's cache;
	__asm__ __volatile__ ("mrs %0, psp" : "=r" (kill_me):);

	return kill_me;

}

//---------------------------------------------------- Threads setup ---------------------------------------------------

/**
 * core_enter_thread_mode : this function initialises threads in a safe state. It never returns.
 * 	The preemption environment must have been initialised before;
 *
 * 	- initialises all threads in their lowest privilege state;
 * 	- updates exception stacks;
 * 	- triggers the preemption;
 * 	- make all threads run an infinite loop;
 *
 * @param exception_stacks : stacks that must be used in case of interrupt;
 * @param preemption_call : the function to execute in the executing thread when all other threads are idle;
 */

void core_enter_thread_mode(struct core_stack **exception_stacks, void (*preemption_call)()) {

	//Disable all interrupts;
	ic_disable_interrupts();

	//A static var will insure that the function is called at most once;
	static bool entered = false;

	//If the flag has already been set, the function is called for the second time.
	if (entered) {

		//Error, must not happen;
		core_error("cortex_m4f : core_enter_thread_mode : Function called a second time;");

	}

	//Mark the function called;
	entered = true;

	//If we are in handler mode, ignore the request;
	if (ic_in_handler_mode()) {//TODO

		//Error, this function can't be executed in handler mode;
		core_error("cortex_m4f : core_enter_thread_mode : executed while in handler mode;");

		//Never reached;
		return;

	}

	static volatile void *volatile msp;
	static volatile void *volatile psp;
	static volatile void (*volatile prempt)();

	//Save the preemption_call;
	prempt = preemption_call;

	//Save the exception stack value in msp;
	msp = exception_stacks[0]->sp;

	//Save the current main stack pointer in psp's cache;
	__asm__ __volatile__ ("mrs %0, msp" : "=r" (psp):);

	//Execute an ISB;
	__asm__ __volatile__ ("DMB");

	//Update the main stack pointer so that exceptions occur in the exception stack;
	__asm__ __volatile__ (\
			"msr msp, %0\n\t"::"r" (msp):"memory"
	);

	//Execute an ISB;
	__asm__ __volatile__ ("ISB");


	//Update the process stack pointer;
	__asm__ __volatile__ (\
			"msr psp, %0\n\t"::"r" (psp):"memory"
	);

	//Execute an ISB;
	__asm__ __volatile__ ("ISB");


	//Update the control register to use PSP;//TODO UNPRIVILLEGE
	__asm__ __volatile__(\
        "mov r4, #2 \n\t"\
        "msr control, r4\n\t":::"r4", "cc", "memory"
	);

	//Execute an ISB;
	__asm__ __volatile__ ("ISB");

	//Execute the preemption call;
	(*prempt)();

	//Enable interrupts;
	ic_enable_interrupts();

	//Wait eternally for the preemption to be triggered;
	while(1);

}

//-------------------------------------------------- Stack management  -------------------------------------------------

/*
 * Notice :
 *
 *
 *  arm Cortex M4 Stack Frame :
 *
 *      ----------- <- Pre IRQ
 *      XPSR
 *      PC
 *      LR
 *      R12
 *      R3
 *      R2
 *      R1
 *      R0--------- <- Post IRQ
 */

/*
 * core_init_stack : this function initialises the unstacking environment, so that the given function will
 *  be executed at context switch time;
 *
 *  It starts by caching the process_t stack pointer, and stacks the process functions pointers, and the PSR.
 *
 *  Then, it saves the process index in R12 (next word);
 *
 *  Finally, it leaves space for empty stack frame and saves the stack pointer;
 */

void core_init_stack(struct core_stack *stack, void (*function)(), void (*exit_loop)(), void *arg) {

	//Reset the stack;
	uint32_t *sp4 = stack->sp_reset;

	//Store the mode //TODO DOC
	*(sp4 - 1) = 0x01000000;

	//Store the function in PC cache
	*(sp4 - 2) = (uint32_t) (function);

	//Store the return function in LR;
	*(sp4 - 3) = (uint32_t) (exit_loop);

	//Store the arg in R12 cache;
	*(sp4 - 4) = (uint32_t) (arg);

	//Update the stack pointer:
	sp4 -= 8;

	//TODO NOP, THE FPU MUST BE TAKEN INTO ACCOUNT, -16 (verify);

	//Return the stack pointer;
	stack->sp = sp4;

}


/*
 * core_get_init_arg : this function will return the value of r12. If called by the process init function,
 *  it will be equal to the previous function's arg;
 */

void *core_get_init_arg() {
	uint32_t arg = 0;
	__asm__ __volatile__("mov %0, r12": "=r" (arg):);
	return (void *)arg;
}

//----------------------------------------------------- Preemption -----------------------------------------------------



//The process stack provider; Declared in the core lib;
extern void *(*core_stack_provider)(uint8_t, void *);


/*
 * The context switcher :
 *
 *  - saves the current context;
 *  - calls the scheduler;
 *  - loads the new task's context;
 */

//TODO MAKE ANOTHER ONE FOR NO FPU SAVE;

void core_context_switcher() {


	//TODO ONLY IN HANDLER MODE;

	//A static variable will contain the temporary psp that we transmit to the stack provider;
	static void *volatile psp;

	/*
	 * This function happens in an interrupt basis. Will not happen during a critical section.
	 * Ints can be safely disabled;
	 */

	//Disable all interrupts during context switching;
	__asm__ __volatile__("cpsid i");


	//As R0 is already saved in memory, we can use it;
	__asm__ __volatile__ (\
	        "mrs r0, psp \n\t"\
    	    "stmdb r0!, {r4 - r11}\n\t"\
        	/*"vstmdb r0!, {s16 - s31}"\*/
	 );


	//Execute an ISB;
	__asm__ __volatile__ ("ISB");


	//Save the current process_t's stack pointer, while the process_t hasn't been terminated;
	__asm__ __volatile__ ("mrs %0, psp" : "=r" (psp):);

	//Execute an ISB;
	__asm__ __volatile__ ("ISB");

	//Enable interrupts;
	__asm__ __volatile__("cpsie i");

	//Provide the old stack and get a new one; There is only one thread, with the index 0;
	psp = (*core_stack_provider)(0, psp);

	core_log("PREEMPTION");

	//Disable all interrupts during context switching;
	__asm__ __volatile__("cpsid i");

	//Update the PSP;
	__asm__ __volatile__ ("msr psp, %0"::"r" (psp));

	//Execute an ISB;
	__asm__ __volatile__ ("ISB");

	//Unstack the context from the new stack; R0 will be updated at interrupt prempt, we can use it;
	__asm__ __volatile__ (\
       		"mrs r0, psp \n\t"\
        	"ldmdb r0!, {r4 - r11} \n\t"\
    	    /*"vldmdb r0!, {s16 - s31} "\*/
    );

	//Execute an ISB;
	__asm__ __volatile__ ("ISB");

	//Enable interrupts;
	__asm__ __volatile__("cpsie i");

}
