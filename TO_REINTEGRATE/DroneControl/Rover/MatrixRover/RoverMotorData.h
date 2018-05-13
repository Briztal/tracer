/*
  RoverMotorData.h - Part of TRACER

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

#ifndef TRACER_ROVERMOTORDATA_H
#define TRACER_ROVERMOTORDATA_H

#include "stdint.h"

class RoverMotorData {

public:

    //A simple constructor, setting regular position values;
    RoverMotorData(float x, float y, float radius, uint8_t motor_index) :

            x(x), y(y), phi(0), theta(0), radius(radius), motor_index(motor_index)

    {}


    //The most complete constructor, setting all coefficients;
    RoverMotorData(float x, float y, float phi, float theta,  float radius, uint8_t motor_index) :

            x(x), y(y), phi(phi), theta(theta), radius(radius), motor_index(motor_index)

    {}

public:

    //------------------- Position constants -------------------

    //Motor coordinates;
    float x, y;

    //Motor orientation
    float phi, theta;

    //The wheel's radius;
    float radius;


    //------------------- Command values -------------------

    //The index of the servo controlling the motor;
    uint8_t motor_index;


};


#endif //TRACER_ROVERMOTORDATA_H
