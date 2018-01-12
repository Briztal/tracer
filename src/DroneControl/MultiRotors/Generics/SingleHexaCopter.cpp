/*
  SingleHexaCopter.cpp - Part of TRACER

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

#include "SingleHexaCopter.h"

#include <Interaction/Interaction.h>


/*
 * Constructor : calls the SolidMultiRotor constructor, and solves the control model;
 */

SingleHexaCopter::SingleHexaCopter() : SolidMultiRotor(), MotorRegisterer(6) {}


/*
 * Destructor : deletes the registered motors array;
 */

SingleHexaCopter::~SingleHexaCopter() = default;


/*
 * setCoordinateSystem : enable all 6 coordinates;
 */

void SingleHexaCopter::setCoordinateSystem(MultiRotorCoordinateSystem *coordinate_system) {

    //Enable all x, y, z, pitch, roll and yaw;
    coordinate_system->z_en = true;
    coordinate_system->x_en = true;
    coordinate_system->y_en = true;
    coordinate_system->pitch_en = true;
    coordinate_system->roll_en = true;
    coordinate_system->yaw_en = true;

}


/*
 * setRegistered : register a motor for the hexa-copter. All parameters must be provided;
 */

void SingleHexaCopter::registerMotor(uint8_t motor_index, float x, float y, float z, float phi, float theta,
                                     bool direction, float traction_coeff, float torque_coeff, float kV, float voltage,
                                     float max_signal, uint8_t servo_index) {


    //If the [index]-th motor is already registered, or doesn't exist :
    if (isRegistered(motor_index)) {

        //Log;
        std_out("Error in SingleHexaCopter::setRegistered : the index is incorrect, or the motor has already been added.");

        //Fail;
        return;

    }

    //Create the motor data with all provided values, leaving z, theta and phi to zero;
    MultiRotorMotorData *motor_data = new MultiRotorMotorData
            (x, y, z, phi, theta, direction, traction_coeff, torque_coeff, kV, voltage, max_signal, servo_index);

    //Register the motor;
    SolidMultiRotor::addMotor(motor_data);

    //Mark the motor as registered;
    setRegistered(motor_index);

}
