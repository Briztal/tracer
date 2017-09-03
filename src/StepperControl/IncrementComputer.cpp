/*
  PreProcessor.cpp - Part of TRACER

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

#include "IncrementComputer.h"
#include <StepperControl/MachineInterface.h>
#include <interface.h>

#define DEFAULT_ABSOLUTE_INCREMENT (float) 0.01


bool IncrementComputer::determine_increments(movement_data *movement) {

    //Cache vars
    float min = movement->min, max = movement->max;
    float min_increment, max_increment;

    //We must distinguish the two cases, of an ascending movement, and a descending movement.
    if (min<max) {
        min_increment = extract_increment(movement->trajectory_function, min, DEFAULT_ABSOLUTE_INCREMENT);
        max_increment = extract_increment(movement->trajectory_function, max, -DEFAULT_ABSOLUTE_INCREMENT);

    } else {
        min_increment = extract_increment(movement->trajectory_function, min, -DEFAULT_ABSOLUTE_INCREMENT);
        max_increment = extract_increment(movement->trajectory_function, max, DEFAULT_ABSOLUTE_INCREMENT);

    }


    /* We must check if the movement is a micro movement. A movement is qualified by micro-movement when
     *      its first sub-movement goes beyond its limit. It is not actually traceable, so we must ignore it.
     */
    if (((min_increment > 0) && (min + min_increment > max)) || ((min_increment < 0) && (min + min_increment < max))) {

        //Send an error message
        CI::echo("ERROR : THE MOVEMENT PROVIDED IS A MICRO MOVEMENT, AND WILL BE IGNORED.");

        //ignore movement
        return false;

    }

    //Save cache vars
    movement->min_increment = min_increment;
    movement->max_increment = max_increment;

    //No error
    return true;
}


/*
 * extract_increment : this function takes in arguments :
 *      - get_position : a trajectory function
 *      - point : a float, of the dimension of get_function's input.
 *      - increment : a distance from point, of the same dimension than point
 *      - distance_target : a distance in the stepper coordinate system.
 *
 *  With these inputs, it extracts a value of increment that verifies the property below.
 *
 *  "The maximum step_distances on all steppers axis between the images of point and point+increment through the
 *      trajectory function is target_distance"
 *
 *  Literally, this function is called during the scheduling of a movement, when the increment variable
 *      must be determined.
 *
 */

float IncrementComputer::extract_increment(void (*get_position)(float, float *), float point, float increment) {

    //Arrays initialisation
    float initial_positions[NB_AXIS], positions[NB_AXIS];

    //Get the initial stepper position
    MachineInterface::get_stepper_positions_for(get_position, point, initial_positions);

    //Get the candidate stepper_position
    MachineInterface::get_stepper_positions_for(get_position, point + increment, positions);

    //get the distance between the initial distance and the candidate position.
    uint32_t d = get_max_dist(initial_positions, positions);

    //cache float var for distance_target
    float ftarget = (float) DISTANCE_TARGET;


    //correct the increment until it gives a position at the target distance.
    while (d != DISTANCE_TARGET) {

        if (!d) d = 1;

        //Update the increment
        increment *= ftarget / (float) d;

        //Get the new candidate stepper_position
        MachineInterface::get_stepper_positions_for(get_position, point + increment, positions);

        //get the distance between the initial distance and the new candidate position.
        d = get_max_dist(initial_positions, positions);

    }

    //return the increment that gives a position at the target distance of the initial position.
    return increment;

}


/*
 * get_max_dists : this function gets the step_distances between the two provided stepper positions, p0 and p1.
 *
 *  For each stepper, it determines the real distance (float), and determines the absolute integer distance.
 *
 */

uint32_t IncrementComputer::get_max_dist(float *p0, float *p1) {

    //initialise the maimum distance
    uint32_t max_dist = 0;

    //for each stepper
    for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {


        //get the algebric distance
        int32_t dist = (int32_t) p1[stepper] - (int32_t) p0[stepper];

        //obtain the absolute distance
        if (dist < 0) {
            dist = -dist;
        }

        //cast to unsigned
        uint32_t distance = (uint32_t) dist;

        //update max_dist
        if (distance > max_dist) {
            max_dist = distance;
        }

    }

    //return the maximum distance.
    return max_dist;

}

#endif