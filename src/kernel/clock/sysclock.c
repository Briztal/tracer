/*
  sysclock.c Part of TRACER

  Copyright (c) 2018 Raphaël Outhier

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

#include "sysclock.h"

#include <kernel/panic.h>

#include <kernel/log.h>

#include <kernel/async/preempt.h>

#include <kernel/async/interrupt.h>


//---------------------------------------------------- System timer ----------------------------------------------------

//The system timer. Initialised to 0;
static struct sys_timer *system_timer = 0;


/**
 * sysclock_init_timer : initialises the system timer. If already initialised, triggers a kernel panic;
 * @param timer : the new system timer;
 */

void sysclock_init(struct sys_timer *const timer) {
	
	//If the system time is already initialised :
	if (system_timer) {
		
		//Kernel panic. System timer can't be initialised twice;
		kernel_panic("sysclock_init : attempted to initialise the timer twice;");
		
	}
	
	//Initialise the system timer;
	system_timer = timer;
	
}


/**
 * sysclock_initialised :
 * @return return true if the system timer is not null;
 */

bool sysclock_initialised() {
	
	//Return true if the system timer is not null;
	return (bool) system_timer;
	
}

//---------------------------------------------------- Operations ----------------------------------------------------

/*
 * System clock operations. Any call with system timer not initialised generates a kernel panic;
 */

//The sysclock_milliseconds reference;
volatile uint32_t systick_half_millis = 0;

//The current task's activity_time. At init, preemption disabled;
volatile uint32_t task_duration = 0;



void sysclock_start() {
	
	//Cache the system timer;
	struct sys_timer *const timer = system_timer;
	
	//If the system timer is null :
	if (!timer) {
		
		//Panic, can't use the sysclock without system timer;
		kernel_panic("sysclock_start : system timer uninitialised;");
		
	}
	
	
	//No preemption for instance;
	sysclock_set_process_duration(0);
	

	//2KHz systick, 2 per millisecond, to be 1ms accurate;
	(*timer->int_set_frequency)(2000);

	//Systick has its own priority;
	(*timer->int_set_priority)(KERNEL_SYSTICK_PRIORITY);

	//Set the core timer to trigger systick function;
	(*timer->int_set_handler)(&sysclock_tick);

	//Enable the interrupt;
	(*timer->int_enable)();

	//Start the core timer;
	(*timer->start)();

	//Log;
	kernel_log_("system clock started");

}
//---------------------- System tick ----------------------

/*
 * systick : the system tick function;
 *
 *  It does the following operations :
 *      - increments the ms counter;
 *      - verifies that a context switch is required;
 *      - triggers it if so;
 */


void sysclock_tick() {

    //Increment the ms/2 counter;
    systick_half_millis++;

    //If the current task can pe preempted :
    if (task_duration) {

        //If it must be preempted :
        if (!--task_duration) {

            //Trigger the preemption;
			preemption_set_pending();

            //Task activity_time becomes 0, preemption won't be called anymore;

        }

    }

}


//---------------------- Process preemption ----------------------

/*
 * setTaskDuration : sets the current task's activity_time :
 *  - 0 : infinite;
 *  - x (!= 0) : activity_time of x sysclock_milliseconds;
 */

void sysclock_set_process_duration(uint16_t ms) {

    //Update the task's activity_time to the double of the provided activity_time;
    task_duration = ((uint32_t) ms) << 1;

}


//---------------------- Time reference ----------------------

/*
 * milliseconds : returns the sysclock_milliseconds reference. It is obtained by dividing the ms/2 reference by 2;
 */

uint32_t sysclock_milliseconds() {
	
	//If the system timer is null :
	if (!system_timer) {
		
		//Panic, can't use the sysclock without system timer;
		kernel_panic("sysclock_milliseconds : system timer uninitialised;");
		
	}
	
	//Return the current time value;
    return systick_half_millis >> 1;
}


/*
 * Wait till time has reached the given limit;
 */

void systick_wait(uint16_t ms_delay) {
	
	//If the system timer is null :
	if (!system_timer) {
		
		//Panic, can't use the sysclock without system timer;
		kernel_panic("sysclock_milliseconds : system timer uninitialised;");
		
	}

    //Determine the limit;
    volatile uint32_t limit = systick_half_millis + (((uint32_t) ms_delay) << 1);

    //Sleep till the limit;
    while ((volatile uint32_t) systick_half_millis < limit);

}
