/*
  MotorRegisterer.cpp - Part of TRACER

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

#include "MotorRegisterer.h"


/*
 * Constructor : initialise the flags array;
 */

MotorRegisterer::MotorRegisterer(uint8_t nb_motors) :
        nbMotors(nb_motors), registrationFlags(new bool[nb_motors]{false}) {}


/*
 * Destructor : deletes the flags array;
 */

MotorRegisterer::~MotorRegisterer() {

    //Delete the array;
    delete registrationFlags;

}


/*
 * setRegistered : if the index corresponds to a motor, mark it as registered by setting its flag;
 */

void MotorRegisterer::setRegistered(uint8_t index) {

    //If the index is valid :
    if (index < nbMotors) {

        //Set the motor's flag;
        registrationFlags[index] = true;

    }

}


/*
 * isRegistered : return true is the [index]-th motor is already registered, or if the index is invalid;
 */

bool MotorRegisterer::isRegistered(uint8_t index) {

    //If the index is invalid :
    if (index >= nbMotors) {

        //Return true for safety
        return true;
    }

    //If the index is valid, return the flag of the motor referenced;
    return registrationFlags[index];
}


/*
 * allRegistered : return true only if all motors are registered;
 */

bool MotorRegisterer::allRegistered() {

    //For every motor :
    for (uint8_t motor_index = 0; motor_index < nbMotors; motor_index++) {

        //If the motor is not registered :
        if (!registrationFlags[motor_index]) {

            //Return false;
            return false;

        }

    }

    //If all motors are registered, return true;
    return true;

}
