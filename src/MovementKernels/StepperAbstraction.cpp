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


#include <config.h>

#ifdef ENABLE_STEPPER_CONTROL

#include <stdint.h>
#include <Arduino.h>
#include "StepperAbstraction.h"
#include "../Core/EEPROMStorage.h"
#include <interface.h>
#include <MovementKernels/Kernel2/RealTimeProcessor.h>


/*
 * translate : this function translates a position expressed in the high level coordinate system into
 *      its image, in the stepper coordinate system.
 *
 */

void StepperAbstraction::translate(const float *const hl_coordinates, float *const steppers_coordinates) {

    //0.8 us
    for (uint8_t axis = 0; axis < NB_STEPPERS; axis++) {
        steppers_coordinates[axis] = (EEPROMStorage::steps[axis] * hl_coordinates[axis]);

    }
}


/*
 * invert : this function translates a position expressed in the stepper coordinate system into
 *      its image, in the high level coordinate system.
 *
 */

void StepperAbstraction::invert(const float *const steppers_coordinates, float *const hl_coordinates) {

    for (uint8_t axis = 0; axis < NB_STEPPERS; axis++) {
        hl_coordinates[axis] = (steppers_coordinates[axis]) / EEPROMStorage::steps[axis];
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

    //Update the current position
    memcpy(current_position, new_position, sizeof(float) * NB_AXIS);

    //Update the low level's end point.
    RealTimeProcessor::update_end_position(new_position);

}


/*
 * get_speed : this function returns the speed on the current speed group.
 *
 */

float StepperAbstraction::get_speed() {
    return speeds[speed_group];
}


/*
 * get_speed_group : this function provides the speed group to any external process.
 */

uint8_t StepperAbstraction::get_speed_group() {
    return speed_group;
}


/*
 * set_speed_group : this function updates the current speed group with the value provided inargument.
 */

void StepperAbstraction::set_speed_group(uint8_t speed_group) {
    StepperAbstraction::speed_group = speed_group;
}


/*
 * set_speed_for_group : this function updates the speed for the group provided in argument with the speed
 *      provided in argument, or with the maximum speed for this group if the speed is greater than the limit.
 *
 */

void StepperAbstraction::set_speed_for_group(uint8_t speed_group, float new_speed) {

    //Get the maximum speed for this group
    float max_speed = max_speeds[speed_group];

    //update speeds with the minimum of speed and the maximum speed
    speeds[speed_group] = min(max_speed, new_speed);

}

//Static declaration - definition :
#define m StepperAbstraction

//Positions
float t_hl_pos[NB_AXIS]{0};
float *m::current_position = t_hl_pos;

//Speeds
float t_speeds[NB_CARTESIAN_GROUPS];
float *const m::speeds = t_speeds;

uint8_t m::speed_group;
//Maximum speeds

//declare and fill the array
static const float t_mx_speeds[NB_CARTESIAN_GROUPS + 1] = {

#define CARTESIAN_GROUP(i, a, b, c, s) s,

#include <config.h>

#undef CARTESIAN_GROUP
        //end the array
        0};
//Assign the array

const float *const m::max_speeds = t_mx_speeds;

#undef m

#endif