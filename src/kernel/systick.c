/*
  systick.c Part of TRACER

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

#include <kernel/scheduler/sprocess.h>

#include "systick.h"

//---------------------- System tick ----------------------


//The systick_milliseconds reference;
volatile uint32_t systick_millis = 0;

//The current task's duration. At init, preemption disabled;
volatile uint16_t task_duration = 0;


//---------------------- System tick ----------------------

/*
 * systick : the system tick function;
 *
 *  It does the following operations :
 *      - increments the ms counter;
 *      - verifies that a context switch is required;
 *      - triggers it if so;
 */


void systick_tick() {

    if (!(systick_millis % 1000)) {
        //Serial.println("SUUS "+String(st_millis));
    }

    //Increment the ms counter;
    systick_millis++;

    //If the current task can pe preempted :
    if (task_duration) {

        //If it must be preempted :
        if (!--task_duration) {

            //Trigger the preemption;
            core_preempt_process();

            //Task duration becomes 0, preemption won't be called anymore;

        }

    }

}


//---------------------- Context Switch ----------------------


/*
 * setTaskDuration : sets the current task's duration :
 *  - 0 : infinite;
 *  - x (!= 0) : duration of x systick_milliseconds;
 */

void systick_set_process_duration(uint16_t ms) {

    //Update the task's duration;
    task_duration = ms;

}


//---------------------- Time reference ----------------------

/*
 * milliseconds : returns a copy of the systick_milliseconds reference;
 */

uint32_t systick_milliseconds() {
    return systick_millis;
}


/*
 * Wait till time has reached the given limit;
 */

void systick_wait(uint32_t delta_t) {

    //Determine the limit;
    volatile uint32_t limit = systick_millis + delta_t;

    //Sleep till the limit;
    while ((volatile uint32_t) systick_millis < limit) {
    };


}
