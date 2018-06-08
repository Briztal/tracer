

#include "arm_cortex_m4f.h"


//--------------------------------------------- Private headers ---------------------------------------------

//The default stack provider. Simply returns the provided stack;
stack_t *default_stack_provider(stack_t *old_stack) { return old_stack;}


//Set the process's stack pointer;
inline void core_set_process_stack_pointer(const void *sp);

//Get the process's stack pointer;
inline void *core_get_process_stack_pointer();

//Stack the current process context;
inline void core_stack_process_context();

//Unstack the current process context;
inline void core_unstack_process_context();

//The preemption function;
void core_premtion();


//--------------------------------------------- Fields ---------------------------------------------

//The process's stack;
stack_t *process_stack;

//The process stack provider; Initialised to the default provider;
stack_t *(*process_stack_provider)(stack_t*) = &default_stack_provider;


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
 * core_set_process_stack_pointer : injects assembly inline to move the provided stack pointer into psp;
 */

inline void core_set_process_stack_pointer(const void *sp) {
    __asm__ __volatile__ (\
			"msr psp, %0"::"r" (sp)
	);
	
	//Execute an ISB;
	__asm__ __volatile__ ("ISB");
	
}


/*
 * core_get_process_stack_pointer : injects assembly to query psp and returns the result;
 */

inline void *core_get_process_stack_pointer() {
    void *ptr = 0;
    __asm__ __volatile__ ("mrs %0, psp" : "=r" ((uint32_t) ptr):);
    return ptr;
}


/*
 * core_stack_process_context : injects assembly code in order to stack memory that is not
 *  stacked automatically in the process stack;
 *
 *  It moves psp into r0, and then stacks r4-r7, general purposes registers, and finally
 *      stacks s16- s31, floating point unit memory;
 *
 *  You may notice that PSP remains unchanged after the stacking. This is done with purpose, in order to
 *      allow the processor to unstack properly even if core_unstack_context is called;
 */

inline void core_stack_process_context() {
    uint32_t scratch;
    __asm__ __volatile__ (\
	        "mrs %0, psp \n\t"\
    	    "stmdb %0!, {r4 - r11}\n\t"\
        	"vstmdb %0!, {s16 - s31}" : "=r" (scratch)\
	 );

	//Execute an ISB;
	__asm__ __volatile__ ("ISB");
	
}


/*
 * core_stack_process_context : injects assembly code to stack memory that is not
 *  stacked automatically in the process stack;
 *
 *  It moves psp into r0, and then unstacks r4-r7, general purposes memory, and finally
 *      unstacks s16- s31, floating point unit memory;
 *
 *  As core_stack_process_context didn't alter PSP, this function doesn't either;
 */

void core_unstack_process_context() {
    uint32_t scratch;
    __asm__ __volatile__ (\
       		"mrs %0, psp \n\t"\
        	"ldmdb %0!, {r4 - r11} \n\t"\
    	    "vldmdb %0!, {s16 - s31} " : "=r" (scratch)\
    );

	//Execute an ISB;
    __asm__ __volatile__ ("ISB");

}


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

void *core_init_stack(void *sp, void (*function)(), void (*exit_loop)(), void *arg) {
	
	//Cast the pointer to a manipulable pointer;	
	uint32_t *sp4 = (uint32_t *)sp;

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
	return (void *)sp4;

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
 * core_set_stack_provider : updates the stack provider;
 */

void core_set_stack_provider(stack_t *(*new_provider)(stack_t*)) {
	
	//Update the new provider;
	process_stack_provider = new_provider;

}


/*
 * core_reset_stack_provider : resets the stack provider;
 */

void core_reset_stack_provider() {
	
	//Update the new provider;
	process_stack_provider = &default_stack_provider;

}


/*
 * core_execute_process : executes the provided process, discarding what was executed before; 
 * 	Must have been initialised by core_init_stack;
 */

void core_execute_process(stack_t *stack, void (*function)(void *), void *arg) {
	
	//If we are in handler mode, ignore the request;
	if (core_in_handler_mode()) {
		return;
	}

	//As this function contains a context switch, we will copy our args in static variables;
	static void (*sf)(void *);
	static void *sa;
	
	//Cache the function and the arg in static vars;
	sf = function;
	sa = arg;

	//Update the process stack pointer;
	core_set_process_stack_pointer(stack);
		
	//Update the control register to use PSP;//TODO UNPRIVILLEGE
    __asm__ __volatile__(\
        "mov r4, #2 \n\t"\
        "msr control, r4\n\t"
    );

	//Execute an ISB;
    __asm__ __volatile__ ("ISB");
	
	//Execute the cached function with the cached arg;
	(*sf)(sa);

}


/*
 * The context switcher :
 *
 *  - saves the current context;
 *  - calls the scheduler;
 *  - loads the new task's context;
 */

void core_pereemption() {

    //Save the context in the current process stack;
    core_stack_process_context();
	
    //Save the current process_t's stack pointer, while the process_t hasn't been deleted;
    process_stack->stack_pointer = core_get_process_stack_pointer();
    
	//Provide the old stack and get a new one;
    process_stack = process_stack_provider(process_stack);

    //Update the process stack pointer;
    core_set_process_stack_pointer(process_stack->stack_pointer);

    //Unstack the context from the previous process_t stack
    core_unstack_process_context();

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
 *  the reload value is determined by :
 *
 *
 *      timer_period_seconds = 1 / F_CPU seconds;
 *      systick_period_seconds = systick_period_ms / 1000;
 *
 *      nb_ticks = systick_period / timer_period = F_CPU * systick_period_ms / 1000
 *
 *      ex : FCPU = 120 E6 , for 1 ms systick period, nb_ticks = 120 E6 / 1000 = 120 E3
 */

void core_systick_start(uint32_t systick_period_us, void (*systick_function)(void)) {

    //Set the systick function;
    _VectorsRam[15] = systick_function;

    //Set the systick priority to the highest priority level;
    NVIC_SET_PRIORITY(-1, 0);

    //Update the reload value register;
    SYST_RVR = (uint32_t) (((float) systick_period_us) * ((float) F_CPU / (float) 1000));

    //Clear the systick flag;
    SCB_ICSR &= ~SCB_ICSR_PENDSTSET;

    //Enable the systick interrupt;
    SYST_CSR |= SYST_CSR_ENABLE;

}

