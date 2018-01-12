/*
  TriRover.cpp - Part of TRACER

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


#include "TriRover.h"


/*
 * Constructor : calls the SolidMultiRotor constructor, ans solves the control model;
 */

TriRover::TriRover() : MatrixRover(), MotorRegisterer(3) {}


/*
 * Destructor : deletes the registered motors array;
 */

TriRover::~TriRover() = default;

/*
 * setCoordinateSystem : set the common (z, pitch, roll, yaw) coordinate system;
 */

void TriRover::setCoordinateSystem(RoverCoordinateSystem *coordinate_system) {

    //Enable only z, pitch, roll and yaw;
    coordinate_system->x_en = true;
    coordinate_system->y_en = true;
    coordinate_system->yaw_en = true;

}


/*
 * setRegistered : register a motor for the quadcopter (no z, phi or theta).
 */

void TriRover::registerMotor(uint8_t motor_index, float x, float y, float phi, float theta, float radius,
                             uint8_t servo_index) {


    //If the [index]-th motor is already registered, or doesn't exist :
    if (isRegistered(motor_index)) {

        //Log;
        std_out("Error in TriRover::setRegistered : the index is incorrect, or the motor has already been added.");

        //Fail;
        return;

    }

    //Create the motor data with all provided values, leaving z, theta and phi to zero;
    RoverMotorData *motor_data = new RoverMotorData(x, y, phi, theta, radius, servo_index);

    //Register the motor;
    MatrixRover::addMotor(motor_data);

    //Mark the motor as registered;
    setRegistered(motor_index);

}
