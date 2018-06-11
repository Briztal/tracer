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



#include "arm_cortex_m4f.h"


#include <kernel/scheduler/scheduler.h>
#include <kernel/systick.h>
#include <kernel/kernel.h>
#include <kernel/drivers/PORT.h>

//--------------------------------------------- Private headers ---------------------------------------------

//The default stack provider. Simply return 1ms for process time and does not change the stack pointer;
void default_stack_provider(core_process_t *old_process) {}

//Initialise the systick timer;
void core_systick_init();


//Initialise the preemption;
void core_preemption_init();

//The preemption function;
void core_preemption();


//--------------------------------------------- Fields ---------------------------------------------

//The process's stack;
static core_process_t *core_process = 0;

//The process stack provider; Initialised to the default provider;
static void (*process_stack_provider)(core_process_t *) = &default_stack_provider;


//------------------------------------- Core init -------------------------------------

void core_init() {

    //Disable all interrupts;
    core_disable_interrupts();

    //Reset the stack provider;
    process_stack_provider = default_stack_provider;

    //Initialise and start the systick timer;
    core_systick_init();

    //Initialise the preemption interrupt;
    core_preemption_init();

    //Enable all interrupts;
    core_enable_interrupts();

    //Call kernel initialisation;
    kernel_init();

}


/*
 * ------------------------------------- Systick -------------------------------------
 */

/*
 * core_start_systick_timer :
 *  - sets the system timer reload value;
 *  - sets the function to execute at undf;
 *  - resets the undf flag;
 *  - enables the timer;
 *
 *  The timer must overflow every 1ms, and call the systick_tick function;
 *
 *  It must reset the process activity_time to 0ms to disable preemption;
 *
 *  the reload value is determined by :
 *
 *      timer_period_seconds = 1 / F_CPU seconds;
 *      systick_period_seconds = systick_period_ms / 1000;
 *
 *      nb_ticks = systick_period / timer_period = F_CPU * systick_period_ms / 1000
 *
 *      ex : FCPU = 120 E6 , for 1 ms systick period, nb_ticks = 120 E6 / 1000 = 120 E3
 */

void core_systick_init() {

    //Reset the systick process activity_time to disable preemption;
    systick_set_process_duration(0);

    //Set the systick function;
    _VectorsRam[15] = systick_tick;

    //Set the last byte of SHPR3 (systick prio) to 0, highest priority.
    *((uint8_t *)&SCB_SHPR3 + 3) = 0;

    //Set the systick priority to the highest priority level;
    //NVIC_SET_PRIORITY(-1, 0);

    //Update the reload value register;
    SYST_RVR = (uint32_t) ((float) F_CPU / (float) 2000);

    //Clear the systick flag;
    SCB_ICSR &= ~SCB_ICSR_PENDSTSET;

    //Enable the systick interrupt;
    SYST_CSR |= SYST_CSR_ENABLE;

}



//------------------------------- Preemtion -------------------------------

/*
 * core_preemption_init : sets function and priority for the pendSV interrupt;
 */

void core_preemption_init() {

    //Clear the pendSV pending status;
    SCB_ICSR &= ~SCB_ICSR_PENDSVSET;

    //Set the pendSV function;
    _VectorsRam[14] = core_preemption;

    //Set the third byte of SHPR3 (pendSV prio) to 240, lowest priority.
    *((uint8_t *)&SCB_SHPR3 + 2) = 240;

   // NVIC_SET_PRIORITY(-2, 224);//TODO 240

}


/*
 * The context switcher :
 *
 *  - saves the current context;
 *  - calls the scheduler;
 *  - loads the new task's context;
 */

void core_preemption() {


    //As R0 is already saved in memory, we can use it;
    __asm__ __volatile__ (\
	        "mrs r0, psp \n\t"\
    	    "stmdb r0!, {r4 - r11}\n\t"\
        	"vstmdb r0!, {s16 - s31}"\
	 );

    //Execute an ISB;
    __asm__ __volatile__ ("ISB");


    //Save the current process_t's stack pointer, while the process_t hasn't been deleted;
    __asm__ __volatile__ ("mrs %0, psp" : "=r" (core_process->process_stack->stack_pointer):);

    //Execute an ISB;
    __asm__ __volatile__ ("ISB");


    //Provide the old stack and get a new one;
    process_stack_provider(core_process);

    //Update the process duration;
    systick_set_process_duration(core_process->activity_time);

    __asm__ __volatile__ (\
            "msr psp, %0"::"r" (core_process->process_stack->stack_pointer)
    );

    //Execute an ISB;
    __asm__ __volatile__ ("ISB");

    //Unstack the context from the new stack; R0 will be updated at interrupt exit, we can use it;
    __asm__ __volatile__ (\
       		"mrs r0, psp \n\t"\
        	"ldmdb r0!, {r4 - r11} \n\t"\
    	    "vldmdb r0!, {s16 - s31} "\
    );

    //Execute an ISB;
    __asm__ __volatile__ ("ISB");


}


//------------------------------------- Stack management  -------------------------------------

/*
 * Notice :
 *
 *
 *  ARM Cortex M4 Stack Frame :
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

void core_init_stack(core_stack_t *stack, void (*function)(), void (*exit_loop)(), void *arg) {

    //Reset the stack;
    stack->stack_pointer = stack->stack_begin;

	//Cast the pointer to a manipulable pointer;
	uint32_t *sp4 = (uint32_t *)stack->stack_pointer;

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

	//Return the stack pointer;
	stack->stack_pointer = sp4;

}


/*
 * core_get_init_arg : this function will return the value of r12. If called by the process init function,
 *  it will be equal to the previous function's arg;
 */

void *core_get_init_arg() {
    uint32_t seed = 0;
    __asm__ __volatile__("mov %0, r12": "=r" (seed):);
    return (void *)seed;
}


//------------------------------------- Execution -------------------------------------

/*
 * core_set_process_provider : updates the stack provider;
 */

void core_set_process_provider(void (*new_provider)(core_process_t *)) {

	//Update the new provider;
	process_stack_provider = new_provider;

}


/*
 * core_reset_process_provider : resets the stack provider;
 */

void core_reset_process_provider() {

	//Update the new provider;
	process_stack_provider = &default_stack_provider;

}


/*
 * core_execute_process : executes the provided process, discarding what was executed before;
 * 	Must have been initialised by core_init_stack;
 */

void core_execute_process(core_process_t *process, void (*function)(void *), void *arg) {

	//If we are in handler mode, ignore the request;
	if (core_in_handler_mode()) {
		return;
	}

	//As this function contains a context switch, we will copy our args in static variables;
	static void (*volatile sf)(void *volatile);
	static void *volatile sa;

	//Cache the function and the arg in static vars;
	sf = function;
	sa = arg;

    //Cache the stack_t pointer;
    core_stack_t *stack = process->process_stack;

	//Reset the stack;
	stack->stack_pointer = stack->stack_begin;

    //Set the process duration;
    systick_set_process_duration(process->activity_time);

    //Save the current stack;
    core_process = process;

    //Update the process stack pointer;
	 __asm__ __volatile__ (\
			"msr psp, %0\n\t"::"r" (stack->stack_pointer):"memory"
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

	//Execute the cached function with the cached arg;
	(*sf)(sa);

}
