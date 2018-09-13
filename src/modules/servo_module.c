/*
  servo_module.c -  Part of TRACER

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

#include <util/type.h>

struct servo_channel {

    //Servo channels are stored in linked lists;
    struct list_head head;

    //The minimal value of the chanel;
    float value_min;

    //The maximal value of the chanel;
    float value_max;

    //The minimal duration (in us) of an impulse on the channel;
    uint16_t impulse_min;

    //The maximal duration (in us) of an impulse on the channel;
    uint16_t impulse_max;

    //The current impulse duration;
    float impulse_duration;

};

//--------------------------------------------------- Private structs --------------------------------------------------

typedef struct {

    //The microsecond based timer used by the controller;
    timer_base_t *us_timer;

    //The list of started channels;
	servo_channel enabled_channels;

    //The list of unregistered channels;
    linked_list_t disabled_channels;

    //The currently active channel;
    const servo_channel_t *active_channel;

    //The period of all servo channels;
    float period;

    //The free time after a cycle;
    float complement;

    //A flag set if the controller is initialiser;
    volatile bool initialised;

    //A flag to stop the isr handler;
    volatile bool started;

} servo_controller_t;


//------------------------------------------------------- Globals ------------------------------------------------------

//There exists only one servo controller, for interrupt;
static servo_controller_t servo_controller = {
        .us_timer = 0,
        .enabled_channels = EMPTY_LINKED_LIST(255),
        .disabled_channels = EMPTY_LINKED_LIST(255),
        .active_channel = 0,
        .period = 20000,
        .complement = 20000,
        .initialised = false,
        .started = false,
};


//--------------------------------------------------- Private headers --------------------------------------------------

//Check that a cycle can happen correctly;
void servo_check_cycle(const servo_controller_t *);

//Count the max duration of a channel list;
float servo_check_duration(const linked_list_t *channels);


//Enable a channel providing an impulse value;
void servo_enable_channel(servo_controller_t *controller, servo_channel_t *channel, float impulse_duration);

//Enable a channel providing an impulse value;
void servo_disable_channel(servo_controller_t *, servo_channel_t *channel);


//Update the complement to all delays;
void servo_update_complement(servo_controller_t *);


//Start the timer routine;
void servo_service_start(servo_controller_t *);

//Stop the timer routine;
void servo_service_stop(servo_controller_t *);

//The interrupt function;
void servo_isr_handler();


//----------------------------------------------------- Init - Exit ----------------------------------------------------

/*
 * servo_module_init : initialises the servo controller; Must provide a us based timer with an ovf interrupt capability;
 */

void servo_module_init(timer_base_t *const us_timer, const float period_us) {

    //Cache the controller;
    servo_controller_t *controller = &servo_controller;

    //If the controller is already initialised :
    if (controller->initialised) {

        //Error;
        kernel_error("servo.c : servo_module_init : the servo controller is already initialised;");

    }

    //Initialise the servo controller;
    *controller = (servo_controller_t) {
            .us_timer = us_timer,
            .enabled_channels = EMPTY_LINKED_LIST(255),
            .disabled_channels = EMPTY_LINKED_LIST(255),
            .active_channel = 0,
            .period = period_us,
            .complement = period_us,
            .initialised = true,
            .started = false,
    };

}


/*
 * servo_module_exit : de-initialises the servo controller;
 */

void servo_module_exit() {

    //Cache the controller;
    servo_controller_t *controller = &servo_controller;

    //If the controller is not initialised :
    if (!controller->initialised) {

        //Nothing to do;
        return;

    }

    //De initialise the timer;
    timer_exit(controller->us_timer);

    //Mark the controller de-initialised and unregistered;
    controller->initialised = controller->started = false;

}



//-------------------------------------------------- Controller setup --------------------------------------------------

/*
 * servo_module_set_period : updates the period of the servo controller;
 */

void servo_module_set_period(const float new_period) {

    //Cache the servo controller pointer;
    servo_controller_t *controller = &servo_controller;

    //If the controller is already initialised :
    if (controller->initialised) {

        //Error;
        kernel_error("servo.c : servo_module_set_period : the servo controller is not initialised;");

    }

    //If the controller is started :
    if (controller->started) {

        //Error;
        kernel_error("servo.c : servo_module_set_period : the servo controller is currently started;");


    }

    //Update the period;
    controller->period = new_period;

    //Check that complete cycle can happen correctly;
    servo_check_cycle(controller);

    //Update the complement;
    servo_update_complement(controller);


}


/*
 * servo_check_cycle : verifies that all channels can be active at full time in the required period;
 */

void servo_check_cycle(const servo_controller_t *const controller) {

    //Determine the total period;
    float period =
            servo_check_duration(&controller->enabled_channels) + servo_check_duration(&controller->disabled_channels);

    //If the determined period exceeds (or equals) the required period :
    if (period >= controller->period) {

        //Error. No time for complement, or complement negative;
        kernel_error("servo.c : servo_check_cycle : the sum of all channels exceed the required period");

    }

}


/*
 * servo_check_duration : counts the maximal duration of the channels list;
 */

float servo_check_duration(const linked_list_t *const channels) {

    //Initialise the duration sum;
    float duration = 0;

    //Cache the first channel;
    const linked_element_t *channel = channels->first;

    //Cache the number of channels in the list;
    size_t nb_channels = channels->nb_elements;

    //For each channel :
    while (nb_channels--) {

        //Sum the duration;
        duration += ((servo_channel_t *) channel)->impulse_duration;

        //Update the channel;
        channel = channel->next;

    }

	//Return the total duration;
	return duration;

}

/* 
 * servo_module_add_channel : adds a channel to the servo controller; The pin must have been properly initialised;
 */

servo_channel_t *servo_module_add_channel(PORT_pin_t *initialised_pin, const float value_min, const float value_max,
                         const uint16_t impulse_min, const uint16_t impulse_max) {

    //Cache the servo controller pointer;
    servo_controller_t *controller = &servo_controller;

    //If the controller is already initialised :
    if (controller->initialised) {

        //Error;
        kernel_error("servo.c : servo_module_add_channel : the servo controller is not initialised;");

    }

    //If the controller is started :
    if (controller->started) {

        //Error;
        kernel_error("servo.c : servo_module_add_channel : the servo controller is currently started;");

    }

    //If values are incorrect
    if (value_min >= value_max) {

        //Error;
        kernel_error("servo.c : servo_module_add_channel : value_min is greater or equal than value_max;");

    }

    //If values are incorrect
    if (impulse_min >= impulse_max) {

        //Error;
        kernel_error("servo.c : servo_module_add_channel : impulse_min is greater or equal than impulse_max;");

    }

    //Create registers structs;
    GPIO_output_registers_t GPIO_regs;

    //Get GPIO output registers for provided port;
    PORT_get_GPIO_output_registers(initialised_pin->port_data, &GPIO_regs);

    //Create the bitmask for C5;
    GPIO_mask_t mask = GPIO_get_mask(initialised_pin);

    //Create the servo channel initializer;
    servo_channel_t init = {
            .link = EMPTY_LINKED_ELEMENT(),
            .pin_set_register = GPIO_regs.data_register,
            .pin_clear_register = GPIO_regs.data_register,
            .pin_mask = mask,
            .value_min = value_min,
            .value_max = value_max,
            .impulse_min = impulse_min,
            .impulse_max = impulse_max,
            .impulse_duration = 0,//Stopped -> duration of 0;
    };

    //Create the servo channel in the kernel heap;
    servo_channel_t *channel = kernel_malloc_copy(sizeof(servo_channel_t), &init);

    //Add the channel at the end of the unregistered list;
    llist_insert_last(&controller->disabled_channels, (linked_element_t *) channel);

	//Return the channel;
	return channel;
}


/*
 * servo_module_remove_channel : removes a channel from the servo controller;
 */

void servo_module_remove_channel(servo_channel_t *const channel) {

    //Cache the servo controller pointer;
    servo_controller_t *controller = &servo_controller;

    //If the controller is already initialised :
    if (controller->initialised) {

        //Error;
        kernel_error("servo.c : servo_module_add_channel : the servo controller is not initialised;");

    }

    //If the controller is started :
    if (controller->started) {

        //Error;
        kernel_error("servo.c : servo_module_add_channel : the servo controller is currently started;");

    }

    //If the channel is in the started list (impulse not null) :
    if (channel->impulse_duration) {

        //Remove the channel from the enabled list;
        llist_remove_element(&controller->enabled_channels, (linked_element_t *) channel);

    } else {

        //Remove the channel from the disabled list;
        llist_remove_element(&controller->disabled_channels, (linked_element_t *) channel);

    }

    //Finally, delete the channel;
    kernel_free(channel);

}



//--------------------------------------------------- Channels setup ---------------------------------------------------

/*
 * servo_module_set_channel_value : Determines the impulse duration, and stops or starts the channel.
 *  Public function, critical checks are made;
 */

void servo_module_set_channel_value(servo_channel_t *const channel, float value) {

    //Cache the controller;
    servo_controller_t *controller = &servo_controller;

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
    float duration = imp_min + ((imp_max - imp_min) * (value - v_min) / (v_max - v_min));


    //Access to channels is servo-critical;

    //Cache the timer;
    timer_base_t *timer = controller->us_timer;

    //If the module is enabled :
    if (controller->started) {

        //Enter the servo-critical section
        timer_interrupt_disable(&timer->reload_interrupt);

    }

    //Cache the current state;
    bool enabled = channel->impulse_duration != 0;

    //If the duration is not null, the channel must be enabled;
    if (duration) {

        //If the channel is disabled :
        if (!enabled) {

            //Enable the channel;
            servo_enable_channel(controller, channel, duration);

        } else {

            //If the channel is already enabled, simply update its duration;
            channel->impulse_duration = duration;

        }

    } else {

        //If the channel is enabled :
        if (enabled) {

            //Stop the channel;
            servo_disable_channel(controller, channel);

        }

        //If not enabled, nothing to do;

    }

    //Update the complement;
    servo_update_complement(controller);

    //If the module is enabled :
    if (controller->started) {

        //Leave the servo-critical section
        timer_interrupt_disable(&timer->reload_interrupt);

    }

}


/*
 * servo_module_disable_channel : disabled a channel. Public method, critical check is made;
 */

void servo_module_disable_channel(servo_channel_t *const channel) {

    //If the channel is already disabled, nothing to do;
    if (!channel->impulse_duration)
        return;

    //Cache the controller;
    servo_controller_t *controller = &servo_controller;

    //Access to channels is servo-critical;

    //Cache the timer;
    timer_base_t *timer = controller->us_timer;

    //If the module is enabled :
    if (controller->started) {

        //Enter the servo-critical section
        timer_interrupt_disable(&timer->reload_interrupt);

    }

    //Disable the channel;
    servo_disable_channel(controller, channel);

    //Update the complement;
    servo_update_complement(controller);

    //If the module is enabled :
    if (controller->started) {

        //Leave the servo-critical section
        timer_interrupt_disable(&timer->reload_interrupt);

    }

}


/*
 * servo_enable_channel : enables a channel. Private method, no critical check is made;
 */

void servo_enable_channel(servo_controller_t *const controller, servo_channel_t *const channel,
                          const float impulse_duration) {

    //Update the channel's duration;
    channel->impulse_duration = impulse_duration;

    //Remove the channel from the disabled list;
    llist_remove_element(&controller->disabled_channels, (linked_element_t *) channel);

    //Insert the element in the enabled list;
    llist_insert_last(&controller->enabled_channels, (linked_element_t *) channel);

    //If the service is not started :
    if (!controller->started) {

        //Start it;
        servo_service_start(controller);

    }

}


/*
 * servo_disable_channel : disabled a channel. Private method, no critical check is made;
 *
 *  If no more channels are enabled, stop the module;
 */

void servo_disable_channel(servo_controller_t *controller, servo_channel_t *channel) {

    //Reset the channel's duration;
    channel->impulse_duration = 0;

    //If the channel is active :
    if (channel == controller->active_channel) {

        //Disable its line;
        GPIO_clear_bits(channel->pin_clear_register, channel->pin_mask);

        //Update the channel;
        controller->active_channel = (const servo_channel_t *) ((linked_element_t *) channel)->prev;

    }

    //Remove the channel from the disabled list;
    llist_remove_element(&controller->disabled_channels, (linked_element_t *) channel);

    //Insert the element in the enabled list;
    llist_insert_last(&controller->enabled_channels, (linked_element_t *) channel);

    //If there are no more active channels and the service is running:
    if ((!controller->enabled_channels.nb_elements) && (controller->started)) {

        //Stop it;
        servo_service_stop(controller);


    }

}


/*
 * update_complement : this function will determine the period complement,
 *  so that a complete servo cycle lasts exactly one period.
 */

void servo_update_complement(servo_controller_t *const controller) {

    //Cache the value complement;
    float complement = controller->period;

    //Cache the number of active channels;
    size_t nb_channels = controller->enabled_channels.nb_elements;

    //Cache the first active channel;
    linked_element_t *channel = controller->enabled_channels.first;

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


//---------------------------------------------------- Start - Stop ----------------------------------------------------

/*
 * servo_service_start : this function starts the servo interrupt routine.
 */

void servo_service_start(servo_controller_t *const controller) {

    //Mark the controller started;
    controller->started = true;

    //Reset the active channel;
    controller->active_channel = 0;

    //Cache the timer;
    timer_base_t *timer = controller->us_timer;

    //Set the interrupt function;
    timer_interrupt_set_handler(&timer->reload_interrupt, servo_isr_handler);

    //Start the timer;
    timer_start_timer((const timer_base_t *) timer);

    //Start the interrupt routine;
    servo_isr_handler();

}


/*
 * servo_service_stop : stop the interrupt routine;
 */

void servo_service_stop(servo_controller_t *const controller) {

    //Access to started flag is critical;
    kernel_enter_critical_section();

    //Mark the controller started;
    controller->started = false;

    //Leave the critical section
    kernel_leave_critical_section();

    //The routine will be unregistered and cleaned at next interrupt;

}


//------------------------------------------------- Interrupt handler --------------------------------------------------


/*
 * interrupt_routine : this function handles the interaction with servos;
 */

void servo_isr_handler() {

    //Cache the servo controller pointer;
    servo_controller_t *const controller = &servo_controller;

    //Cache the servo timer pointer;
    const timer_base_t *const timer = controller->us_timer;

    //Prevent the handler from being called again;;
    timer_interrupt_disable(&timer->reload_interrupt);

    //Cache the previously set channel;
    const servo_channel_t *active_channel = controller->active_channel;

    //If the previous channel exists :
    if (active_channel) {

        //Disable the channel
        GPIO_clear_bits(active_channel->pin_clear_register, active_channel->pin_mask);

        //Update the active channel;
        active_channel = (servo_channel_t *) ((linked_element_t *) active_channel)->next;

    } else {

        //If there is no active channel (reload), the first element of the list becomes the first element;
        active_channel = (servo_channel_t *) controller->enabled_channels.first;

    }

    //If we must stop the routine :
    if (!controller->started) {

        //Reset the active channel
        controller->active_channel = 0;

        //Stop the timer;
        timer_stop_timer(timer);

        //Complete, as the interrupt is disabled;
        return;

    }

    //If the newly active channel exists :
    if (active_channel) {

        //Disable the channel
        GPIO_set_bits(active_channel->pin_set_register, active_channel->pin_mask);

        //Set the new period;
        timer_set_period(timer, active_channel->impulse_duration);

    } else {

        //Set the new period;
        timer_set_period(timer, controller->complement);

    }

    //Save the active channel in the servo controller;
    controller->active_channel = active_channel;

    //Enable the servo interrupt;
    timer_interrupt_enable(&timer->reload_interrupt);

}
