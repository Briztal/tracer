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

#include "PreProcessor.h"
#include "RealTimeProcessor.h"

#include <MovementKernels/MachineAbstraction.h>
#include <Core/EEPROMStorage.h>

#define DEFAULT_ABSOLUTE_INCREMENT (float) 0.01


/*
 * pre_process_increment_and_jerk : this function pre-process the increment and determines the jerk parameter for
 *      the provided movement.
 *
 */

float PreProcessor::pre_process_increment_and_jerk(const float min, const float max,
                                                   void (*trajectory_function)(float, float *),
                                                   const uint8_t speed_group, bool jerk_checking,
                                                   uint32_t *jerk_distances_offsets) {


    float min_default_increment = (min < max) ? DEFAULT_ABSOLUTE_INCREMENT : -DEFAULT_ABSOLUTE_INCREMENT;

    //First, extract two correct sub_movements, for the min, and for the max.
    float min_increment = extract_increment(trajectory_function, min, min_default_increment);
    float max_increment = extract_increment(trajectory_function, max, -min_default_increment);

    //Initialise an array for the jerk distances (initial and final)
    float jerk_distances[NB_STEPPERS];

    //If a jerk checking is required (not required for the first movement of the routine)
    if (jerk_checking) {

        //Get the distances for the first sub_movement
        get_stepper_distances(min, min + min_increment, trajectory_function, jerk_distances);

        //get the correct jerk offsets for the current movement
        get_jerk_offsets(jerk_distances, speed_group, jerk_distances_offsets);

    }

    //Get the distances for the first sub_movement
    get_stepper_distances(min, min + min_increment, trajectory_function, jerk_distances);

    //update the jerk_ratios for the last movement
    update_jerk_final_data(jerk_distances, speed_group);


    return min_increment;

}

/*
 * verify_jerk_absorptions : this function verifies on each axis that the new jerk parameters do not absorb
 *      previous ones.
 *
 *  If they do, it replaces old ones with new ones.
 *
 */
void PreProcessor::verify_jerk_absorptions(uint32_t *enqueued_jerk_offsets, int32_t *enqueued_jerk_positions,
                                           uint32_t *new_jerk_offsets,
                                           int32_t *new_jerk_positions) {

    for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {

        //get the absolute distance on the axis
        float dist = new_jerk_positions[stepper] - enqueued_jerk_positions[stepper];
        if (dist<0) dist = -dist;

        //cache
        uint32_t jd = new_jerk_offsets[stepper];

        //If the new jerk absorbs the old one
        if (enqueued_jerk_offsets[stepper] + dist < jd) {

            //Replace old offsets
            enqueued_jerk_offsets[stepper] = jd;

            //Replace old position
            enqueued_jerk_positions[stepper] = new_jerk_positions[stepper];
        }

    }

}


/*
 * get_jerk_offsets : this function computes the jerk offsets caused by the current movement, depending on the
 *      previous movement's ending.
 *
 */

void PreProcessor::get_jerk_offsets(float *initial_steps_distances, const uint8_t speed_group,
                                    uint32_t *jerk_distances_offsets) {

    float initial_jerk_ratios[NB_STEPPERS];

    //-----------initial jerk ratios and minimum speed------------

    //First, determine the inverse of th high level distance for the sub_movement.
    float distance_inverse = MachineAbstraction::get_movement_distance_for_group(speed_group, initial_steps_distances);

    float speed = 0;
    bool first = true;

    for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {

        //determine the initial jerk_ratio for the current stepper;
        float initial_jerk_ratio = initial_steps_distances[stepper] * distance_inverse;

        //determine the algebric ratio difference.
        float algebric_difference = final_jerk_ratios[stepper] - initial_jerk_ratio;

        //extract the absolute difference
        if (algebric_difference < 0) algebric_difference = -algebric_difference;

        //Get the maximum speed.
        //Formula : speed < max_jerk(unit/s) / abs(final_ratio - initial_ratio)
        float max_speed = EEPROMStorage::jerks[stepper] * EEPROMStorage::steps[stepper] / algebric_difference;

        //Update the maximum speed.
        speed = (first) ? speed : ((speed < max_speed) ? speed : max_speed);

        //first stepper is done.
        first = false;

        //Save the jerk distance.
        initial_jerk_ratios[stepper] = initial_jerk_ratio;

    }

    //-----------maximum stepper speeds------------

    //Now that we know the maximum speed, we can determine the deceleration distances :

    for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {

        //get the maximum speed on the current axis :
        //Formula : max_stepper_speed = max_speed * stepper_distance / high_level_distance;
        float stepper_speed = speed * initial_jerk_ratios[stepper];

        //Get the deceleration distance offset :
        //Formula : the deceleration distance for a stepper at the speed s (steps/s) is given by
        //      s * s / (2 * acceleration (steps/s^2))
        jerk_distances_offsets[stepper] = (uint32_t) (stepper_speed * stepper_speed /
                                                      (2 * EEPROMStorage::accelerations[stepper] *
                                                       EEPROMStorage::steps[stepper]));

    }


}

void PreProcessor::update_jerk_final_data(float *final_steps_distances, const uint8_t speed_group) {

    //First, determine the inverse of the high level distance for the sub_movement
    float distance_inverse =
            1 / MachineAbstraction::get_movement_distance_for_group(speed_group, final_steps_distances);

    //Then, update the end ratios :
    for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {
        final_jerk_ratios[stepper] = final_steps_distances[stepper] / distance_inverse;
    }
}


/*
 * get_stepper_distances : this function gets the algebric distances on each stepper, between the two provided
 *      positions of the provided trajectory.
 */

void PreProcessor::get_stepper_distances(const float p0, const float p1, void (*trajectory_function)(float, float *),
                                         float *distances) {

    float t0[NB_STEPPERS]{0}, t1[NB_STEPPERS]{0};

    get_stepper_position(trajectory_function, p0, t0);

    get_stepper_position(trajectory_function, p1, t1);

    for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {

        distances[stepper] = t1[stepper] - t0[stepper];

    }

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
 *  "The maximum distances on all steppers axis between the images of point and point+increment through the
 *      trajectory function is target_distance"
 *
 *  Literally, this function is called during the scheduling of a movement, when the increment variable
 *      must be determined.
 *
 */

float PreProcessor::extract_increment(void (*get_position)(float, float *), float point, float increment) {

    //Arrays initialisation
    float initial_positions[NB_AXIS], positions[NB_AXIS];

    //Get the initial stepper position
    get_stepper_position(get_position, point, initial_positions);

    //Get the candidate stepper_position
    get_stepper_position(get_position, point + increment, positions);

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
        get_stepper_position(get_position, point + increment, positions);

        //get the distance between the initial distance and the new candidate position.
        d = get_max_dist(initial_positions, positions);

    }

    //return the increment that gives a position at the target distance of the initial position.
    return increment;

}


/*
 * get_stepper_position : this function gets the stepper positions, for the provided point, according to
 *      the provided trajectory_function (get_position).
 *
 */

void PreProcessor::get_stepper_position(void (*get_position)(float, float *), float point, float *positions) {

    //Initialisa the local high level position array;
    float hl_positions[NB_AXIS];

    //Get the initial high level position
    get_position(point, hl_positions);

    //Translate it to obtain the initial stepper position
    MachineAbstraction::translate(hl_positions, positions);
}


/*
 * get_max_dists : this function gets the distances between the two provided stepper positions, p0 and p1.
 *
 *  For each stepper, it determines the real distance (float), and determines the absolute integer distance.
 *
 */

uint32_t PreProcessor::get_max_dist(float *p0, float *p1) {

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


float t_fjr[NB_STEPPERS];
static float *const final_jerk_ratios = t_fjr;


#endif