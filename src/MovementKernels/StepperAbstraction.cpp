/*
  StepperAbstraction.cpp - Part of TRACER

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
 * StepperAbstraction : this class is the interface between the high level coordinate system
 *      the stepper coordinate system.
 *
 *  It is also in charge of the current position storage.
 *
 */

#include <stdint.h>
#include <Arduino.h>
#include "StepperAbstraction.h"
#include "../Core/EEPROMStorage.h"


/*
 * translate : this function translates a position expressed in the high level coordinate system into
 *      its image, in the stepper coordinate system.
 *
 */

void StepperAbstraction::translate(const float *const hl_coordinates, long *const steppers_coordinates) {

    for (uint8_t axis = 0; axis < NB_STEPPERS; axis++) {
        steppers_coordinates[axis] = (long) (EEPROMStorage::steps[axis] * hl_coordinates[axis]);
    }

}


/*
 * invert : this function translates a position expressed in the stepper coordinate system into
 *      its image, in the high level coordinate system.
 *
 */

void StepperAbstraction::invert(const long *const steppers_coordinates, float *const hl_coordinates) {

    for (uint8_t axis = 0; axis < NB_STEPPERS; axis++) {
        hl_coordinates[axis] = (float) (steppers_coordinates[axis]) / EEPROMStorage::steps[axis];
    }

}


/*
 * get_current_position : this function provides the current position to any external process.
 *
 */

void StepperAbstraction::get_current_position(float *const position) {
    memcpy(position, current_position, sizeof(float) * NB_AXIS);
}


/*
 * update_position : this function updates the current position with the new position provided as argument.
 *
 */

void StepperAbstraction::update_position(const float *const new_position) {
    memcpy(current_position, new_position, sizeof(float) * NB_AXIS);
}


//Static declaration - definition :
float t_hl_pos[NB_AXIS];
float *StepperAbstraction::current_position = t_hl_pos;