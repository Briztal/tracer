/*
  IncrementComputer.cpp - Part of TRACER

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
#include <stdint.h>
#include <MovementKernels/StepperAbstraction.h>
#include "IncrementComputer.h"
#include <interface.h>

/*
 * extract_increment : this function takes as arguments :
 *      - get_position : a trajectory function
 *      - point : a float, of the dimension of get_function's input.
 *      - increment : a distance from point, of the same dimension than point
 *      - distance_target : a distance in the stepper coordinate system.
 *
 *  With these inputs, it extracts a value of increment that verifies the property below.
 *
 *  "The maximum distances on all steppers axis between the images of point and point+increment through the
 *      trajectory function is target_distance"
 *
 *  Literally, this function is called during the scheduling of a movement, when the increment variable
 *      must be determined.
 *
 */

float IncrementComputer::extract_increment(void (*get_position)(float, float *), float point, float increment,
                                           const uint32_t distance_target) {

    //Arrays initialisation
    float initial_positions[NB_AXIS], positions[NB_AXIS];

    //Get the initial stepper position
    get_stepper_position(get_position, point, initial_positions);

    //Get the candidate stepper_position
    get_stepper_position(get_position, point + increment, positions);

    //get the distance between the initial distance and the candidate position.
    uint32_t d = get_max_dist(initial_positions, positions);

    //cache float var for distance_target
    float ftarget = (float) distance_target;


    //correct the increment until it gives a position at the target distance.
    while (d != distance_target) {

        if (!d) d = 1;

        //Update the increment
        increment *= ftarget / (float) d;

        //Get the new candidate stepper_position
        get_stepper_position(get_position, point + increment, positions);

        //get the distance between the initial distance and the new candidate position.
        d = get_max_dist(initial_positions, positions);

    }

    //return the increment that gives a position at the target distance of the initial position.
    return increment;

}


void IncrementComputer::get_stepper_position(void (*get_position)(float, float *), float point, float *positions) {

    //Initialisa the local high level position array;
    float hl_positions[NB_AXIS];

    //Get the initial high level position
    get_position(point, hl_positions);

    //Translate it to obtain the initial stepper position
    StepperAbstraction::translate(hl_positions, positions);
}


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

    //return the macimum distance.
    return max_dist;

}
