//
// Created by root on 4/6/18.
//


#include "stdint.h"

//The stack pointer type;
typedef uint32_t *stack_ptr_t;


/*
 * core_set_thread_stack_pointer : creates a temporary variable to contain the casted stack pointer,
 *  and injects assembly inline to move the content of the temp into psp;
 */

#define core_set_thread_stack_pointer(sp) {\
        uint32_t __temp_var_core_set_thread_stack_pointer__ = (uint32_t) (sp);\
        __asm__ __volatile__ ("msr psp, %0" : "r" (__temp_var_core_set_thread_stack_pointer__));\
    }


/*
 * core_get_thread_stack_pointer : injects assembly code in order to move the content of psp into the
 *  provided variable -> sp must be an existing variable name;
 */

#define core_get_thread_stack_pointer(sp) {\
        __asm__ __volatile__ ("mrs %0, psp" : "=r" ((uint32_t)sp));\
    }


/*
 * core_stack_thread_context : injects assembly code in order to stack registers that are not
 *  stacked automatically in the process stack;
 *
 *  It moves psp into r0, and then stacks r4-r7, general purposes registers, and finally
 *      stacks s16- s31, floating point unit registers;
 *
 *  You may notice that PSP remains unchanged after the stacking. This is done with purpose, in order to
 *      allow the processor to unstack properly even if core_unstack_context is called;
 */

#define core_stack_thread_context()\
    __asm__ __volatile__ (\
        "mrs r0, psp \n"\
        "stmdb r0, {r4 - r11, s16 - s31}"\
    )



/*
 * core_stack_thread_context : injects assembly code in order to stack registers that are not
 *  stacked automatically in the process stack;
 *
 *  It moves psp into r0, and then unstacks r4-r7, general purposes registers, and finally
 *      unstacks s16- s31, floating point unit registers;
 *
 *  As core_stack_thread_context didn't alter PSP, this function doesn't either;
 */

#define core_unstack_thread_context()\
    __asm__ __volatile__ (\
        "mrs r0, psp \n"\
        "ldmdb r0, {r4 - r11, s16 - s31}"\
    )


/*
 * core_init_stack : this function initialises the unstacking environment, so that the given function will
 *  be executed at context switch time;
 *
 *  It starts by caching the process stack pointer, and stacks the thread functions pointers, and the PSR.
 *
 *  It then leaves space for empty stack frame and saves the stack pointer;
 */

#define core_init_stack(function, end_loop) {\
        uint32_t *__core_init_stack_sp__ = 0;\
        core_get_thread_stack_pointer(__core_init_stack_sp__);\
        *(__core_init_stack_sp__ - 1) = 0x01000000;\
        *(__core_init_stack_sp__ - 2) = (uint32_t) (function);\
        *(__core_init_stack_sp__ - 3) = (uint32_t) (end_loop);\
        __core_init_stack_sp__ -= 8;\
        core_set_thread_stack_pointer(__core_init_stack_sp__);\
    }



/*
 * core_trigger_context_switch : sets the pendSV exception pending flag;
 */

#define core_trigger_context_switch() NVIC_SET_PENDING(-2);


/*
 * core_enable_interrupts : injects inline assembly code to disable all interrupts. Fault handling is still enabled;
 */

#define core_enable_interrupts() __asm__ __volatile__("cpsie i")

/*
 * core_disable_interrupts : injects inline assembly code to disable all interrupts. Fault handling is still enabled;
 */

#define core_disable_interrupts() __asm__ __volatile__("cpsid i")


//TODO SET USER MODE;
#define core_set_user_mode()\
    __asm__ __volatile__(\
        "mrs r0, cpsr \n"\
        "bic r0, #31 \n"\
        "add r0, r0, #16 \n"\
        "msr cpsr, r0 \n"\
    )