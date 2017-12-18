/*
  PWMGPIO.cpp - Part of TRACER

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

#include "PWMGPIO.h"
#include <hardware_language_abstraction.h>
#include <EEPROM/EEPROMStorage.h>
#include <Control/Control.h>


//------------------------------------------- Init -------------------------------------------

/*
 * initialise_hardware : this function sets Actuators mode to output for every pwm.
 */

void PWMGPIO::initialise_hardware() {

    //Disable every PWM.

#define CONTINUOUS(i, name, pin, max) \
    pin_mode_output(pin);

#include <Config/actions_config.h>

#undef CONTINUOUS

}


/*
 * initialise_hardware : this function initialises all PWMs.
 */

void PWMGPIO::initialise_data() {

    //Disable every PWM.

#define CONTINUOUS(i, name, pin, max) \
    analog_write(pin, 0);

#include <Config/actions_config.h>

#undef CONTINUOUS

}


//------------------------------------------- Setup by id -------------------------------------------

/*
 * set_power : this function sets a particular action's power.
 */

void PWMGPIO::set_power(uint8_t action, float power) {

    //Verify the the action exists
    if (action >= NB_CONTINUOUS)
        return;

    //Switch the given action and call the appropriate function.

#define CONTINUOUS(i, ...) case (uint8_t) i:set_power_##i(power);break;

    switch (action) {

#include <Config/actions_config.h>

#undef CONTINUOUS

        default:
            break;
    }

}


/*
 * get_state : returns a particular action's state.
 */

bool PWMGPIO::get_state(uint8_t action) {

    //Verify the the action exists;
    if (action >= NB_CONTINUOUS) {

        //If the action doesn't exist, disables by default;
        return false;

    }

    //Return the appropriate state;
    return states[action];

}


/*
 * stop : this function stops a particular action.
 */

void PWMGPIO::stop(uint8_t action) {

    //Verify the the action exists
    if (action >= NB_CONTINUOUS)
        return;

    //Switch the given action and call the appropriate function.

#define CONTINUOUS(i, ...) case (uint8_t) i: stop_##i();break;

    switch (action) {

#include <Config/actions_config.h>

#undef CONTINUOUS

        default:
            break;
    }
}


//------------------------------------------- Setup by action -------------------------------------------

#define CONTINUOUS(i, name, pin, ...) \
void PWMGPIO::set_power_##i (float power) {\
    /*Get the maximum value;*/\
    float max = EEPROMStorage::continuous_data[i].max;\
    \
    /*Get the analog value for the power;*/\
    uint32_t analog_value = (uint32_t) (power * ((float) 255.0 / max));\
    \
    /*If the value is zero, stop properly;*/\
    if (analog_value<=0){\
        stop_##i();\
        return;\
    }\
    \
    /*Update the state;*/\
    states[i] = true;\
    /*Major the value and write;*/\
    if (analog_value<=255){analog_write(pin, analog_value);}\
    else{analog_write(pin, 255);}\
}\
\
/*Get the state of a particular action (by id);*/\
bool PWMGPIO::get_state_##i () {\
    return states[i];\
}\
\
/*Stop a particular action, by id;*/\
void PWMGPIO::stop_##i() {\
    debug("STOPPING");\
    analog_write(pin,0);\
    states[i] = false;\
}


#include <Config/actions_config.h>

#undef CONTINUOUS


//------------------------------------------- States -------------------------------------------

bool t_b_cont[NB_CONTINUOUS];
bool *const PWMGPIO::states = t_b_cont;