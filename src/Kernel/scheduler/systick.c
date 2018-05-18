//
// Created by root on 4/10/18.
//

#include <Kernel/scheduler/process.h>

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
            core_trigger_context_switch();

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

void setTaskDuration(uint16_t ms) {

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

void sleep(uint32_t delta_t) {

    //Determine the limit;
    volatile uint32_t limit = systick_millis + delta_t;

    //Sleep till the limit;
    while ((volatile uint32_t) systick_millis < limit) {
    };


}


/*
 * msResetRequired : this function will return true if the st_millis reference has to be reset;
 */

bool msResetRequired() {

    //If the systick_milliseconds counter has reached the half of its maximal value, it's time to reset;
    return (systick_millis > (uint32_t) -1 >> 1);

}


/*
 * msReset : resets the time reference to 0;
 */
void msReset() {

    //Reset the st_millis counter;
    systick_millis = 0;

}
