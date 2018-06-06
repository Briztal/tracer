//
// Created by root on 4/6/18.
//

#ifndef CORE_ARM_CORTEXM4F
#define CORE_ARM_CORTEXM4F

#include "stdint.h"

#include "stdbool.h"

#include "kinetis.h"

/*
 * -------------------------------------- Types --------------------------------------
 */

//The stack element type;
typedef uint32_t stack_element_t;

//The stack pointer type;
typedef stack_element_t *stack_ptr_t;

//The process stack type;
typedef struct {
	
	//The current stack pointer;
    stack_ptr_t stack_pointer;

	//The beginning of the stack;
    stack_ptr_t stack_begin;
	
	//The end of the stack;
    stack_ptr_t stack_end;

} stack_t;


/*
 * -------------------------------------- Headers --------------------------------------
 */

//TODO CORE_EXECUTE(privilege level, function);
//Get the current thread stack pointer;//TODO get_thread_stack_pointer
//static inline void core_get_stack_pointer(sp);


//------------------------------------- Stack bounds -------------------------------------

//Get the stack begin from the allocated region;
inline void core_get_stack_begin(allocated_pointer, size);

//Get the stack end from the allocated region;
inline void core_get_stack_end(allocated_pointer, size);


//------------------------------------- Stack management  -------------------------------------

//Initialise a stack for a future unstacking. An arg pointer can be passed
void core_init_stack(void (*function)(), void (*end_loop)(), void *init_arg);

//Get the argument stored at stack init. Must be called at the very beginning of the process function;
void *core_get_init_arg();


//------------------------------------- Context switch -------------------------------------

//Trigger the preemption;
inline void core_preempt_process();


//------------------------------------- Execution modes and privilleges -------------------------------------

//Enter into process mode;
inline void core_enter_process_mode();

//Returns true if the current code runs in thread mode
inline bool core_in_interrupt();


//------------------------------------- Systick -------------------------------------

//Start the systick timer;
inline void core_systick_start(uint32_t systick_period_us, void (*systick_function)(void));


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


//Enable all interrupts;
inline void core_enable_interrupts();

//Disable all interrupts;
inline void core_disable_interrupts();


//Enable the specified interrupt;
inline void core_IC_enable(uint8_t interrupt_index);

//Disable the specified interrupt
inline void core_IC_disable(uint8_t interrupt_index); 

//Mark the interrupt pending. Will be executed asap;
inline void core_IC_set_pending(interrupt_index);
  
//Mark the interrupt as not pending;
inline void core_IC_clear_pending(uint8_t interrupt_index);

//Set the priority of the required interrupt to the povided;
inline void core_IC_set_priority(uint8_t interrupt_index, uint8_t priority); 

//Set the handler of the required interrupt;
inline void core_IC_set_handler(uint8_t interrupt_index, void (*handler)());


/*
 * ------------------------------------- Stack bounds -------------------------------------
 */

/*
 * core_get_stack_begin : determines the stacks beginning case from the allocated pointer and the size;
 *
 *  In an arm cortex, the stack decreases, and the stack pointer points to the last pushed element;
 */

inline void core_get_stack_begin(allocated_pointer, size) {
	return ((allocated_pointer) + (size));
}


/*
 * core_get_stack_end : determines the stacks end case from the allocated pointer and the size;
 *
 *  In an arm cortex, the stack decreases, and the stack pointer points to the last pushed element;
 */

inline void core_get_stack_end(allocated_pointer, size) {
	return  (allocated_pointer);
}



/*
 * ------------------------------------- Context switch -------------------------------------
 */

/*
 * core_preempt_process : sets the pendSV exception isPending flag;
 */

static inline void core_preempt_process() {}
    SCB_ICSR |= SCB_ICSR_PENDSVSET;
}


/*
 * ------------------------------------- Execution modes and privilleges -------------------------------------
 */

/*
 * core_enter_process_mode : updates the control register to use the process stack;
 */

static inline void core_enter_process_mode() {

	//TODO UNPRIVILEGE
   	
	//TODO Exec. ISB after changing CONTORL (recommended);
	__asm__ __volatile__(\
        "mov r4, #2 \n\t"\
        "msr control, r4\n\t"\
	);

}


/*
 * core_in_interrupt : this function return true if the current code is ran from an interrup;
 *
 *  If we are an interrupt, the 8 first bits of the System Control Block will contain the exception number.
 *
 *  If those are 0, no interupt is running yet;
 */

static inline bool core_in_interrupt() {
    return (bool) ((uint8_t) SCB_ICSR);
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

inline void core_systick_start(uint32_t systick_period_us, void (*systick_function)(void)) {

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


/*
 * ------------------------------------- Interrupts -------------------------------------
 */

/*
 * core_enable_interrupts : injects inline assembly code to disable all interrupts. 
 * 	Fault handling is still enabled;
 */

static inline void core_enable_interrupts() {
	__asm__ __volatile__("cpsie i");
}


/*
 * core_disable_interrupts : injects inline assembly code to disable all interrupts. 
 * 	Fault handling is still enabled;
 */

static inline void core_disable_interrupts() {
	__asm__ __volatile__("cpsid i");
}


/*
 * core_IC_enable : enables the specified interrupt;
 */

inline void core_IC_enable(uint8_t interrupt_index) {
    NVIC_ENABLE_IRQ(interrupt_index);
}


/*
 * core_IC_disable : disables the specified interrupt;
 */

inline void core_IC_disable(uint8_t interrupt_index) {
    NVIC_DISABLE_IRQ(interrupt_index);
}


/*
 * core_IC_set_pending : marks the interrupt as pending. Will be executed asap;
 */

inline void core_IC_set_pending(uint8_t interrupt_index) {
    NVIC_SET_PENDING(interrupt_index);
}


/*
 * core_IC_clear_pending :  marks the interrupt as not pending;
 */

inline void core_IC_clear_pending(uint8_t interrupt_index) {
    NVIC_CLEAR_PENDING(interrupt_index);
}


/*
 * core_IC_set_priority : sets the priority of the required interrupt to the povided;
 */

inline void core_IC_set_priority(uint8_t interrupt_index, uint8_t priority) {
    NVIC_SET_PRIORITY(interrupt_index, priority);
}


/*
 * core_IC_set_handler : sets the handler of the required interrupt;
 */

inline void core_IC_set_handler(uint8_t interrupt_index, void (*handler)()) {
    _VectorsRam[16 + (interrupt_index)] = handler;
}


/*
 * ------------------------------------- Context switch -------------------------------------
 */

/*
 * core_set_context_switcher : sets the function to be called when a context switch is required;
 */

/*

inline void core_set_context_switcher(context_switcher) {
    _VectorsRam[14] = context_switcher;
    NVIC_SET_PRIORITY(-2, 240);
}

*/

#endif
