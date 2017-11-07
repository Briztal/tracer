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

void ContinuousActions::begin() {
    //INIT : PWM à 0;
#define CONTINUOUS(i, name, pin, max) \
    analog_write(pin, 0);

#include <config.h>

#undef CONTINUOUS
}

#define CONTINUOUS(i, name, pin, max) \
void ContinuousActions::set_power_##i (float f) {\
    uint8_t power = f * ((float) 255.0 / (float) max);\
    if (power<=0){\
        stop_##i();\
        return;\
    }\
    states[i] = true;\
    if (power<=255){analog_write(pin, power);}\
    else{analog_write(pin, 255);}\
}\
bool ContinuousActions::get_state_##i () {\
    return states[i];\
}\
void ContinuousActions::stop_##i() {\
    analog_write(pin,0);\
    states[i] = false;\
}


#include <config.h>

#undef CONTINUOUS




void ContinuousActions::set_power(uint8_t action, float power) {

#define CONTINUOUS(i, ...) case i: set_power_##i(power);break;

    switch (action) {

#include <config.h>

#undef CONTINUOUS

        default:
            break;
    }

}

bool ContinuousActions::get_state(uint8_t action) {

    if (action >= NB_CONTINUOUS)
        return 0;

    return states[action];

}

void ContinuousActions::stop(uint8_t action) {

#define CONTINUOUS(i, ...) case i: stop_##i();break;

    switch (action) {

#include <config.h>

#undef CONTINUOUS

        default:
            break;
    }
}


bool t_b_cont[NB_CONTINUOUS];
bool *const ContinuousActions::states = t_b_cont;