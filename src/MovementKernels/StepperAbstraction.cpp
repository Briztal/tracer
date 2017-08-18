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


/*
 * get_speed : this function returns the speed on the current speed group.
 *
 */

float StepperAbstraction::get_speed() {
    return speeds[speed_group];
}


/*
 * get_movement_distance_for_group : this function computes the movement distance for the movement provided in argument,
 *      in the cartesian group provided in argument.
 *
 *  The movement is provided in the form of its distances.
 *
 *  The distance in the group is defined as the norm2 of the distance vector's projected
 *      in the concerned cartesian group
 *
 */

float StepperAbstraction::get_movement_distance_for_group(uint8_t speed_group, float *distances) {

    float square_dist_sum = 0;

    //Initialise the stepper index pointer
    const int8_t * indexes = (speed_groups_indices + 3 * speed_group);

    //Sum the square of all distance :
    for (uint8_t stepper = 0; stepper < 3; stepper++) {

        //Get the current axis value
        int8_t index = indexes[stepper];

        //if the cartesian group comprises less than 3 axis;
        if (index==-1) break;

        //get the distance
        float dist = distances[indexes[stepper]];

        //update the square sum
        square_dist_sum += dist * dist;
    }

    //compute the square root and return it.
    return (float) sqrt(square_dist_sum);
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
float t_hl_pos[NB_AXIS];
float *m::current_position = t_hl_pos;

//Speeds
float t_speeds[NB_CARTESIAN_GROUPS];
float *const m::speeds = t_speeds;

uint8_t m::speed_group;

//Speed groups indices

//declare and fill the array
int8_t t_sg_indices[3 * NB_CARTESIAN_GROUPS + 1] = {

#define CARTESIAN_GROUP(i, a, b, c, s) a, b, c,

#include <config.h>

#undef CARTESIAN_GROUP
        //end the array
        0 };

//Assign the array
const int8_t *const m::speed_groups_indices = t_sg_indices;


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
