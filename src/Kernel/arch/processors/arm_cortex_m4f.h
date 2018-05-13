//
// Created by root on 4/6/18.
//

#ifndef CORE_ARM_CORTEXM4F
#define CORE_ARM_CORTEXM4F

#include "stdint.h"

#include "stdbool.h"

#include "kinetis.h"

//The stack element type;
typedef uint32_t stack_element_t;

//The stack pointer type;
typedef stack_element_t *stack_ptr_t;


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
 * core_set_thread_stack_pointer : creates a temporary variable to contain the casted stack pointer,
 *  and injects assembly inline to move the data of the temp into psp;
 */

static inline void core_set_thread_stack_pointer(const stack_ptr_t sp) {
    __asm__ __volatile__ ("msr psp, %0"::"r" (sp));\

}


/*
 * core_get_thread_stack_pointer : injects assembly code in order to move the data of psp into the
 *  provided variable -> sp must be an existing variable name;
 */

static inline stack_ptr_t core_get_thread_stack_pointer() {
    stack_ptr_t ptr = 0;
    __asm__ __volatile__ ("mrs %0, psp" : "=r" ((uint32_t) ptr):);
    return ptr;
}


/*
 * core_stack_thread_context : injects assembly code in order to stack memory that are not
 *  stacked automatically in the process_t stack;
 *
 *  It moves psp into r0, and then stacks r4-r7, general purposes memory, and finally
 *      stacks s16- s31, floating point unit memory;
 *
 *  You may notice that PSP remains unchanged after the stacking. This is done with purpose, in order to
 *      allow the processor to unstack properly even if core_unstack_context is called;
 */

static inline void core_stack_thread_context() {
    uint32_t scratch;
    __asm__ __volatile__ (\
        "mrs %0, psp \n\t"\
        "stmdb %0!, {r4 - r11}\n\t"\
        "vstmdb %0!, {s16 - s31}" : "=r" (scratch)\
    );
}


/*
 * core_stack_thread_context : injects assembly code in order to stack memory that are not
 *  stacked automatically in the process_t stack;
 *
 *  It moves psp into r0, and then unstacks r4-r7, general purposes memory, and finally
 *      unstacks s16- s31, floating point unit memory;
 *
 *  As core_stack_thread_context didn't alter PSP, this function doesn't either;
 */

static inline void core_unstack_thread_context() {
    uint32_t scratch;
    __asm__ __volatile__ (\
        "mrs %0, psp \n\t"\
        "ldmdb %0!, {r4 - r11} \n\t"\
        "vldmdb %0!, {s16 - s31} " : "=r" (scratch)\
    );
}


/*
 * core_init_stack : this function initialises the unstacking environment, so that the given function will
 *  be executed at context switch time;
 *
 *  It starts by caching the process_t stack pointer, and stacks the thread functions pointers, and the PSR.
 *
 *  Then, it saves the thread index in R12 (next word);
 *
 *  Finally, it leaves space for empty stack frame and saves the stack pointer;
 */

static inline void core_init_stack(void (*function)(), void (*end_loop)(), stack_element_t process_pointer) {
    uint32_t *__core_init_stack_sp__ = core_get_thread_stack_pointer();
    *(__core_init_stack_sp__ - 1) = 0x01000000;
    *(__core_init_stack_sp__ - 2) = (uint32_t) (function);
    *(__core_init_stack_sp__ - 3) = (uint32_t) (end_loop);
    *(__core_init_stack_sp__ - 4) = (uint32_t) (process_pointer);
    __core_init_stack_sp__ -= 8;
    core_set_thread_stack_pointer(__core_init_stack_sp__);
}


/*
 * core_get_process : this function will set thread_id to the value sored in r12, where was stored the thread
 *  index in the core_init_stack macro;
 */

static inline stack_element_t core_get_process() {
    stack_element_t thread_index = 0;
    __asm__ __volatile__("mov %0, r12": "=r" (thread_index):);
    return thread_index;

}


/*
 * core_trigger_context_switch : sets the pendSV exception isPending flag;
 */

#define core_trigger_context_switch()\
    SCB_ICSR |= SCB_ICSR_PENDSVSET


/*
 * core_trigger_context_switch : sets the pendSV exception isPending flag;
 */

#define core_untriggerr_context_switch()\
    SCB_ICSR |= SCB_ICSR_PENDSVCLR


/*
 * core_enable_interrupts : injects inline assembly code to disable all interrupts. Fault handling is still enabled;
 */

#define core_enable_interrupts() __asm__ __volatile__("cpsie i")

/*
 * core_disable_interrupts : injects inline assembly code to disable all interrupts. Fault handling is still enabled;
 */

#define core_disable_interrupts() __asm__ __volatile__("cpsid i")


/*
 * core_set_thread_mode : set the processor to use the psp, and stay privileged in thread mode;
 */

//TODO UNPRIVILEGE
#define core_set_thread_mode()\
    __asm__ __volatile__(\
        "mov r4, #2 \n\t"\
        "msr control, r4\n\t"\
    )/* Exec. ISB after changing CONTORL (recommended) */\



#define core_get_stack_pointer(sp)\
    __asm__ __volatile__(\
        "mov %0, sp" : "=r" (sp)\
    )



/*
 * ------------------------------------- Stack bound -------------------------------------
 */

/*
 * core_get_stack_begin : determines the stacks beginning case from the allocated pointer and the size;
 *
 *  In an arm cortex, the stack decreases, and the stack pointer points to the last pushed element;
 */

#define core_get_stack_begin(allocated_pointer, size) ((allocated_pointer) + (size))


/*
 * core_get_stack_begin : determines the stacks end case from the allocated pointer and the size;
 *
 *  In an arm cortex, the stack decreases, and the stack pointer points to the last pushed element;
 */

#define core_get_stack_end(allocated_pointer, size) (allocated_pointer)

/*
 * ------------------------------------- Context switch -------------------------------------
 */

/*
 * core_set_context_switcher : sets the function to be called when a context switch is required;
 */

#define core_set_context_switcher(context_switcher)\
    _VectorsRam[14] = context_switcher;\
    NVIC_SET_PRIORITY(-2, 240);


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

inline void core_start_systick_timer(uint32_t systick_period_us, void (*systick_function)(void)) {

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
 * core_enable_interrupt : enables the specified interrupt;
 */

inline void core_enable_interrupt(uint8_t interrupt_index) {
    NVIC_ENABLE_IRQ(interrupt_index);
}



/*
 * core_disable_interrupt : disables the specified interrupt;
 */

#define core_disable_interrupt(interrupt_index)\
    NVIC_DISABLE_IRQ(interrupt_index);


/*
 * core_set_interrupt_pending : marks the interrupt as pending. Will be executed asap;
 */

#define core_set_interrupt_pending(interrupt_index)\
    NVIC_SET_PENDING(interrupt_index);


/*
 * core_clear_interrupt_pending : marks the interrupt as not pending;
 */

inline void core_clear_interrupt_pending(uint8_t interrupt_index) {
    NVIC_CLEAR_PENDING(interrupt_index);
}


/*
 * core_set_interrupt_priority : sets the priority of the required interrupt to the povided;
 */

inline void core_set_interrupt_priority(uint8_t interrupt_index, uint8_t priority) {
    NVIC_SET_PRIORITY(interrupt_index, priority);
}


/*
 * Priority levels : these are the standard interrupt priorities for drivers;
 *
 * Cortex-M4: 0,16,32,48,64,80,96,112,128,144,160,176,192,208,224,240
 */

//The standard priority for status interrupt;
#define DRIVER_STARUS_INTERRUPT_PRIORITY 32

//The standard priority for error interrupt;
#define DRIVER_ERROR_INTERRUPT_PRIORITY 16


/*
 * core_set_interrupt_handler : sets the handler of the required interrupt;
 */

inline void core_set_interrupt_handler(uint8_t interrupt_index, void (*handler)()) {
    _VectorsRam[16 + (interrupt_index)] = handler;
}


/*
 * core_in_thread_mode : this function return true if the current code runs in thread mode;
 *
 *  If we are in an interrupt, the 8 first bits of the System Control Block will contain the exception number.
 *
 *  If they are 0, we are in thread mode;
 */

static inline bool core_in_thread_mode() {
    return (bool) (!(uint8_t) SCB_ICSR);
}


#endif






