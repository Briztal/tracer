/*
  KinematicsCore1.cpp - Part of TRACER

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

#include "KinematicsCore2.h"
#include "K2Physics.h"

#include <StepperControl/TrajectoryTracer.h>
#include <StepperControl/StepperController.h>
#include <StepperControl/Steppers.h>

//------------------------------------------- Initialisation --------------------------------------------

/*
 * reset : this function intialises the module and its physics manager to a safe state;
 */

void KinematicsCore2::initialise_data() {

    //Reset the high level position;
    memset(current_hl_position, 0, NB_STEPPERS * sizeof(float));

    //Reset the speed group;
    movement_speed_group = 0;

    //Reset the regulation speed;
    next_regulation_speed = 0;

    //Initialise the physics manager;
    K2Physics::initialise_data();

}


//------------------------------------------- Start --------------------------------------------

void KinematicsCore2::start_tracing_procedure() {
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

void KinematicsCore2::initialise_kinetics_data(k2_movement_data *movement_data) {

    //Initialisation of regulation_speed variables
    movement_data->speed = StepperController::get_speed();
    movement_data->speed_group = StepperController::get_speed_group();

}


/*
 * load_pre_rocess_kinetics_data : TODO
 */

void KinematicsCore2::load_pre_process_kinetics_data(k2_movement_data *movement_data) {

    //Set the regulation_speed group
    movement_speed_group = movement_data->speed_group;

    //Set the regulation_speed
    next_regulation_speed = movement_data->speed;

}



//------------------------------------------------Real-time data update------------------------------------------------


void KinematicsCore2::compute_jerk_offsets(float speed, k2_movement_data *previous_movement) {

    K2Physics::compute_jerk_offsets(speed, previous_movement);

}


/*
 * TODO
 */

void KinematicsCore2::load_real_time_jerk_data(const k2_movement_data *movement_data) {

    //Jerk
    if (movement_data->jerk_point) {
        K2Physics::update_jerk_offsets(movement_data->jerk_offsets);
    }

}


//--------------------------------------------sub_movements preparation---------------------------------------------


/*
 * TODO
 */

void KinematicsCore2::initialise_sub_movement(k2_sub_movement_data *sub_movement_data) {

    float high_level_distances[NB_STEPPERS];
    float *candidate = sub_movement_data->candidate_high_level_positions;
    float *current = current_hl_position;

    //Compute the high level distances
    for (uint8_t axis = 0; axis < NB_STEPPERS; axis++) {
        high_level_distances[axis] = candidate[axis] - current[axis];
    }

    //Compute the movement distance for the current regulation_speed group
    sub_movement_data->movement_distance = StepperController::get_movement_distance_for_group(movement_speed_group,
                                                                                              high_level_distances);


    //Update the regulation_speed for the real-time processor
    sub_movement_data->regulation_speed = next_regulation_speed;

}


/*
 * accept_sub_movement : updates the current high level position with the provided one.
 */

void KinematicsCore2::accept_sub_movement(sub_movement_data_t *sub_movement_data) {

    float *new_hl_position = sub_movement_data->candidate_high_level_positions;

    memcpy(current_hl_position, new_hl_position, sizeof(float) * NB_STEPPERS);

}


/*
 * compute_us_time_for_first_sub_movement : this function sets all variables up for the movement routine.
 *
 *  It processes the first sub_movement, and then fills the sub_movement queue;
 *
 */
float KinematicsCore2::compute_us_time_for_first_sub_movement(k2_sub_movement_data *sub_movement_data) {



    //Get the sub_movement time
    float time = K2Physics::get_first_sub_movement_time(sub_movement_data);

    //update the speeds
    K2Physics::update_speeds(sub_movement_data, time);

    //Update tools powers
    float working_carriage_speed = sub_movement_data->movement_distance / time;
    TrajectoryTracer::update_tools_powers(working_carriage_speed);
    //TODO NOT CORE_VERSION'S JOB! THE CORE_VERSION MUST ONLY PROVIDE THE SPEED.

    return (float) 1000000 * time;

}


/*
 * compute_time_for_sub_movement : This function prepares the next sub_movement.
 *
 *  Steps to insert_object a sub movement :
 *      - Get the next step_distances > task given to the subclass
 *      - check if obtained step_distances are long enough
 *      - If true, dequeue the distance array
 *      - if not, discard_sub_movement the distance array and start with a new increment
 *
 * IMPORTANT : END_DISTANCE PROCESSING
 *
 *  It processes the next steps_per_unit :
 *      - Process the next step_distances
 *      - Adjust the regulation_speed
 *      - set the new regulation_speed values
 *      - get a new distance array
 *      - if queue not empty, get one more new distance array
 *
 */


float KinematicsCore2::compute_us_time_for_sub_movement(k2_sub_movement_data *sub_movement_data) {

    STEP_AND_WAIT;

    //4us 4 machine_control, 13us 17 machine_control : 1.23us + 0.7 per machine_control
    //Step 3 : Update the regulation_speed movementDistance with the new heuristic step_distances
    float time = K2Physics::get_sub_movement_time(sub_movement_data);

    STEP_AND_WAIT;

    //5us 4 machine_control, 9us 17 machine_control : 3.76us + 0.3us per machine_control

    //Step 4 : Update the machine_control speeds
    K2Physics::update_speeds(sub_movement_data, time);

    //Update tools powers
    float working_carriage_speed = sub_movement_data->movement_distance / time;
    TrajectoryTracer::update_tools_powers(working_carriage_speed);
    //TODO NOT CORE_VERSION'S JOB! THE CORE_VERSION MUST ONLY PROVIDE THE SPEED.

    return (float) 1000000 * time;
}


//----------------------------------------------------positon log---------------------------------------------------


/*
 * send_position : this function sends the high level position to the controller.
 *
 *  It first inverts the current machine_control positions, to obtain the high level position.
 *
 *  Then, it sends it using the interface.
 *
 */
void KinematicsCore2::send_position() {

    //Send the current high level position
    //CI::send_position(current_hl_position);
    //Steppers::send_position();


}

//-----------------------------------------Static declarations - definitions--------------------------------------------

//Positions
float t_rl_pos[NB_STEPPERS]{0};
float *const KinematicsCore2::current_hl_position = t_rl_pos;


//Speed group for the current movement
uint8_t KinematicsCore2::movement_speed_group = 0;

//Regulation speed for the next movement
float KinematicsCore2::next_regulation_speed = 0;




