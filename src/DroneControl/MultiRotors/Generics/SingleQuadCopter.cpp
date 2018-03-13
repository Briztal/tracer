/*
  SingleQuadCopter.cpp - Part of TRACER

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

#include <Interaction/Interaction.h>
#include "SingleQuadCopter.h"


/*
 * Constructor : calls the SolidMultiRotor constructor, ans solves the control model;
 */

SingleQuadCopter::SingleQuadCopter() : SolidMultiRotor(), MotorRegisterer(4) {}


/*
 * Destructor : deletes the registered motors array;
 */

SingleQuadCopter::~SingleQuadCopter() = default;

/*
 * setCoordinateSystem : set the common (z, pitch, roll, yaw) coordinate system;
 */

void SingleQuadCopter::setCoordinateSystem(MultiRotorCoordinateSystem *coordinate_system) {

    //Enable only z, pitch, roll and yaw;
    coordinate_system->z_en = true;
    coordinate_system->pitch_en = true;
    coordinate_system->roll_en = true;
    coordinate_system->yaw_en = true;

}


/*
 * setRegistered : register a motor for the quadcopter (no z, phi or theta).
 */

void SingleQuadCopter::registerMotor(uint8_t motor_index, float x, float y, float z, bool direction,
                                     float traction_coeff, float torque_coeff, float kV, float voltage,
                                     float max_signal, uint8_t servo_index) {


    //If the [index]-th motor is already registered, or doesn't exist :
    if (isRegistered(motor_index)) {

        //Log;
        std_out("Error in SingleQuadCopter::setRegistered : the index is incorrect, or the motor has already been added.");

        //Fail;
        return;

    }

    //Create the motor data with all provided values, leaving z, theta and phi to zero;
    MultiRotorMotorData *motor_data = new MultiRotorMotorData
            (x, y, z, 0, 0, direction, traction_coeff, torque_coeff, kV, voltage,max_signal, servo_index);

    //Register the motor;
    //SolidMultiRotor::addMotor(motor_data);

    //Mark the motor as registered;
    setRegistered(motor_index);

}
