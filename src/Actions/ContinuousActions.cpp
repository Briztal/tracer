/*
  ContinuousActions.cpp - Part of TRACER

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

#include "ContinuousActions.h"
#include <hardware_language_abstraction.h>
#include <EEPROM/EEPROMStorage.h>
#include <Interfaces/Interfaces.h>


/*
 * initialise_data : this function initialises all PWMs
 */

void ContinuousActions::init() {

    //Disable every PWM.

#define CONTINUOUS(i, name, pin, max) \
    analog_write(pin, 0);

#include <config.h>

#undef CONTINUOUS

}


/*
 * set_power : this function sets a particular action's power
 *
 */

void ContinuousActions::set_power(uint8_t action, float power) {

    //Verify the the action exists
    if (action >= NB_CONTINUOUS)
        return;

    //Switch the given action and call the appropriate function.

#define CONTINUOUS(i, ...) case (uint8_t) i:set_power_##i(power);break;

    switch (action) {

#include <config.h>

#undef CONTINUOUS

        default:
            break;
    }

}


/*
 * get_state : returns a particular action's state
 *
 */

bool ContinuousActions::get_state(uint8_t action) {

    //Verify the the action exists
    if (action >= NB_CONTINUOUS)
        return 0;

    return states[action];

}


/*
 * stop : this function stops a particular action.
 *
 */
void ContinuousActions::stop(uint8_t action) {

    //Verify the the action exists
    if (action >= NB_CONTINUOUS)
        return;

    //Switch the given action and call the appropriate function.

#define CONTINUOUS(i, ...) case (uint8_t) i: stop_##i();break;

    switch (action) {

#include <config.h>

#undef CONTINUOUS

        default:
            break;
    }
}




#define CONTINUOUS(i, name, pin, ...) \
void ContinuousActions::set_power_##i (float power) {\
    /*Get the maximum value.*/\
    float max = EEPROMStorage::continuous_data[i].max;\
    \
    /*Get the analog value for the power.*/\
    uint32_t analog_value = (uint32_t) (power * ((float) 255.0 / max));\
    \
    /*If the value is zero, stop properly.*/\
    if (analog_value<=0){\
        stop_##i();\
        return;\
    }\
    \
    /*Update the state*/\
    states[i] = true;\
    /*Major the value and write*/\
    if (analog_value<=255){analog_write(pin, analog_value);}\
    else{analog_write(pin, 255);}\
}\
\
\
bool ContinuousActions::get_state_##i () {\
    return states[i];\
}\
\
\
void ContinuousActions::stop_##i() {\
    CI::echo("STOPPING");\
    analog_write(pin,0);\
    states[i] = false;\
}


#include <config.h>

#undef CONTINUOUS




bool t_b_cont[NB_CONTINUOUS];
bool *const ContinuousActions::states = t_b_cont;