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


#include <stdbool.h>

#include <string.h>

#include <kernel/scheduler/scheduler.h>

#include <driver/ic.h>


//The error message output. Null at init;
static void (*error_message_function)(const char *) = 0;


//------------------------------------------- Entry Point -------------------------------------------


//Update the error message output function;
void kernel_set_error_output(void (*error_msg)(const char *)) {

	//Update the log function;
	error_message_function = error_msg;

}


/*
 * kernel_error : this function is called when an error is encountered in the code;
 *
 *  It will signal the error in various ways.
 */

void kernel_error(const char *error_message) {

    //Stop the scheduler;
    scheduler_stop();

    //Exit all critical sections;
    ic_force_critical_section_exit();

    //TODO RESET IDLE INTERRUPT HANDLERS

    //TODO THREAD MODES;

    if (error_message_function) {

        //Transmit the message;
        (*error_message_function)(error_message);

    }

	//infinite SOS sequence on debug led;
	debug_sos();

    while (true);

}


/*
 * -------------------------------------------- Heap management ---------------------------------------------
 */

//TODO TMALLOC SYSCALL;

/*
 * kernel_malloc : the malloc to use across the code; It is thread safe, and checks for exceptions;
 */

/*
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
 */

/*
 * kernel_malloc_copy : allocates a memory zone, and copies the correct amount of data from the initialiser to
 *  the allocated zone;
 */

/*

void *kernel_malloc_copy(const size_t size, const void *const initialiser) {

    //Enter a critical section;
    kernel_enter_critical_section();

    //Allocate data in the heap;
    void *ptr = malloc(size);

    //Leave the critical section;
    kernel_leave_critical_section();

    //If the allocation failed, error;
    if (!ptr) {

        kernel_error("kernel.c : kernel_malloc_copy : malloc failed");

    }

    //Copy the initialiser data;
    memcpy(ptr, initialiser, size);

    //Return the allocated data;
    return ptr;

}
 */


/*
 * kernel_realloc : the realloc to use across the code; It is thread safe, and checks for exceptions;
 */

/*
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
*/

/*
 * kernel_free : the free to use across the code; It is thread safe, and checks for exceptions;
 */

/*

void kernel_free(void *data) {

    //Enter a critical section;
    kernel_enter_critical_section();

    free(data);

    //Leave the critical section;
    kernel_leave_critical_section();

}

 */


