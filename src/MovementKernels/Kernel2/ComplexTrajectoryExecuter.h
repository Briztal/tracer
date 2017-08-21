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


/*
 * ComplexTrajectoryExecuter : this class is in charge of the two following tasks :
 *
 *  - Managing the planned movement : it disposes of a movement queue containing all data related to any kind
 *      of movement, and provides functions to enqueue a movement.
 *
 *  - Executing the movements : it executes the procedure to trace movement and to regulate speed, for each movement.
 *
 */

#include <config.h>

#ifdef ENABLE_STEPPER_CONTROL

#ifndef TRACER_TRAJECTORYEXECUTER_H
#define TRACER_TRAJECTORYEXECUTER_H

#include "../../hardware_language_abstraction.h"
#include "_kernel_2_data.h"
#include "../../DataStructures/Queue.h"
#include "../StepperController.h"

class ComplexTrajectoryExecuter {


    //--------------------------------------------movement_queue_management---------------------------------------------


private:

    static Queue<k2_movement_data> movement_data_queue;


    //-----------------------------------------------sub_movement_queue-------------------------------------------------

public :

    //The state of the movement routine;
    static bool started;


private :

    //the stop flag : enabled when all sub_movements have been processed
    static bool stop_programmed;

    //the final sub_movement flag : enabled when the last sub_movement of the movement procedure has begun.
    static bool final_sub_movement_started;

    //the queue lock flag :
    static bool queue_lock_flag;


    //---------------------------------------------Real_Time_Movement_data----------------------------------------------

public :

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

    //The arrays to store signatures. They alternately store the current signature, or the saved signatures
    static sig_t *const es0, *const es1;

    //The array flag : is true when saved elementary_signatures is es0.
    static bool is_es_0;


    //------------------------------------------------Movement_Procedure------------------------------------------------

public :

    //movement routing start
    static void start();

    //movement routine interruption
    static void stop();

    //The function to verify that the enqueueing is authorised. returns true if the push isn't unauthorised.
    static bool enqueue_unauthorised();

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

private :

    //Signatures processing
    static void process_signatures(uint8_t *const elementary_dists, sig_t *elementary_signatures);

    //Method to initialise a sub_movement
    static sig_t *initialise_sub_movement();

    //Method to initialise the first sub movement of the movement procedure
    static void prepare_first_sub_movement();


    //--------------------------------------------------------Tools-----------------------------------------------------

    //NOT WORKING YET

public :

    //The function to update the action variables
    static void update_tools_powers(float time, float distance) ;

private:

    //Number of actions enabled during the current movement
    static uint8_t tools_nb;

    //The action linear power array
    static float *tools_linear_powers;

    //The container of action linear power arrays
    static float *const tools_linear_powers_storage;

    //The variables for action data update :
    static uint8_t next_tools_powers_indice;
    static sig_t next_tools_signature;

    //The flag for a movement speeds switch
    static bool movement_switch_flag;

    //The number of sub_movements before the movement switch
    static uint8_t movement_switch_counter;

    //The function to update actions speed
    static void (**tools_update_functions)(float);

    //the function to change the linear powers and functions
    static void update_tools_data();


};


#define WAIT\
    while(!stepper_int_flag) {}\
        stepper_int_flag_clear();

#define ME ComplexTrajectoryExecuter

#define STEP_AND_WAIT \
    {\
        sig_t s_w_signature;\
        s_w_signature = ME::saved_elementary_signatures[ME::trajectory_array[ME::saved_trajectory_indice--]];\
        StepperController::fastStep(s_w_signature);\
    }\
    WAIT


#endif //TRACER_MOTIONEXECUTER_H


#endif