/*
  servo.c -  Part of TRACER

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

#include "servo.h"

#include <kernel/kernel.h>
#include <kernel/drivers/timer.h>


typedef struct {

    //The microsecond based timer used by the controller;
    timer_ovf_int_t *us_timer;

    //The list of started channels;
    linked_list_t started_channels;

    //The list of stopped channels;
    linked_list_t stopped_channels;

    //The currently active channel;
    const servo_channel_t *active_channel;

    //The period of all servo channels;
    float period;

    //The free time after a cycle;
    float complement;

    //A flag set if the controller is initialiser;
    bool initialised;

    //A flag to stop the isr handler;
    bool started;

} servo_controller_t;


//------------------------------- Globals -------------------------------

//There exists only one servo controller, for interrupt;
static servo_controller_t servo_controller = {
        .us_timer = 0,
        .started_channels = EMPTY_LINKED_LIST(255),
        .stopped_channels = EMPTY_LINKED_LIST(255),
        .active_channel = 0,
        .period = 20000,
        .complement = 20000,
        .initialised = false,
        .started = false,
};


//------------------------------- Private headers -------------------------------

//Update the complement to all delays;
void servo_update_complement();

//The interrupt function;
void servo_isr_handler();

//TODO PINMODES


//------------------------------- Init - Exit -------------------------------

/*
 * servo_init : initialises the servo controller; Must provide a us based timer with an ovf interrupt capability;
 */

void servo_init(timer_ovf_int_t *us_timer) {

    //Cache the controller;
    servo_controller_t *controller = &servo_controller;

    //If the controller is already initialised :
    if (controller->initialised) {

        //Error;
        kernel_error("servo.c : servo_init : the servo controller is already initialised");

    }

    //Initialise the servo controller;
    *controller = (servo_controller_t) {
            .us_timer = us_timer,
            .started_channels = EMPTY_LINKED_LIST(255),
            .stopped_channels = EMPTY_LINKED_LIST(255),
            .active_channel = 0,
            .period = 20000,
            .complement = 20000,
            .initialised = true,
            .started = false,
    };

}


/*
 * servo_exit : de-initialises the servo controller;
 */

void servo_exit() {

    //Cache the controller;
    servo_controller_t *controller = &servo_controller;

    //If the controller is not initialised :
    if (!controller->initialised) {

        //Nothing to do;
        return;

    }

    //De initialise the timer;
    timer_ovf_int_exit(controller->us_timer);

    //Mark the controller de-initialised and stopped;
    controller->initialised = controller->started = false;

}


//------------------------------- Start - Stop -------------------------------

/*
 * start : this function starts the servo interrupt routine.
 */

void servo_start() {

    //Cache the servo controller pointer;
    servo_controller_t *controller = &servo_controller;

    //If the controller is already started, nothing to do;
    if (controller->started)
        return;

    //Mark the controller started;
    controller->started = true;

    //Reset the active channel;
    controller->active_channel = 0;

    //Cache the timer;
    timer_ovf_int_t *timer = controller->us_timer;

    //Set the interrupt function;
    timer_interrupt_set_handler(&timer->ovf_interrupt, servo_isr_handler);

    //Start the timer;
    timer_start_timer((const timer_base_t *) timer);

    //Start the interrupt routine;
    servo_isr_handler();

}


/*
 * stop : stop the interrupt routine;
 */

void servo_stop() {

    //Cache the servo controller pointer;
    servo_controller_t *controller = &servo_controller;

    //Access to started flag is critical;
    kernel_enter_critical_section();

    //Mark the controller started;
    controller->started = false;

    //Leave the critical section
    kernel_leave_critical_section();

    //The routine will be stopped and cleaned at next interrupt;

}


/*
 * servo_controller_set_channel_value : sets the
 */

void servo_controller_set_channel_value(servo_channel_t *channel, float value) {

    //Cache value bounds;
    float v_min = channel->value_min;
    float v_max = channel->value_max;

    /*Force the value to be inside the bounds;*/
    if (value < v_min) value = v_min;
    if (v_max < value) value = v_max;

    //Determine the new period in float;
    float imp_min = channel->impulse_min;
    float imp_max = channel->impulse_max;

    //Determine the impulse length;
    channel->impulse_duration = imp_min + ((imp_max - imp_min) * (value - v_min) / (v_max - v_min));

}


/*
 * update_complement : this function will determine the period complement,
 *  so that a complete servo cycle lasts exactly one period.
 */

void servo_update_complement() {

    //Cache the pointer to the servo controller;
    servo_controller_t *controller = &servo_controller;

    //Cache the value complement;
    float complement = controller->period;

    //Cache the number of active channels;
    size_t nb_channels = controller->started_channels.nb_elements;

    //Cache the first active channel;
    linked_element_t *channel = controller->started_channels.first;

    //For each channel :
    while (nb_channels--) {

        //Subtract the channel's value;
        complement -= ((servo_channel_t *) channel)->impulse_duration;

        //Update the next channel;
        channel = channel->next;

    }

    //Initialise the new complement to its default value;
    controller->complement = complement;

}


/*
 * interrupt_routine : this function handles the communication with servos;
 */

void servo_isr_handler() {

    //Cache the servo controller pointer;
    servo_controller_t *const controller = &servo_controller;

    //Cache the servo timer pointer;
    const timer_ovf_int_t *const timer = controller->us_timer;

    //Prevent the handler from being called again;;
    timer_interrupt_disable(&timer->ovf_interrupt);

    //Cache the previously set channel;
    const servo_channel_t *active_channel = controller->active_channel;

    //If the previous channel exists :
    if (active_channel) {

        //Disable the channel
        //TODO PIN DISABLE;

        //Update the active channel;
        active_channel = (servo_channel_t *) ((linked_element_t *) active_channel)->next;

    } else {

        //If there is no active channel (reload), the first element of the list becomes the first element;
        active_channel = (servo_channel_t *) controller->started_channels.first;

    }

    //If we must stop the routine :
    if (!controller->started) {

        //Reset the active channel
        controller->active_channel = 0;

        //Stop the timer;
        timer_stop_timer((const timer_base_t *) timer);

        //Complete, as the interrupt is disabled;
        return;

    }

    //If the newly active channel exists :
    if (active_channel) {

        //TODO PIN ENABLE;

        //Set the new period;
        timer_set_period((const timer_base_t *const) timer, active_channel->impulse_duration);

    } else {

        //Set the new period;
        timer_set_period((const timer_base_t *const) timer, controller->complement);

    }

    //Save the active channel in the servo controller;
    controller->active_channel = active_channel;

    //Enable the servo interrupt;
    timer_interrupt_enable(&timer->ovf_interrupt);

}
