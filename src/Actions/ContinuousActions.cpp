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
#include "../hardware_language_abstraction.h"
#include "../Interfaces/CommandInterface.h"

void ContinuousActions::begin() {
    //INIT : PWM à 0;
#define CONTINUOUS(i, name, pin, max) \
    analog_write(pin, 0);

#include "../config.h"

#undef CONTINUOUS
}

//TODO CHECK DES VALEURS MAX
//TODO PASSER PAR UN CONST FLOAT ET VOIR SI CA ACCELERE

bool ContinuousActions::linear_modes_enabled() {
    return linear_modes != 0;
}

#define POW_2(i) ((sig_t)(1<<i))


#define CONTINUOUS(i, name, pin, max) \
void ContinuousActions::setPower##i (float f) {\
    linear_modes &= (!POW_2(i));\
    if (f<=0){\
        disable##i();\
        return;\
    }\
    enabled_actions|=POW_2(i);\
    if (f<=max)\
        analog_write(pin,  f * (255.0 / (float)max));\
    else \
        analog_write(pin, 255);\
}\
void ContinuousActions::setLinearPower##i (float f) {\
    if (!(enabled_actions&POW_2(i))){\
        linearPower##i = f;\
        linear_modes |= POW_2(i);\
    }\
}\
void ContinuousActions::set_power_for_speed_##i (float speed) {\
    if (speed<=0){\
        disable##i();\
        return;\
    }\
    float power = speed*linearPower##i;\
    enabled_actions|=POW_2(i);\
    if (power<=max) {\
        analog_write(pin,  power * (255.0 / (float)max));\
    } else \
        analog_write(pin,  255);\
}\
void ContinuousActions::disable##i() {\
    enabled_actions&=(!POW_2(i));\
    analogWrite(pin, 0);\
}\
float ContinuousActions::linearPower##i = 0;\

#include "../config.h"

#undef CONTINUOUS

uint8_t ContinuousActions::getSetFunctions(void (**f)(float)) {
    uint8_t id = 0;
#define CONTINUOUS(i, name, pin, max) \
    if (linear_modes&POW_2(i)) {\
        f[id] = set_power_for_speed_##i;\
        id++;\
    }
#include "../config.h"
#undef CONTINUOUS
    return id;
}

sig_t ContinuousActions::enabled_actions = 0;
sig_t ContinuousActions::linear_modes = 0;

