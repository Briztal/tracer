/*
  ServoActions.cpp -  Part of TRACER

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

#include "ServoActions.h"

void ServoActions::begin() {
#define SERVO(i, name, pin, min, max) \
    servo##i->attach(pin, min, max);

#include "../config.h"
}

#define SERVO(i, name, pin, min, max) \
void ServoActions::set##i (float f) {\
    servo##i->write(f);\
}

#include "../config.h"

#undef SERVO

//###################################################SERVO INIT#########################################################
#define SERVO(i, name, pin, min, max) \
Servo *ServoActions::servo##i = new Servo();\


#include "../config.h"

#undef SERVO

