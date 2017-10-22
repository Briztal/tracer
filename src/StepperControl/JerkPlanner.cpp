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



#include <config.h>

#ifdef ENABLE_STEPPER_CONTROL


#include <StepperControl/MachineInterface.h>
#include <StepperControl/_kinematics_data.h>
#include <EEPROMStorage/EEPROMStorage.h>
#include "JerkPlanner.h"

#include <StepperControl/KinematicsCore1/KinematicsCore1.h>
#include <StepperControl/KinematicsCore2/KinematicsCore2.h>


/*
 * control_and_initialise_jerk : this function pre-process the increment and determines the jerk parameter for
 *      the provided movement.
 *
 */

bool JerkPlanner::control_and_initialise_jerk(const movement_data_t *current_movement,
                                              movement_data_t *previous_movement, bool jerk_control) {

    //cache vars for min and max.
    const float min = current_movement->min, max = current_movement->max;
    const float min_increment = current_movement->min_increment, max_increment = current_movement->max_increment;
    const uint8_t speed_group = current_movement->speed_group;

    //Initialise an array for the jerk step_distances (initial and final)
    float jerk_distances[NB_STEPPERS];
    float t_min[NB_STEPPERS];

    //Cache for the current movement's trajectory function.
    void (*trajectory_function)(float, float *) = current_movement->pre_process_trajectory_function;

    //if we must do a jerk verification
    if (jerk_control) {

        float final_jerk_ratios[NB_STEPPERS];

        //Update the final jerk ratios with the final sub movement data
        get_final_jerk_ratios(saved_final_stepper_positions, speed_group, final_jerk_ratios);

        //Get the jerk position
        MachineInterface::get_stepper_positions_for(trajectory_function, min, t_min);

        //cache var for jerk pos
        int32_t *jerk_pos = previous_movement->jerk_position;

        //Convert to int32_t
        for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {
            jerk_pos[stepper] = (int32_t) t_min[stepper];
        }

        //Get the step_distances for the first sub_movement
        get_stepper_distances(min, min + min_increment, trajectory_function, jerk_distances);

        //Get the initial jerk ratios
        float speed = get_jerk_point_speed(jerk_distances, speed_group, final_jerk_ratios);

        //get the correct jerk offsets for the current movement
        Kinematics::compute_jerk_offsets(speed, previous_movement);

        previous_movement->jerk_point = true;

    }

    //Anyway, init the jerk control for the next movement

    //Get the step_distances for the last sub_movement
    get_stepper_distances(max, max - max_increment, trajectory_function, saved_final_stepper_positions);

    get_final_jerk_ratios(saved_final_stepper_positions, speed_group, previous_movement->final_jerk_ratios);

    return true;

}

float JerkPlanner::get_jerk_point_speed(const float *initial_steps_distances, const uint8_t speed_group, const float *final_jerk_ratios) {

    //First, determine the inverse of th high level distance for the sub_movement.
    float distance_inverse = (float) 1 / MachineInterface::get_movement_distance_for_group(speed_group, initial_steps_distances);

    float speed = 0;
    bool first = true;

    for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {

        //determine the initial jerk_ratio for the current stepper;
        float initial_jerk_ratio = initial_steps_distances[stepper] * distance_inverse;

        //determine the algebric ratio difference.
        float algebric_difference = final_jerk_ratios[stepper] - initial_jerk_ratio;

        //extract the absolute difference
        if (algebric_difference < 0) algebric_difference = -algebric_difference;

        //Get the maximum regulation_speed.
        //Formula : regulation_speed < max_jerk(unit/s) / abs(final_ratio - initial_ratio)
        stepper_data_t *data = EEPROMStorage::steppers_data+stepper;
        float max_speed = data->jerk * data->steps / algebric_difference;

        //Update the maximum regulation_speed.
        speed = (first) ? speed : ((speed < max_speed) ? speed : max_speed);

        //first stepper is done.
        first = false;

    }

    return speed;

}


/*
 * get_stepper_distances : this function gets the algebric step_distances on each stepper, between the two provided
 *      positions of the provided trajectory.
 */

void JerkPlanner::get_stepper_distances(const float p0, const float p1, void (*trajectory_function)(float, float *),
                                        float *distances) {

    float t0[NB_STEPPERS]{0}, t1[NB_STEPPERS]{0};

    //Get the stepper position for the point p0
    MachineInterface::get_stepper_positions_for(trajectory_function, p0, t0);

    //Get the stepper position for the point p1
    MachineInterface::get_stepper_positions_for(trajectory_function, p1, t1);

    //Get distances
    for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {
        distances[stepper] = t1[stepper] - t0[stepper];
    }

}


void JerkPlanner::get_final_jerk_ratios(float *final_steps_distances, const uint8_t speed_group,
                                        float *final_jerk_ratios) {

    //First, determine the inverse of the high level distance for the sub_movement
    float distance_inverse =//TODO NO !!!!!!!!! THE MOVEMENT DISTANCE IS NOT KNOWN AS THE NEXT SPEED GROUP IS NEITHER. DUMBASS.
            1 / MachineInterface::get_movement_distance_for_group(speed_group, final_steps_distances);

    //Then, update the end ratios :
    for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {
        final_jerk_ratios[stepper] = final_steps_distances[stepper] / distance_inverse;
    }

}


float t_sfsp[NB_STEPPERS];
float *const JerkPlanner::saved_final_stepper_positions = t_sfsp;


#endif
