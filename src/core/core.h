//
// Created by root on 8/28/18.
//

#ifndef TRACER_CORE_H
#define TRACER_CORE_H


#include <stdbool.h>

#include <stdint.h>

#include <mem/stack.h>

/*
 * The core library provides an abstraction layer over the core module.
 *
 * 	This file references all data and functions that are available for a hardware independent application to run;
 */

//TODO CORE CONFIG;
#define ENABLE_CORE_ERRORS


//-------------------------------------------------- Execution --------------------------------------------------

//The number of threads;
extern const uint8_t core_nb_threads;

/**
 * core_enter_thread_mode : this function initialises threads in a safe state. It never returns.
 * 	The preemption environment must have been initialised before;
 *
 * 	- initialises all threads in their lowest privilege state;
 * 	- updates exception stacks;
 * 	- calls the provided function, that should trigger the preemption;
 * 	- make all threads run an infinite loop;
 * 	- execute the preemption call;
 * 	- enable interrupts;
 * 	- loop;
 *
 * @param exception_stacks : stacks that must be used in case of interrupt;
 */

extern void core_enter_thread_mode(struct core_stack **exception_stacks, void (*preemption_call)());


//Initialise the stack for initialisation. Implemented by the proc lib;
extern void core_init_stack(struct core_stack *stack, void (*function)(), void (*end_loop)(), void *init_arg);

//Get the initial arg;
extern void *core_get_init_arg();


//--------------------------------------------------- Context Switch ---------------------------------------------------

//The core context switcher; Should be set as the preemption handler for a preemption to occur;
extern void core_context_switcher();

//Updates the stack provider;
void core_set_stack_provider(void *(*new_provider)(uint8_t, void *));


//----------------------------------------------------- Preemption -----------------------------------------------------

//Set the handler of the preemption exception;
extern void core_preemption_set_handler(void (*)(void));

//Set the priority of the preemption exception;
extern void core_preemption_set_priority(uint8_t priority);

//Enable the preemption exception;
extern void core_preemption_enable();

//Trigger the preemption;
extern void core_preemption_trigger();


//----------------------------------------------------- Syscall -----------------------------------------------------

//Set the handler of the syscall exception;
extern void core_syscall_set_handler(void (*)(void));

//Set the priority of the syscall exception;
extern void core_syscall_set_priority(uint8_t priority);

//Enable the syscall exception;
extern void core_syscall_enable();

//Trigger the syscall;
extern void core_syscall_trigger();


//------------------------------------------------------- Systick ------------------------------------------------------

/*
 * The core library provides a system timer, with basic functions.
 */

//Start the core timer;
extern void core_timer_start();

//Stop the core timer;
extern void core_timer_stop();

//Enable the core interrupt;
extern void core_timer_int_enable();

//Disable the core interrupt;
extern void core_timer_int_disable();


//Set the core timer interrupt frequency. The core frequency must be known;
extern void core_timer_int_set_frequency(uint32_t frequency);

//Set the core timer interrupt priority;
extern void core_timer_int_set_priority(uint8_t priority);

//Set the core timer interrupt handler;
extern void core_timer_int_set_handler(void (*handler)());



//---------------------------------------------------- Temporisation ---------------------------------------------------

//Wait a certain number of milliseconds;
extern void core_delay_ms(uint32_t ms);

//Wait a certain number of milliseconds;
extern void core_delay_us(uint32_t us);


//---------------------------------------------------- Communication ---------------------------------------------------

//Light the debug led high;
extern void core_led_high();

//Turn off the debug led;
extern void core_led_low();

//Send a log message;
extern void core_log(const char *log);

//Convert an int to its string value and send it;
void core_log_int(uint32_t integer);

//Convert an int to its hex string value and send it;
void core_log_hex(uint32_t integer);


//---------------------------------------------- General Interrupt control ---------------------------------------------

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

//A driver status interrupt has a low priority;
#define KERNEL_DRIVER_STATUS_PRIORITY ic_priority_2

//A driver error must be caught, and so has a higher priority;
#define KERNEL_DRIVER_ERROR_PRIORITY ic_priority_4

//The systick has the highest priority level, so that ticks are never missed;
#define KERNEL_SYSTICK_PRIORITY ic_priority_6


/*
 * In order to use non system interrupts, general functions are implemented by the core module;
 */

//Enable the interrupt control;
void ic_enable_interrupts();

//Disables the interrupt control;
void ic_disable_interrupts();

//Enables the required interrupt channel;
void ic_enable_interrupt(uint16_t channel);

//Disables the required interrupt channel;
void ic_disable_interrupt(uint16_t channel);

//Sets the required non-system interrupt in the pending state;
void ic_set_interrupt_pending(uint16_t channel);

//Sets the required non-system interrupt in the not-pending state;
void ic_clear_interrupt_pending(uint16_t channel);

//Sets the required non-system interrupt in the not-pending state;
void ic_is_interrupt_pending(uint16_t channel);

//Applies the provided priority to the required non-system interupt channel;
void ic_set_interrupt_priority(uint16_t channel, uint8_t priority);

//Returns the priority to the required non-system interupt channel;
uint8_t ic_get_interrupt_priority(uint16_t channel, uint8_t priority);

//Sets the handler of the required channel to 0, isr will return immediately;
void ic_set_interrupt_handler(uint16_t channel, void (*handler)(void));

//Resets the handler of the required channel to 0, isr will return immediately;
void ic_reset_interrupt_handler(uint16_t channel);

//Assert if a handler is currently in execution;
bool ic_in_handler_mode();


//-------------------------------------------------- Critical sections -------------------------------------------------

//Enter a critical section;
void ic_enter_critical_section();

//Leave a critical section;
void ic_leave_critical_section();

//Force leaving a critical section. Unsafe, must be executed by the kernel only;
void ic_force_critical_section_exit();


//-------------------------------------------------- spinlocks -------------------------------------------------

//TODO



//------------------------------------------------------- Error --------------------------------------------------------

/*
 * If core errors are enabled, link core_error to _core_error. If not, link it to nothing, strings will not be
 * 	generated in .rodata;
 */

#ifdef ENABLE_CORE_ERRORS
#define core_error(s) _core_error(s);
#else
#define core_error(s) {while(1);}
#endif


//Halt and send a message continuously;
void _core_error(const char *msg);


#endif //TRACER_CORE_H
