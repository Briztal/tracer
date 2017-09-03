/*
  Kernel1.cpp - Part of TRACER

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

#include "KinematicsCore1.h"
#include "K1RealTimeProcessor.h"

#include <StepperControl/TrajectoryTracer.h>
#include <hardware_language_abstraction.h>
#include <StepperController.h>



void KinematicsCore1::initialise_tracing_procedure() {
    //TODO
}

//-----------------------------------------------Kernel status flags------------------------------------------------

//The function to call to verify that sub_movements are available in the queue.
uint8_t KinematicsCore1::available_sub_movements() {
    //TODO
}

//The function to call to know if the current movement has been processed.
bool KinematicsCore1::movement_processed() {
    //TODO
}


//---------------------------------------------Current movement update----------------------------------------------


uint8_t KinematicsCore1::update_current_movement(k1_movement_data *movement_data) {
    //TODO
}

void KinematicsCore1::initialise_movement_data(k1_movement_data *movement_data) {
    //TODO
}

void KinematicsCore1::compute_jerk_data(const k1_movement_data *current_movement, k1_movement_data *previous_movement) {
    //TODO
}

//------------------------------------------------environment update------------------------------------------------


void KinematicsCore1::update_movement_environment(k1_movement_data *movement_data) {
    //TODO
}

void KinematicsCore1::update_jerk_environment(k1_movement_data *movement_data) {
    //TODO
}


//--------------------------------------------sub_movements preparation---------------------------------------------


//The first sub_movement preparation, called at the beginning of the movement routine.
void KinematicsCore1::prepare_first_sub_movement(uint8_t *elementary_distances, sig_t *negative_signature, float *time) {
    //TODO
}

//The sub_movement preparation function, called on interrupts.
void KinematicsCore1::prepare_next_sub_movement(uint8_t * elementary_distances, sig_t *negative_signatures, float *delay) {


    *negative_signatures = position_processor(elementary_distances);//2*(NB_STEPPERS - 1) tics

    //Distances Processing

    STEP_AND_WAIT

    K1RealTimeProcessor::update_end_jerk_distances(*negative_signatures, elementary_distances);


    STEP_AND_WAIT;


    K1RealTimeProcessor::regulate_speed();


    STEP_AND_WAIT;


    if (K1RealTimeProcessor::delay0_update_required) {


        K1RealTimeProcessor::update_delay_0();

        //Speed Setting
        (*speed_processor)();


        STEP_AND_WAIT;

        //Actions setting
        K1RealTimeProcessor::setActionsSpeeds();

    }

    return;
}


//----------------------------------------------------positon log---------------------------------------------------


void send_position() {}


#endif