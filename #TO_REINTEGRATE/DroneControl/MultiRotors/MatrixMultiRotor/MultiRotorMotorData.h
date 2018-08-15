/*
  MultiRotorMotorData.h - Part of TRACER

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

#ifndef TRACER_MULTIROTORCOORDINATESYSTEM_H
#define TRACER_MULTIROTORCOORDINATESYSTEM_H

#include "stdint.h"

/*
 * The structure that will contain all data related to one motor;
 */
class MultiRotorMotorData {

public:

    //A simple constructor, setting regular position values;
    MultiRotorMotorData(float x, float y, float z, bool direction,
                        float traction_coeff, float torque_coeff,
                        float kV, float voltage,
                        float max_signal, uint8_t servo_index) :

            x(x), y(y), z(z), phi(0), theta(0), rotation_direction(direction),
            traction_coefficient(traction_coeff), torque_coefficient(torque_coeff),
            kV(kV), voltage(voltage), max_signal(max_signal), servo_index(servo_index)
    {}


    //The most complete constructor, setting all coefficients;
    MultiRotorMotorData(float x, float y, float z, float phi, float theta, bool direction,
                        float traction_coeff, float torque_coeff,
                        float kV, float voltage,
                        float max_signal, uint8_t servo_index) :

            x(x), y(y), z(z), phi(phi), theta(theta), rotation_direction(direction),
            traction_coefficient(traction_coeff), torque_coefficient(torque_coeff),
            kV(kV), voltage(voltage), max_signal(max_signal), servo_index(servo_index)
    {}

public:

    //------------------- Position constants -------------------

    //Motor coordinates;
    float x, y, z;

    //Motor orientation
    float phi, theta;

    //Does the motor turns in trigonometric direction?
    bool rotation_direction;

    //------------------- Dynamics constants -------------------

    //The motor's traction and torque coefficients;
    float traction_coefficient;

    float torque_coefficient;

    //------------------- Voltage values -------------------
    //The motor's Kv;
    float kV;

    //The power voltage;
    float voltage;


    //------------------- Command values -------------------

    //The maximum signal value;
    float max_signal;

    //The index of the servo controlling the motor;
    uint8_t servo_index;


};

#endif //TRACER_MULTIROTORMOTORDATA_H
