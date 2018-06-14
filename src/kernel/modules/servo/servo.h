/*
  servo.h -  Part of TRACER

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

#ifndef CODE_SERVOACTIONS_H
#define CODE_SERVOACTIONS_H

#include "stdbool.h"

#include "stdint.h"

#include "stddef.h"

#include <kernel/drivers/timer.h>

#include <data_structures/containers/llist.h>

//--------------------------------------------------- Public structs ---------------------------------------------------

typedef struct {

    //Servo channels are stored in linked lists;
    linked_element_t link;

    //TODO PIN;

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

} servo_channel_t;


//----------------------------------------------------- Init - Exit ----------------------------------------------------

//Start the servo controller; Must provide a us based timer with an ovf interrupt capability;
void servo_module_init(timer_ovf_int_t *us_timer, float period_us);

//Stop the servo controller;
void servo_module_exit();


//-------------------------------------------------- Controller setup --------------------------------------------------

//Set the period of the servo controller;
void servo_module_set_period(float new_period);

//Add a channel to the servo controller;
servo_channel_t *servo_module_add_channel(float value_min, float value_max, uint16_t impulse_min, uint16_t impulse_max);

//Remove a channel from the servo controller;
void servo_module_remove_channel(servo_channel_t *channel);


//--------------------------------------------------- Channels setup ---------------------------------------------------

//Update a channel's value;
void servo_module_set_channel_value(servo_channel_t *channel, float value);

//Disable a channel;
void servo_module_disable_channel(servo_channel_t *channel);

//Disable all channels;
void servo_module_disable_all(servo_channel_t *channel);


#endif //CODE_SERVOACTIONS_H
