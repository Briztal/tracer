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




#include <kernel/hard/except.h>

#include <kernel/exec/sysclock.h>

#include <kernel/hard/arch/sclk.h>

#include <kernel/hard/arch/prmpt.h>


//---------------------------------------------------- Operations ----------------------------------------------------

/*
 * System clock operations. Any call with system timer not initialised generates a kernel panic;
 */

//The sysclock_milliseconds reference;
static volatile uint32_t systick_half_millis = 0;

//The current task's activity_time. At init, preemption disabled;
static volatile uint32_t task_duration = 0;



void sysclock_start() {
	
	//No preemption for instance;
	sysclock_set_process_duration(0);
	
	//Configure the system clock;
	__sclk_configure(2000, KERNEL_SYSTICK_PRIORITY);

	//Start the system clock;
	__sclk_start();

}


/*--------------------------------------------------- System tick ----------------------------------------------------*/

/*
 * systick : the system tick function;
 *
 *  It does the following operations :
 *      - increments the ms counter;
 *      - verifies that a context switch is required;
 *      - triggers it if so;
 */


void __krnl_tick() {

    //Increment the ms/2 counter;
    systick_half_millis++;

    //If the current task can pe preempted :
    if (task_duration) {

        //If it must be preempted :
        if (!--task_duration) {

            //Trigger the preemption;
			__prmpt_trigger();
	
			//Task activity_time becomes 0, preemption won't be called anymore;

        }

    }

}


/*------------------------------------------------ Process preemption ------------------------------------------------*/

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
	
	//Return the current time value;
    return systick_half_millis >> 1;
	
}


/*
 * Wait till time has reached the given limit;
 */

void systick_wait(uint16_t ms_delay) {
	
    //Determine the limit;
    volatile uint32_t limit = systick_half_millis + (((uint32_t) ms_delay) << 1);

    //Sleep till the limit;
    while ((volatile uint32_t) systick_half_millis < limit);

}
