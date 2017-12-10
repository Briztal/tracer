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
  aint32_t with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/

/*
#include "ServoActions.h"

void ServoActions::init() {
#define SERVO(k1_position_indice, name, pin, beginning, ending) \
    servo##k1_position_indice->attach(pin, beginning, ending);

#include "../config_files.h"
}

#define SERVO(k1_position_indice, name, pin, beginning, ending) \
void ServoActions::set##k1_position_indice (float function) {\
    servo##k1_position_indice->write(function);\
}

#include "../config_files.h"

#undef SERVO

//###################################################SERVO INIT#########################################################
#define SERVO(k1_position_indice, name, pin, beginning, ending) \
Servo *ServoActions::servo##k1_position_indice = new Servo();\


#include "../config_files.h"

#undef SERVO

*/