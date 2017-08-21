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
void ContinuousActions::set_power##i (float f) {\
    if (f<=0){\
        stop_##i();\
        return;\
    }\
    if (f<=max)\
        analog_write(pin,  f * ((float) 255.0 / (float) max));\
    else \
        analog_write(pin, 255);\
}\
void ContinuousActions::stop_##i() {\
    analog_write(pin,0);\
}


#include <config.h>

#undef CONTINUOUS

