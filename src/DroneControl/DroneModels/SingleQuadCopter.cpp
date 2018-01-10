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

SingleQuadCopter::SingleQuadCopter() : SolidMultiRotor() {

    //Solve the model;
    solve();

    //Print the matrix;
    std_out(powerMatrix->toString());

}


void SingleQuadCopter::setCoordinateSystem(SolidMultiRotor::coordinate_system_t *coordinate_system) {
    coordinate_system->z_en = true;
    coordinate_system->pitch_en = true;
    coordinate_system->roll_en = true;
    coordinate_system->yaw_en = true;

}



void SingleQuadCopter::createMotors() {

    /*
    struct motor_data_t {

        //Motor coordinates;
        float x = 0, y = 0, z = 0;

        //Motor orientation
        float theta = 0, phi = 0;

        //The motor's Kv;
        float kV = 1000;

        //The motor's traction and torque coefficients;
        float traction_coefficient = 1, torque_coefficient = 1;

        //The power voltage;
        float voltage = 11;

        //Does the motor turns in positive_rotation direction?
        bool rotation_direction = false;

        //The maximum signal value;
        float max_signal = 255;

        //The index of the servo controlling the motor;
        uint8_t servo_index = 0;

    };

     */

    //Create the front motor;
    motor_data_t front_motor = motor_data_t();
    front_motor.x = (float)0.2;
    front_motor.rotation_direction = true;
    front_motor.servo_index = 0;

    //Create the right motor;
    motor_data_t right_motor = motor_data_t();
    right_motor.y = (float)0.2;
    right_motor.rotation_direction = false;
    right_motor.servo_index = 2;


    //Create the back motor;
    motor_data_t back_motor = motor_data_t();
    back_motor.x = (float)-0.2;
    back_motor.rotation_direction = true;
    back_motor.servo_index = 2;


    //Create the left motor;
    motor_data_t left_motor = motor_data_t();
    left_motor.y = -(float)0.2;
    left_motor.rotation_direction = false;
    left_motor.servo_index = 3;

    //Add al motors;
    addMotor(&front_motor);
    addMotor(&right_motor);
    addMotor(&back_motor);
    addMotor(&left_motor);

}

void SingleQuadCopter::createRelations(LinearSystem *system) {

    //No relation to add;

}
