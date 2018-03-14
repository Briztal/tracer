/*
  TrajectoryTracer.h - Part of TRACER

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
 * TrajectoryTracer : this class is in charge of the two following tasks :
 *
 *  - Managing the planned movement : it disposes of a movement queue containing all data related to any kind
 *      of movement, and provides functions to dequeue a movement.
 *
 *  - Executing the movements : it executes the procedure to trace movement and to regulate regulation_speed, for each movement.
 *
 */

#ifndef TRACER_TRAJECTORYEXECUTER_H
#define TRACER_TRAJECTORYEXECUTER_H

#include <config.h>

#ifdef ENABLE_STEPPER_CONTROL

#include <DataStructures/Queue.h>

#include "hardware_language_abstraction.h"

#include <Kernel/TaskScheduler/task_state_t.h>

#include "_core_include.h"

class TrajectoryTracer {

    //-------------------------------------------- Initalisation ---------------------------------------------

public:

    static void initialise_data();


    //-------------------------------------------- Module status ---------------------------------------------

private :

    //The state of the movement routine. Public because accessed by other modules.
    static volatile bool started;


private :

    //the stop flag : enabled when all sub_movements have been processed;
    static bool stop_programmed;


    //------------------------------------------- Movement_queue_management --------------------------------------------

public:

    //The function to verify that the enqueueing is authorised. returns true if the process isn't unauthorised;
    static bool enqueue_authorised();

    //The function to verify that the queue is locked. returns true if the process isn't unauthorised;
    static bool queue_locked();


private:

    //the queue lock flag;
    static bool movement_queue_lock_flag;


    //-------------------------------------------------- Start - Stop --------------------------------------------------

public :

    //Start the movement procedure;
    static void start();

    //Terminate the movement procedure (regular stop, only cleans the structure, doesn't shutdown interrupt);
    static void stop();

    //Abort the movement procedure (emergency stop, shutdowns the interrupt);
    static void emergency_stop();


    //-------------------------------------------- Real time movement data ---------------------------------------------
private :

    //the step_period_us for the next movement;
    static float delay_us;


    //-------------------------------------------------- Start - Stop --------------------------------------------------

public:

    //Enqueue a new movement;
    static task_state_t enqueue_movement(float min, float max, void (*m_initialisation)(), void (*m_finalisation)(),
                                         void (*trajectory_function)(float, float *),
                                         void (*pre_process_trajectory_function)(float, float *));

private:


    //Discard the current movement;
    static void discard_movement();


    //The queue that contains all movements data;
    static Queue<movement_data_t> movement_data_queue;


    //----------------------------------------------- Movement Procedure -----------------------------------------------

private:

    //new movement processing;
    static void process_next_movement();

    //Movement environment switching;
    static void update_movement_environment();

    //sub_movement finishing procedure;
    static void finish_sub_movement();

    //The function to finalise the current movement;
    static void (*movement_finalisation)();


    //the final sub_movement flag : enabled when the last sub_movement of the movement procedure has begun;
    static bool final_sub_movement_started;

    //The flag for a real-time movement switch;
    static bool movement_switch_flag;

    //The number of sub_movements before the movement switch;
    static uint8_t movement_switch_counter;


    //------------------------------------------------ Sub movements ------------------------------------------------------

private:

    //next sub_movement processing;
    static void prepare_next_sub_movement();

    //Method to initialise_hardware a sub_movement;
    static sig_t *initialise_sub_movement();

    //Method to initialise_hardware the first sub movement of the movement procedure;
    static void prepare_first_sub_movement();


    //-----------------------------------------------Algorithm constants------------------------------------------------

public:

    //An algorithm constant : contains the order in which signatures must be accessed to correctly perform a movement;
    static const uint8_t *const trajectory_array;


private:

    //This is one of the algorithm's constants. It indicated the index to start in, depending on the signatures sizes;
    static const uint8_t *const trajectory_beginning_indices;


    //--------------------------------------------------Real time data--------------------------------------------------


public: //Accessed by other programs

    //The signatures for the sub_movement that is currently executed;
    static sig_t *saved_elementary_signatures;

    //The trajectory index for the next movement;
    static uint8_t saved_trajectory_index;


private :

    //The current index in trajectory_array;
    static uint8_t trajectory_index;

    //The direction signature for the next sub_movement;
    static sig_t next_direction_signature;


    //The arrays to store signatures. They alternately store the current signature, or the saved signatures;
    static sig_t *const es0, *const es1;

    //The array flag : is true when saved elementary_signatures is es0;
    static bool is_es_0;


private :

    //Signatures processing;
    static void process_signatures(uint8_t *const elementary_dists, sig_t *elementary_signatures);


    //--------------------------------------------------------Tools-----------------------------------------------------

public :

    //The function to update the action variables;
    static void update_tools_powers(float current_speed);


private:

    //Number of actions enabled during the current movement;
    static uint8_t tools_nb;

    //The action linear power array;
    static float *tools_energy_densities;

    //The container of action linear power arrays;
    static float *const tools_energy_densities_storage;

    //The variables for action data update;
    static uint8_t next_tools_powers_index;

    //The current tool signatures;
    static sig_t current_tools_signature;

    //The function to update actions regulation_speed;
    static void (**tools_update_functions)(float);

    //the function to change the linear powers and functions;
    static void update_tools_data(const movement_data_t *movement);

    //Stop all tools;
    static void stop_tools();

};


//The step_period_us macros, used in the function below.

#define WAIT\
    while(!stepper_int_flag()) {}\
        stepper_int_flag_clear();

#define CTE TrajectoryTracer

#define STEP_AND_WAIT \
    Steppers::fastStep(CTE::saved_elementary_signatures[CTE::trajectory_array[CTE::saved_trajectory_index--]]);\
    WAIT


#endif //TRACER_MOTIONEXECUTER_H


#endif