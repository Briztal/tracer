//
// Created by root on 9/8/18.
//

#ifndef TRACER_IC_H
#define TRACER_IC_H

#include <stdbool.h>

#include <stdint.h>


/*
 * General interrupt priorities;
 */

//The lowest priority level;
extern const uint8_t ic_priority_0;

//Priority 1;
extern const uint8_t ic_priority_1;

//Priority 2;
extern const uint8_t ic_priority_2;

//Priority 3;
extern const uint8_t ic_priority_3;

//Priority 4;
extern const uint8_t ic_priority_4;

//Priority 5;
extern const uint8_t ic_priority_5;

//Priority 6;
extern const uint8_t ic_priority_6;

//The highest priority level;
extern const uint8_t ic_priority_7;


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


/*
 * In order to use non system interrupts, general functions are implemented by the core module;
 */

//Enable the interrupt control;
extern void ic_enable_interrupts();

//Disables the interrupt control;
extern void ic_disable_interrupts();

//Enables the required interrupt channel;
extern void ic_enable_interrupt(uint16_t channel);

//Disables the required interrupt channel;
extern void ic_disable_interrupt(uint16_t channel);

//Sets the required non-system interrupt in the pending state;
extern void ic_set_interrupt_pending(uint16_t channel);

//Sets the required non-system interrupt in the not-pending state;
extern void ic_clear_interrupt_pending(uint16_t channel);

//Sets the required non-system interrupt in the not-pending state;
extern void ic_is_interrupt_pending(uint16_t channel);

//Applies the provided priority to the required non-system interupt channel;
extern void ic_set_interrupt_priority(uint16_t channel, uint8_t priority);

//Returns the priority to the required non-system interupt channel;
extern uint8_t ic_get_interrupt_priority(uint16_t channel, uint8_t priority);

//Sets the handler of the required channel to 0, isr will return immediately;
extern void ic_set_interrupt_handler(uint16_t channel, void (*handler)(void));

//Resets the handler of the required channel to 0, isr will return immediately;
extern void ic_reset_interrupt_handler(uint16_t channel);

//Assert if a handler is currently in execution;
extern bool ic_in_handler_mode();


//-------------------------------------------------- Critical sections -------------------------------------------------

//Enter a critical section;
void ic_enter_critical_section();

//Leave a critical section;
void ic_leave_critical_section();

//Force leaving a critical section. Unsafe, must be executed by the kernel only;
void ic_force_critical_section_exit();



//Set the handler of the preemption exception;
extern void preemption_set_handler(void (*)(void));

//Set the priority of the preemption exception;
extern void preemption_set_priority(uint8_t priority);

//Enable the preemption exception;
extern void preemption_enable();

//Set the preemption exception pending;
extern void preemption_set_pending();

//Set the preemption exception not pending;
extern void preemption_clear_pending();



//Set the handler of the syscall exception;
extern void syscall_set_handler(void (*)(void));

//Set the priority of the syscall exception;
extern void syscall_set_priority(uint8_t priority);

//Enable the syscall exception;
extern void syscall_enable();

//Trigger the syscall;
extern void syscall_trigger();



#endif //TRACER_IC_H
