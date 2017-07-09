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
  along with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "ContinuousActions.h"
#include "Arduino.h"

void ContinuousActions::begin() {
    //INIT : PWM à 0;
#define CONTINUOUS(i, name, pin, max) \
    analogWrite(pin, 0);

#include "../config.h"

#undef CONTINUOUS
}

//TODO CHECK DES VALEURS MAX
//TODO PASSER PAR UN CONST FLOAT ET VOIR SI CA ACCELERE

bool ContinuousActions::linear_modes_enabled() {

#define CONTINUOUS(i, name, pin, max) a= a||in_linear_mode_##i;

    bool a = false;

#include "../config.h"
#undef CONTINUOUS
    return a;
}



#define CONTINUOUS(i, name, pin, max) \
void ContinuousActions::setPower##i (float f) {\
    in_linear_mode_##i = false;\
    if (f<=0){\
        disable##i();\
        return;\
    }\
    enabled##i=true;\
    if (f<=max)\
        analogWrite(pin,  f * (255.0 / (float)max));\
    else \
        analogWrite(pin, 255);\
}\
void ContinuousActions::setLinearPower##i (float f) {\
    if (!enabled##i){\
        linearPower##i = f;\
        in_linear_mode_##i = true;\
    }\
}\
void ContinuousActions::set_power_for_speed_##i (float speed) {\
    if (speed<=0){\
        disable##i();\
        return;\
    }\
    float power = speed*linearPower##i;\
    enabled##i=true;\
    if (power<=max) {\
        analogWrite(pin,  power * (255.0 / (float)max));\
    } else \
        analogWrite(pin,  255);\
}\
void ContinuousActions::disable##i() {\
    enabled##i = false;\
    analogWrite(pin, 0);\
}\
float ContinuousActions::linearPower##i = 0;\
bool ContinuousActions::enabled##i = false;\
bool ContinuousActions::in_linear_mode_##i = false;

#include "../config.h"

#undef CONTINUOUS

unsigned char ContinuousActions::getSetFunctions(void (**f)(float)) {
    unsigned char id = 0;
#define CONTINUOUS(i, name, pin, max) \
    if (in_linear_mode_##i) {\
        f[id] = set_power_for_speed_##i;\
        id++;\
    }
#include "../config.h"
#undef CONTINUOUS
    return id;
}
