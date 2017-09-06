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

#if defined(ENABLE_STEPPER_CONTROL) && (KERNEL == 1)

#include "K1Physics.h"
#include "KinematicsCore1.h"

#include <StepperControl/TrajectoryTracer.h>
#include <hardware_language_abstraction.h>
#include <StepperControl/StepperController.h>
#include <interface.h>
#include <Core/EEPROMStorage.h>

void KinematicsCore1::initialise_tracing_procedure() {
    K1Physics::initialise_tracing_procedure();
}

//---------------------------------------------Current movement update----------------------------------------------

/*
 * initialise_kinetics_data : this task is a little bit complicated, and is transmitted to K1Physics.
 *
 */

void KinematicsCore1::initialise_kinetics_data(k1_movement_data *movement_data) {

    //TODO INLINE
    K1Physics::initialise_kinetics_data(movement_data);

}


/*
 * load_pre_process_kinetics_data : this function updates the pre_processing data according to the new movement.
 *
 *  It requires some physics, so it is delegated to the K1Physics class.
 *
 */

void KinematicsCore1::load_pre_process_kinetics_data(k1_movement_data *movement_data) {
    //TODO INLINE
    K1Physics::load_pre_process_kinetics_data(movement_data);
}


void KinematicsCore1::compute_jerk_offsets(float speed, k1_movement_data *previous_movement) {

    //TODO iNLINE
    K1Physics::compute_jerk_offsets(speed, previous_movement);

}

//-------------------------------------------real_time_environment update-------------------------------------------

/*
 * load_real_time_kinetics_data : this function updates the real time data.
 *
 *  It requires some physics, so it is delegated to the K1Physics class.
 */

void KinematicsCore1::load_real_time_kinetics_data(k1_movement_data *movement_data) {

    //TODO INLINE
    //Load the kinetics data
    K1Physics::load_real_time_kinetics_data(movement_data);

    //Load the jerk data
    K1Physics::load_real_time_jerk_data(movement_data);

}


void KinematicsCore1::load_real_time_jerk_data(k1_movement_data *movement_data) {

    //TODO INLINE
    K1Physics::load_real_time_jerk_data(movement_data);

}


/*
 * update_evolution_coefficient : this function updates K1Physics evolution coefficient
 */

void KinematicsCore1::update_evolution_coefficient(float multiplier) {

    //TODO INLINE
    K1Physics::update_evolution_coefficient(multiplier);
}


//--------------------------------------------sub_movements preparation---------------------------------------------


/*
 * accept_sub_movement : updates the current high level position with the provided one.
 */

void KinematicsCore1::accept_sub_movement(sub_movement_data_t *sub_movement_data) {

    float *new_hl_position = sub_movement_data->candidate_high_level_positions;

    memcpy(current_hl_position, new_hl_position, sizeof(float) * NB_AXIS);

}


//The first sub_movement preparation, called at the beginning of the movement routine.
float KinematicsCore1::compute_us_time_for_first_sub_movement(k1_sub_movement_data *sub_movement_data) {

    //TODO INLINE
    return K1Physics::get_first_sub_movement_time(sub_movement_data);

}


//The sub_movement preparation function, called on interrupts.
float KinematicsCore1::compute_us_time_for_sub_movement(k1_sub_movement_data *sub_movement_data) {

    K1Physics::update_heuristic_end_distance();

    K1Physics::update_heuristic_jerk_distance();

    STEP_AND_WAIT;

    bool acceleration_flag = K1Physics::regulate_speed();

    STEP_AND_WAIT;

    float new_sub_movement_time = K1Physics::get_sub_movement_time(acceleration_flag);

    if (new_sub_movement_time != sub_movement_time) {

        //STEP_AND_WAIT;

        //Actions setting
        //K1Physics::setActionsSpeeds(); TODO

        sub_movement_time = new_sub_movement_time;

    }

    return new_sub_movement_time;
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
void KinematicsCore1::send_position() {

    //Send the current high level position
    CI::send_position(current_hl_position);
    //StepperController::send_position();

}


//-----------------------------------------Static declarations - definitions--------------------------------------------


#define m KinematicsCore1

float m::sub_movement_time;

//The current position in the high level system.
float t_c_hl_p_1[NB_AXIS]{0};

float *const m::current_hl_position = t_c_hl_p_1;

#undef m

#endif