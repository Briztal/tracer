//
// Created by root on 8/28/18.
//

#ifndef TRACER_CORE_H
#define TRACER_CORE_H


/*
 * The core library provides an abstraction layer over the core module. It references all functions that are available
 * 	for a hardware independent to run;
 */

#include <stdint.h>
#include <core/type/stack.h>

#define ENABLE_CORE_ERRORS


//-------------------------------------------------- Execution --------------------------------------------------

//Execute a function;
void core_execute_process_DO_NOT_USE_THIS(void *stack_ptr, void (*function)(void *), void *arg);

//Initialise the stack for initialisation. Implemented by the proc lib;
extern void core_init_stack(struct core_stack *stack, void (*function)(), void (*end_loop)(), void *init_arg);

//Get the initial arg;
extern void *core_get_init_arg();


//----------------------------------------------------- Preemption -----------------------------------------------------

//Updates the stack provider;
void core_set_process_provider(void *(*new_provider)(void *));

//Resets the stack provider;
void core_reset_process_provider();

//Trigger the preemption;
extern void core_trigger_preemption();


//------------------------------------------------------- Systick ------------------------------------------------------




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
