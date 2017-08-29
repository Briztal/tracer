/*
  JerkPlanner.cpp - Part of TRACER

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


#include <StepperControl/MachineInterface.h>
#include <StepperControl/_kernels_data.h>
#include <Core/EEPROMStorage.h>
#include "JerkPlanner.h"


/*
 * determine_jerk : this function pre-process the increment and determines the jerk parameter for
 *      the provided movement.
 *
 */

//bool JerkPlanner::determine_jerk(const float min, const float max, void (*trajectory_function)(float, float *), const uint8_t speed_group, bool jerk_checking, uint32_t *jerk_distances_offsets) {
bool JerkPlanner::determine_jerk(const k2_movement_data *current_movement, k2_movement_data *previous_movement) {

    //cache vars for min and max.
    const float min = current_movement->min, max = current_movement->max;
    const float min_increment = current_movement->min_increment, max_increment = current_movement->max_increment;
    const uint8_t speed_group = current_movement->speed_group;

    //Initialise an array for the jerk distances (initial and final)
    float jerk_distances[NB_STEPPERS];
    float t_min[NB_STEPPERS];

    void (*trajectory_function)(float, float*) = current_movement->trajectory_function;

    //Get the jerk position
    MachineInterface::get_stepper_positions_for(trajectory_function, min, t_min);

    //cache var for jerk pos
    int32_t *jerk_pos = previous_movement->jerk_position;

    //Convert to int32_t
    for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {
        jerk_pos[stepper] = (int32_t) t_min[stepper];

    }

    //Get the distances for the first sub_movement
    get_stepper_distances(min, min + min_increment, trajectory_function, jerk_distances);

    //get the correct jerk offsets for the current movement
    get_jerk_offsets(jerk_distances, speed_group, previous_movement->jerk_offsets);

    previous_movement->jerk_point = true;

    //Get the distances for the first sub_movement
    get_stepper_distances(max, max - max_increment, trajectory_function, jerk_distances);

    //update the jerk_ratios for the last movement
    update_jerk_final_data(jerk_distances, speed_group);


    return true;

}


/*
 * propagate_jerk : this function verifies on each axis that the new jerk parameters do not absorb
 *      previous ones.
 *
 *  If they do, it replaces old ones with new ones.
 *
 */

void JerkPlanner::propagate_jerk(const k2_movement_data *current_movement, k2_movement_data *previous_movement) {

    //Cache pointers
    const int32_t *current_jerk_pos = current_movement->jerk_position;
    int32_t *previous_jerk_pos = previous_movement->jerk_position;
    const uint32_t *current_jerk_offsets = current_movement->jerk_offsets;
    uint32_t *previous_jerk_offsets = previous_movement->jerk_offsets;

    for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {

        //get the absolute distance on the axis
        int32_t dist = current_jerk_pos[stepper] - previous_jerk_pos[stepper];
        if (dist < 0) dist = -dist;

        //cache
        uint32_t jd = current_jerk_offsets[stepper];

        //If the new jerk absorbs the old one
        if (previous_jerk_offsets[stepper] + dist < jd) {

            //Replace old offsets
            previous_jerk_offsets[stepper] = jd;

            //Replace old position
            previous_jerk_pos[stepper] = current_jerk_pos[stepper];
        }

    }

}


/*
 * get_jerk_offsets : this function computes the jerk offsets caused by the current movement, depending on the
 *      previous movement's ending.
 *
 */

void JerkPlanner::get_jerk_offsets(float *initial_steps_distances, const uint8_t speed_group,
                                         uint32_t *jerk_distances_offsets) {

    float initial_jerk_ratios[NB_STEPPERS];

    //-----------initial jerk ratios and minimum speed------------

    //First, determine the inverse of th high level distance for the sub_movement.
    float distance_inverse = MachineInterface::get_movement_distance_for_group(speed_group, initial_steps_distances);

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

void JerkPlanner::update_jerk_final_data(float *final_steps_distances, const uint8_t speed_group) {

    //First, determine the inverse of the high level distance for the sub_movement
    float distance_inverse =
            1 / MachineInterface::get_movement_distance_for_group(speed_group, final_steps_distances);

    //Then, update the end ratios :
    for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {
        final_jerk_ratios[stepper] = final_steps_distances[stepper] / distance_inverse;
    }
}


/*
 * get_stepper_distances : this function gets the algebric distances on each stepper, between the two provided
 *      positions of the provided trajectory.
 */

void JerkPlanner::get_stepper_distances(const float p0, const float p1, void (*trajectory_function)(float, float *),
                                              float *distances) {

    float t0[NB_STEPPERS]{0}, t1[NB_STEPPERS]{0};

    MachineInterface::get_stepper_positions_for(trajectory_function, p0, t0);

    MachineInterface::get_stepper_positions_for(trajectory_function, p1, t1);

    for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {

        distances[stepper] = t1[stepper] - t0[stepper];

    }

}








float t_fjr[NB_STEPPERS];
float *const JerkPlanner::final_jerk_ratios = t_fjr;

