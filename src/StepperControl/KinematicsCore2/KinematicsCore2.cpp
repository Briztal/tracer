/*
  Kernel2.cpp - Part of TRACER

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


#include <interface.h>

#if defined(ENABLE_STEPPER_CONTROL) && (KERNEL == 2)

#include "KinematicsCore2.h"
#include "K2Physics.h"
#include "JerkPlanner.h"

#include <StepperControl/TrajectoryTracer.h>
#include <StepperControl/MachineInterface.h>
#include <StepperControl/StepperController.h>


void KinematicsCore2::initialise_tracing_procedure() {
    K2Physics::start();
}


//---------------------------------------------Current movement update----------------------------------------------


/*
 * initialise_movement_data : updates the kernel2 specific data in the provided movement container;
 *
 *  It initialises :
 *      - the regulation_speed;
 *      - the regulation_speed group;
 */

void KinematicsCore2::initialise_movement_data(k2_movement_data *movement_data) {

    //Initialisation of regulation_speed variables
    movement_data->speed = MachineInterface::get_speed();
    movement_data->speed_group = MachineInterface::get_speed_group();

}

/*
 * compute_jerk_data : updates the previous movement's jerk data, according to the current movement.
 *
 *  This task is delegated to the JerkPlanner class, as the Jerk determination algorithm is a bit heavy;
 */

void KinematicsCore2::compute_jerk_data(const k2_movement_data *current_movement, k2_movement_data *previous_movement) {

    //Do all the pre-processing for the movement, and throw the eventual error
    JerkPlanner::determine_jerk(current_movement, previous_movement);

}


/*
 * update_pre_process_speed_data : this function updates the movement that KinematicsCore2 is currently pre-processing.
 */

void KinematicsCore2::update_pre_process_speed_data(k2_movement_data *movement_data) {

    //Set the regulation_speed group
    movement_speed_group = movement_data->speed_group;

    //Set the regulation_speed
    next_regulation_speed = movement_data->speed;

}


//------------------------------------------------Real-time data update------------------------------------------------


void KinematicsCore2::update_real_time_jerk_environment(k2_movement_data *movement_data) {

    //Jerk
    if (movement_data->jerk_point) {
        K2Physics::update_jerk_offsets(movement_data->jerk_offsets);
    }

}


//--------------------------------------------sub_movements preparation---------------------------------------------


void KinematicsCore2::initialise_sub_movement(k2_sub_movement_data *sub_movement_data) {

    float high_level_distances[NB_AXIS];
    float *candidate = sub_movement_data->candidate_high_level_positions;
    float *current = current_hl_position;

    //Compute the high level distances
    for (uint8_t axis = 0; axis < NB_AXIS; axis++) {
        high_level_distances[axis] = candidate[axis] - current[axis];
    }

    //Compute the movement distance for the current regulation_speed group
    sub_movement_data->movement_distance = MachineInterface::get_movement_distance_for_group(movement_speed_group, high_level_distances);


    //Translate the high level position into steppers position;
    MachineInterface::translate(sub_movement_data->candidate_high_level_positions,
                                sub_movement_data->future_steppers_positions);

    //Update the regulation_speed for the real-time processor
    sub_movement_data->regulation_speed = next_regulation_speed;

}


/*
 * accept_sub_movement : updates the current high level position with the provided one.
 */

void KinematicsCore2::accept_sub_movement(sub_movement_data_t *sub_movement_data) {

    float *new_hl_position = sub_movement_data->candidate_high_level_positions;

    memcpy(current_hl_position, new_hl_position, sizeof(float) * NB_AXIS);

}


/*
 * compute_time_for_first_sub_movement : this function sets all variables up for the movement routine.
 *
 *  It processes the first sub_movement, and then fills the sub_movement queue;
 *
 */
float KinematicsCore2::compute_time_for_first_sub_movement(k2_sub_movement_data *sub_movement_data) {



    //Get the sub_movement time
    float time = K2Physics::get_first_sub_movement_time(sub_movement_data);

    CI::echo("ftime : "+String(time));
    //update the speeds
    K2Physics::update_speeds(sub_movement_data, time);

    //Update tools powers
    float current_speed = sub_movement_data->movement_distance / time;
    TrajectoryTracer::update_tools_powers(current_speed);
    //TODO NOT KERNEL'S JOB! THE KERNEL MUST ONLY PROVIDE THE SPEED.

    return time;

}


/*
 * compute_time_for_sub_movement : This function prepares the next sub_movement.
 *
 *  Steps to enqueue_object a sub movement :
 *      - Get the next step_distances > task given to the subclass
 *      - check if obtained step_distances are long enough
 *      - If true, dequeue the distance array
 *      - if not, discard the distance array and start with a new increment
 *
 * IMPORTANT : END_DISTANCE PROCESSING
 *
 *  It processes the next steps :
 *      - Process the next step_distances
 *      - Adjust the regulation_speed
 *      - set the new regulation_speed values
 *      - get a new distance array
 *      - if queue not empty, get one more new distance array
 *
 */


float KinematicsCore2::compute_time_for_sub_movement(k2_sub_movement_data *sub_movement_data) {

    STEP_AND_WAIT;

    //4us 4 steppers, 13us 17 steppers : 1.23us + 0.7 per stepper
    //Step 3 : Update the regulation_speed movementDistance with the new heuristic step_distances
    float time = K2Physics::get_sub_movement_time(sub_movement_data);

    STEP_AND_WAIT;

    //5us 4 steppers, 9us 17 steppers : 3.76us + 0.3us per stepper

    //Step 4 : Update the steppers speeds
    K2Physics::update_speeds(sub_movement_data, time);

    //Update tools powers
    float current_speed = sub_movement_data->movement_distance / time;
    TrajectoryTracer::update_tools_powers(current_speed);
    //TODO NOT KERNEL'S JOB! THE KERNEL MUST ONLY PROVIDE THE SPEED.

    return time;
}


//----------------------------------------------------positon log---------------------------------------------------


/*
 * send_position : this function sends the high level position to the controller.
 *
 *  It first inverts the current stepper positions, to obtain the high level position.
 *
 *  Then, it sends it using the interface.
 *
 */
void KinematicsCore2::send_position() {

    //Send the current high level position
    CI::send_position(current_hl_position);
    //StepperController::send_position();


}

//-----------------------------------------Static declarations - definitions--------------------------------------------

//Positions
float t_rl_pos[NB_AXIS]{0};
float *const KinematicsCore2::current_hl_position = t_rl_pos;


//Speed group for the current movement
uint8_t KinematicsCore2::movement_speed_group = 0;

//Regulation speed for the current movement
float KinematicsCore2::next_regulation_speed = 0;




#endif