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


typedef struct {

    //TODO TIMER DATA;

    //The list of started channels;
    linked_list_t started_channels;

    //The list of stopped channels;
    linked_list_t stopped_channels;

    //The currently active channel;
    servo_channel_t *active_channel;

    //The period of all servo channels;
    uint16_t period;

    //The free time after a cycle;
    uint16_t complement;

    //A flag to stop the isr handler;
    bool started;

} servo_controller_t;


//------------------------------- Globals -------------------------------


//There exists only one servo controller, for interrupt;
static servo_controller_t servo_controller;


//------------------------------- Private headers -------------------------------

//Update the complement to all delays;
void servo_update_complement();

//The interrupt function;
void servo_isr_handler();

//TODO PINMODES


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

    //Set the interrupt function;
    set_servo_int_function(interrupt_routine);

    //Enable the servo timer;
    enable_servo_timer();

    //Start the interrupt routine;
    servo_isr_handler();

}


/*
 * stop : stop the interrupt routine;
 */

void servo_stop() {

    //Cache the servo controller pointer;
    servo_controller_t *controller = &servo_controller;

    //Reset the active channel;
    controller->active_channel = 0;

    //Access to started flag is critical;
    kernel_enter_critical_section();

    //Mark the controller started;
    controller->started = false;

    //Leave the critical section
    kernel_leave_critical_section();

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
    float impulse_length = imp_min + ((imp_max - imp_min) * (value - v_min) / (v_max - v_min));

    //Write an impulse
    channel->impulse_duration = timer_get_reload_value(impulse_length);

    //Redetermine the complement value;
    servo_update_complement();

}


/*
 * update_complement : this function will determine the period complement,
 *  so that a complete servo cycle lasts exactly one period.
 */

void servo_update_complement() {

    //Cache the pointer to the servo controller;
    servo_controller_t *controller = &servo_controller;

    //Cache the value complement;
    float reload = controller->period;

    //Cache the number of active channels;
    size_t nb_channels = controller->started_channels.nb_elements;

    //Cache the first active channel;
    linked_element_t *channel = controller->started_channels.first;

    //For each channel :
    while (nb_channels--) {

        //Subtract the channel's value;
        reload -= ((servo_channel_t *) channel)->value;

        //Update the next channel;
        channel = channel->next;

    }

    //Initialise the new complement to its default value;
    controller->complement = servo_period_to_reload(reload);

}


/*
 * interrupt_routine : this function handles the communication with servos;
 */

void servo_isr_handler() {


    //Prevent the handler from being called again;;
    disable_servo_interrupt();

    //Cache the servo controller pointer;
    servo_controller_t *controller = &servo_controller;

    //Cache the previously set channel;
    servo_channel_t *active_channel = controller->active_channel;

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

    //If the newly active channel exists :
    if (active_channel) {

        //TODO PIN ENABLE;

        //Set the new period;
        set_servo_int_reload(active_channel->impulse_duration);

    } else {

        //Set the new period;
        set_servo_int_reload(controller->complement);

    }

    //Save the active channel in the servo controller;
    controller->active_channel = active_channel;

}
