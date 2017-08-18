/*
  ComplexTrajectoryExecuter.h - Part of TRACER

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


#include "../../config.h"

#ifdef ENABLE_STEPPER_CONTROL

#ifndef TRACER_TRAJECTORYEXECUTER_H
#define TRACER_TRAJECTORYEXECUTER_H

#include "../../hardware_language_abstraction.h"
#include "complex_motion_data.h"
#include "../../DataStructures/Queue.h"
#include "../StepperController.h"

class ComplexTrajectoryExecuter {


    //--------------------------------------------movement_queue_management---------------------------------------------

private:

    static Queue<complex_motion_data> motion_data_queue;

    //-----------------------------------------------sub_movement_queue-------------------------------------------------

public :

    //The state of the movement routine;
    static bool started;

private :

    static bool stop_programmed;
    static bool final_sub_movement_started;
    static bool jerk_point;

    //---------------------------------------------Real_Time_Movement_data----------------------------------------------

public :

    //Sub movement procedure variables :

    //The signatures for the sub_movement that is currently executed
    static sig_t *saved_elementary_signatures;

    //The trajectory indice (signature indice in the movement) for the current move and the next prepare_movement
    static uint8_t saved_trajectory_indice, trajectory_indice;

    static sig_t saved_direction_sigature;

    //The trajectory indices. Those are constant in the algorithm.
    static const uint8_t *const trajectory_indices;

    //the delay for the next movement;
    static uint32_t delay;

    //the trajectory array : contains the signature order.
    static const uint8_t *const trajectory_array;

    //--------------------------------------Sub_Movement_Pre_Computation------------------------------------------------

private :

    //The arrys to store signatures
    static sig_t *const es0, *const es1;
    static bool is_es_0;

    //------------------------------------------------Movement_Procedure------------------------------------------------


public :

    //movement routing start
    static void start();

    //movement routine interruption
    static void stop();

    //new movement enqueueing
    static void enqueue_movement(float min, float max, float incr, void (*m_initialisation)(), void (*m_finalisation)(),
                                 void (*trajectory_function)(float, float *));

private:

    //The function to finalise the current movement;
    static void (*movement_finalisation)();

    //new movement processing
    static void process_next_movement(bool first_movement);

    //next sub_movement processing
    static void prepare_next_sub_movement();

    //sub_movement finishing procedure
    static void finish_sub_movement();


    //----------------------------------------------------Signatures----------------------------------------------------

    //Signatures processing
    static void process_signatures(uint8_t *const elementary_dists, sig_t * elementary_signatures, uint8_t *trajectory_indice);

    //Method to initialise a sub_movement
    static sig_t *initialise_sub_movement();

    static void prepare_first_sub_movement();
};



#define WAIT\
    while(!stepper_int_flag) {}\
        stepper_int_flag_clear();

#define ME ComplexTrajectoryExecuter

#define STEP_AND_WAIT \
    {\
        sig_t s_w_signature;\
        while (!(s_w_signature = ME::saved_elementary_signatures[ME::trajectory_array[ME::saved_trajectory_indice--]])){}\
        StepperController::fastStep(s_w_signature);\
    }\
    WAIT


#endif //TRACER_MOTIONEXECUTER_H


#endif