/*
  kernel.c Part of TRACER

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

#include "kernel.h"

#include <stdbool.h>

#include <malloc.h>

#include <string.h>

#include <kernel/systick.h>

#include <kernel/scheduler/scheduler.h>
#include <data_structures/containers/non_concurrent/llist.h>

#include <kernel/arch/processors/arm_cortex_m4f.h>
#include <kernel/scheduler/tasks/sequences.h>
#include <kernel/scheduler/tasks/service.h>


//The critical section counter;
static uint32_t critical_section_counter = 0;


//As exit functions are registered by enabled modules, we need a linked struct to store function pointers;
typedef struct {

    //The linked element part;
    linked_element_t link;

    //The function part;
    void (*exit_function)(void);

} exit_function_t;

//The exit functions list;
linked_list_t exit_list = EMPTY_LINKED_LIST(255);


//------------------------------------------- Entry Point -------------------------------------------



/*
 * The first function called by the scheduler;
 */

void first_function(void *args) {


}


/*
 * kernel_init : this function is called once, by the core initialisation only. It is the project's entry point.
 */

void kernel_init() {

    //Start the scheduler;
    scheduler_start(&first_function);

}


/*
 * kernel_register_exit_function : adds the function to the list of exit functions;
 */

void kernel_register_exit_function(void (*exit_function)(void)) {

    //Initialise the exit function struct;
    exit_function_t init =  {
            .link = EMPTY_LINKED_ELEMENT(),
            .exit_function = exit_function,
    };

    //Allocate some heap data for the exit function struct;
    exit_function_t *data = kernel_malloc_copy(sizeof(exit_function_t), &init);

    //Append the exit function struct at the end of the exit list;
    llist_insert_last(&exit_list, (linked_element_t *) data);

}


/*
 * kernel_exit : executes all exit functions, and delete them;
 */

void kernel_exit() {

    //For each exit functions :
    for (size_t exit_counter = exit_list.nb_elements; exit_counter--;) {

        //Remove and cache the first element of the list;
        const exit_function_t *function = (exit_function_t *) llist_remove_first(&exit_list);

        //Execute the function;
        (*(function->exit_function))();

        //Delete the exit function struct;
        kernel_free((void *) function);

    }

}


/*
 * kernel_error : this function is called when an error is encountered in the code;
 *
 *  It will signal the error in various ways.
 */

void kernel_error(const char *log_message) {

    //Stop the scheduler;
    scheduler_stop();

    //Enable all interrupts;
    core_enable_interrupts();

    //Reset the critical section counter;
    critical_section_counter = 0;

    //TODO RESET IDLE INTERRUPT HANDLERS

    //TODO THREAD MODES;

    //Handle the error;
    arch_handle_error(log_message);

    while (true);

}


void kernel_halt(uint16_t delay) {

    //Stop all started processes;
    scheduler_stop();

    //Enable all interrupts;
    core_enable_interrupts();

    //Reset the critical section counter;
    critical_section_counter = 0;

    //TODO RESET IDLE INTERRUPT HANDLERS

    //TODO THREAD MODES;

    //Handle the error;
    arch_blink(delay);

}


void kernel_count(size_t count) {

    //Stop all started processes;
    scheduler_stop();

    //Enable all interrupts;
    core_enable_interrupts();

    //Reset the critical section counter;
    critical_section_counter = 0;

    //TODO RESET IDLE INTERRUPT HANDLERS

    //TODO THREAD MODES;

    //Handle the error;
    arch_count(count);

}


/*
 * -------------------------------------------- Stack management ---------------------------------------------
 */

/*
 * kernel_stack_alloc : this function allocates a stack, and then initialises stack pointers;
 *
 *  Finally, it sets the thread's sequences_initialised as Terminated;
 */

void kernel_stack_alloc(core_stack_t *stack, size_t stack_size,
                        void (*function)(), void (*exit_loop)(), void *arg) {

    //Truncate the size to get a size matchng with the primitive type;
    stack_size = core_correct_size(stack_size);

    //Then, allocate a memory zone of the required size;
    void *sp = kernel_malloc(stack_size);

    //Set the stack's begin;
    stack->stack_begin = core_get_stack_begin(sp, stack_size);

    //Set the stack's end;
    stack->stack_end = core_get_stack_end(sp, stack_size);

    //Reset the stack;
    kernel_stack_reset(stack, function, exit_loop, arg);

}


/*
 * kernel_reset_stack : resets the process_t to its initial stack pointer,
 * 	and initialises the context to its initial;
 */

void kernel_stack_reset(core_stack_t *stack, void (*init_f)(), void (*exit_f)(), void *arg) {

    //Initialise the stack;
    core_init_stack(stack, init_f, exit_f, arg);

}


/*
 * kernel_stack_free : frees the stack;
 */

void kernel_stack_free(core_stack_t *stack) {

    //Free the stack;
    kernel_free(stack);

}


/*
 * -------------------------------------------- Heap management ---------------------------------------------
 */

/*
 * kernel_malloc : the malloc to use across the code; It is thread safe, and checks for exceptions;
 */

void *kernel_malloc(size_t size) {

    //Enter a critical section;
    kernel_enter_critical_section();

    //Allocate data in the heap;
    void *ptr = malloc(size);

    //Leave the critical section;
    kernel_leave_critical_section();

    //If the allocation failed, error;
    if (!ptr)
        kernel_error("kernel.c : kernel_malloc : malloc failed");

    //Return the allocated data;
    return ptr;

}


/*
 * kernel_malloc_copy : allocates a memory zone, and copies the correct amount of data from the initialiser to
 *  the allocated zone;
 */

void *kernel_malloc_copy(const size_t size, const void *const initialiser) {

    //Enter a critical section;
    kernel_enter_critical_section();

    //Allocate data in the heap;
    void *ptr = malloc(size);

    //Leave the critical section;
    kernel_leave_critical_section();

    //If the allocation failed, error;
    if (!ptr)
        kernel_error("kernel.c : kernel_malloc_copy : malloc failed");

    //Copy the initialiser data;
    memcpy(ptr, initialiser, size);

    //Return the allocated data;
    return ptr;


}


/*
 * kernel_realloc : the realloc to use across the code; It is thread safe, and checks for exceptions;
 */

void *kernel_realloc(void *data, size_t size) {

    //Enter a critical section;
    kernel_enter_critical_section();

    //Reallocate data in the heap;
    void *ptr = realloc(data, size);

    //Leave the critical section;
    kernel_leave_critical_section();

    //If the reallocation failed, error;
    if (!ptr)
        kernel_error("kernel.c : kernel_realloc : realloc failed");

    //Return the reallocated data;
    return ptr;


}


/*
 * kernel_free : the free to use across the code; It is thread safe, and checks for exceptions;
 */

void kernel_free(void *data) {

    //Enter a critical section;
    kernel_enter_critical_section();

    free(data);

    //Leave the critical section;
    kernel_leave_critical_section();

}


/*
 * kernel_enter_critical_section : called whenever any part of the code must execute a critical section;
 */

inline void kernel_enter_critical_section() {

    //Disable interrupts;
    core_disable_interrupts();

    //Increment the section counter;
    critical_section_counter++;

}


/*
 * kernel_leave_critical_section : called whenever any part of the code leaves a critical section;
 */

inline void kernel_leave_critical_section() {

    //To safely detect any code error, disable interrupts;
    core_disable_interrupts();

    //If there was a code error (more leave called than enter);
    if (!critical_section_counter) {

        //TODO EXCEPTION;

    }

    //If we must enable interrupts again :
    if (!(--critical_section_counter)) {

        //Enable interrupts;
        core_enable_interrupts();

    }


}
