/*
  kernel.cpp Part of TRACER

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


#include <kernel/scheduler/process.h>



//The start flag, set only once, at the entry point. Prevents start to be called a second time.
bool started = false;

//The critical section counter;
static uint32_t critical_section_counter = 0;

//------------------------------------------- Entry Point -------------------------------------------

/*
 * start : this function is called once, by the core initialisation only. It is the project's entry point.
 */

void kernel_start() {

    //This function can be called only once. If it has already been called, fail.
    if (started)
        return;

    //Set the flag to prevent multiple executions;
    started = true;

    //TODO IN CORE_INIT FUNCTION, CALLED BEFORE kernel star

    //Start the thread manager;
    process_start_execution();

}

/*
 * kernel_error : this function is called when an error is encountered in the code;
 *
 *  It will signal the error in various ways.
 */

void dumb(){};

void kernel_error(const char *log_message) {

    //Stop all started processes;
    process_stop_execution();

    //Enable all interrupts;
    core_enable_interrupts();

    //Reset the critical section counter;
    critical_section_counter = 0;

    //Disable the context switcher;
    core_set_context_switcher(&dumb);

    //TODO RESET IDLE INTERRUPT HANDLERS

    //TODO THREAD MODES;

    //Handle the error;
    arch_handle_error(log_message);

}

void kernel_halt(uint16_t delay) {

    //Stop all started processes;
    process_stop_execution();

    //Enable all interrupts;
    core_enable_interrupts();

    //Reset the critical section counter;
    critical_section_counter = 0;

    //Disable the context switcher;
    core_set_context_switcher(&dumb);

    //TODO RESET IDLE INTERRUPT HANDLERS

    //TODO THREAD MODES;

    //Handle the error;
    arch_blink(delay);

}



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


//------------------------------------------- Config checking -------------------------------------------

/*
 * check_config : this function will call _ConfigChecker, and blink the led and display the error message
 *  if it fails;
 */

/*
 * TODO REBUILD

void kernel::check_config() {

    //Declare the error message;
    tstring error_message = "Error : ";

    //If the config is ok, return;
    if (_ConfigChecker::check_config(error_message)) {
        return;
    }

    const char *m = error_message.data();

    //If the config is not ok :

    //If a led is provided :
#ifdef HL_LED_FLAG
    //Enable the led output;
    pin_mode_output(LED_PIN);
#endif

    //Enable all transmission layers.

    //A macro to enable all interfaces;
#define EXTERNAL_CONTROL(c, p, s, transmission) transmission::begin();

    //Initialise every interface;
#include "Config/control_config.h"

    //Undef the macro for safety;
#undef EXTERNAL_CONTROL

    delay_ms(2000);

    //Infinite loop, as the code is badly configured;
    while(true) {

        //If a led is provided :
#ifdef HL_LED_FLAG
        //Blink the led;
        digital_write(LED_PIN, !digital_read(LED_PIN));
#endif

        //TODO CORRECT SEND
        //A macro to enable all interfaces;
#define EXTERNAL_CONTROL(c, p, s, transmission) transmission::send_str(m);

        //Initialise every interface;
#include "Config/control_config.h"

        //Undef the macro for safety;
#undef EXTERNAL_CONTROL

        //Wait
        delay_ms(1000);

    }

}

  */


//------------------------------------------- Initialisation -------------------------------------------


/*
 * initialise_hardware : this function is called once, by start only. It is the project initialisation function.
 *
 *  As its name suggests, it will initialise_hardware every module of the code.
 */

/*
 * TODO REBUILD
void kernel::initialise_hardware() {


    //Initialise all enabled interfaces
    Interaction::initialise_hardware();

    //Initialise the SteppersController module only if it is enabled
#ifdef ENABLE_STEPPER_CONTROL

    StepperController::initialise_hardware();

#endif


}
 */


/*
 * initialise_hardware : this function is called once, by start only. It is the project initialisation function.
 *
 *  As its name suggests, it will initialise_hardware every module of the code.
 */

/*
 * TODO REBUILD

void kernel::initialise_data() {

    //Clear the EEPROMMap profile;
    EEPROMMap::initialise_data();

    //Initialise all enabled interfaces
    Interaction::initialise_data();

    //Initialise the task scheduler;
    TaskScheduler::initialise_data();

    //Initalise the event manager;
    EventManager::reset();

    //Initialise the task programmer;
    TaskProgrammer::reset();


    //Initialise the SteppersController module only if it is enabled
#ifdef ENABLE_STEPPER_CONTROL

    StepperController::initialise_data();

#endif

    //Finally, solve the project;
    Project::initialise_data();

    //Lock the EEPROMMap tree, and solve data across the code if a relevant profile is found in the Storage;
    EEPROMMap::lock_tree();


}
  */

