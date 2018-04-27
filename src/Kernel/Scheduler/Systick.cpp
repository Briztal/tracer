//
// Created by root on 4/10/18.
//

#include "Arduino.h"

#include "Systick.h"

#include "usb_dev.h"

#include "Kernel/Arch/Processors/core_arm_cortex_m4f.h"

//---------------------- System tick ----------------------

namespace Systick {

    //The milliseconds reference;
    volatile uint32_t st_millis = 0;

    //The current task's duration. At init, preemption disabled;
    volatile uint16_t task_duration = 0;

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

void Systick::systick() {

    if (!(st_millis%1000)) {
        //Serial.println("SUUS "+String(st_millis));
    }

    //Increment the ms counter;
    st_millis++;

    //If the current task can pe preempted :
    if (task_duration) {

        //If it must be preempted :
        if (!--task_duration) {

            //Trigger the preemption;
            core_trigger_context_switch();

            //Task duration becomes 0, won't be preempted anymore;

        }

    }

}


//---------------------- Context Switch ----------------------


/*
 * setTaskDuration : sets the current task's duration :
 *  - 0 : infinite;
 *  - x (!= 0) : duration of x milliseconds;
 */

void Systick::setTaskDuration(uint16_t ms) {

    //Update the task's duration;
    task_duration = ms;

}


//---------------------- Time reference ----------------------

/*
 * milliseconds : returns a copy of the milliseconds reference;
 */

uint32_t Systick::milliseconds() {
    return st_millis;
}



/*
 * Wait till time has reached the given limit;
 */

void Systick::delay(uint32_t delta_t) {

    //Determine the limit;
    uint32_t limit = st_millis + delta_t;

    //Serial.println("DELAYING");

    //Sleep till the limit;
    while((volatile uint32_t) Systick::st_millis < limit) {
        //Serial.println(String((volatile uint32_t) Systick::st_millis) + " "+String(limit));
    };

}


/*
 * msResetRequired : this function will return true if the st_millis reference has to be reset;
 */

bool Systick::msResetRequired() {

    //If the milliseconds counter has reached the half of its maximal value, it's time to reset;
    return (st_millis > (uint32_t) -1 >> 1);

}


/*
 * msReset : resets the time reference to 0;
 */
void Systick::msReset() {

    //Reset the st_millis counter;
    st_millis = 0;

}
