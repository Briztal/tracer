//
// Created by root on 9/8/18.
//

#ifndef TRACER_IC_H
#define TRACER_IC_H

#include <stdbool.h>

#include <stdint.h>

#include <kernel/hard.h>


//-------------------------------------------- General interrupt priorities --------------------------------------------

//The lowest priority level;
#define ic_priority_0 __ic_priority_0
#define ic_priority_1 __ic_priority_1
#define ic_priority_2 __ic_priority_2
#define ic_priority_3 __ic_priority_3
#define ic_priority_4 __ic_priority_4
#define ic_priority_5 __ic_priority_5
#define ic_priority_6 __ic_priority_6
#define ic_priority_7 __ic_priority_7
//The highest priority level;


/*
 * In the kernel, some priority levels are used for particular exceptions and interrupts;
 */

//The preemption has the lowest priority level, so that a context switch never occurs over an interrupt;
#define KERNEL_PREMPTION_PRIORITY ic_priority_0

//A interface status interrupt has a low priority;
#define KERNEL_DRIVER_STATUS_PRIORITY ic_priority_2

//A interface error must be caught, and so has a higher priority;
#define KERNEL_DRIVER_ERROR_PRIORITY ic_priority_4

//The systick has the highest priority level, so that ticks are never missed;
#define KERNEL_SYSTICK_PRIORITY ic_priority_6


//-------------------------------------------------- Exceptions -------------------------------------------------

/*
 * In order to use non system interrupts, general functions are implemented by the core module;
 */

//Enable the interrupt control;
#define exceptions_enable() __exceptions_enable()

//Disables the interrupt control;
#define exceptions_disable() __exceptions_disable()

//Sets the handler of the required exception;
#define exception_set_handler(channel, handler) __exception_set_handler(channel, handler)


//-------------------------------------------------- Interrupt ReQuest -------------------------------------------------

//Enables the required interrupt channel;
#define irq_enable(channel) __irq_enable(channel)

//Disables the required interrupt channel;
#define irq_disable(channel) __irq_disable(channel)

//Sets the required non-system interrupt in the pending state;
#define irq_set_pending(channel) __irq_set_pending(channel)

//Sets the required non-system interrupt in the not-pending state;
#define irq_clear_pending(channel) __irq_clear_pending(channel)

//Sets the required non-system interrupt in the not-pending state;
#define irq_is_pending(channel) __irq_is_pending(channel)

//Applies the provided priority to the required non-system interupt channel;
#define irq_set_priority(channel, priority) __irq_set_priority(channel, priority)

//Returns the priority to the required non-system interupt channel;
#define irq_get_priority(channel, priority) __irq_get_priority(channel, priority)

//Update the handler of the required irq;
#define irq_set_handler(channel, handler) __irq_set_handler(channel, handler)

//Resets the handler of the required channel to 0, isr will return immediately;
#define irq_reset_interrupt_handler(channel) __irq_reset_interrupt_handler(channel)

//Assert if a handler is currently in execution;
#define irq_in_handler_mode() __irq_in_handler_mode()


//-------------------------------------------------- Critical sections -------------------------------------------------

//Enter a critical section;
void critical_section_enter();

//Leave a critical section;
void critical_section_leave();

//Force leaving a critical section. Unsafe, must be executed by the kernel only;
void critical_section_force_exit();




#endif //TRACER_IC_H
