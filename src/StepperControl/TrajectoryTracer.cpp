/*
  ComplexTrajectoryExecuter.cpp - Part of TRACER

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

#include "TrajectoryTracer.h"
#include "IncrementComputer.h"
#include "SubMovementManager.h"
#include <interface.h>
#include <StepperControl/MachineInterface.h>
#include <Actions/ContinuousActions.h>
#include <StepperControl/StepperController.h>
#include <StepperControl/KinematicsCore1/KinematicsCore1.h>
#include <StepperControl/KinematicsCore2/KinematicsCore2.h>
#include <StepperControl/JerkPlanner.h>

//--------------------------------------------- Movement queue management ----------------------------------------------

/*
 * enqueue_authorised : this function returns true is a movement can be enqueued, and false if not.
 */

bool TrajectoryTracer::enqueue_authorised() {

    //A movement can be enqueued only is there is almost one available space in the queue;
    return (bool) movement_data_queue.available_spaces();

}


/*
 * queue_locked : this function is used by another process, to verify if the movement queue is not locked.
 * 
 * The queue lock is a punctual interrupt-controlled action, that is independent of the queue's content. The queue can 
 *  be empty, but locked. This is used to prevent the enqueue of a moment for a short period of time.
 *  
 *      For now, it it only used when all movement have been entirely processed, and only sub_movements present
 *      in the sub_movement queue must be executed.
 * 
 *      It returns true if the queue is locked, and the enqueuing is not permitted.
 */

bool TrajectoryTracer::queue_locked() {
    return movement_queue_lock_flag;
}


//------------------------------------------------ Start - Stop ------------------------------------------------

/*
 * The function to start the movement routine. It will run in interrupts while movements are to trace, and then stop.
 *
 *  It first verifies that movement are enqueued,
 *
 *  If there are, it pops the first one, and initialises it.
 *
 *  It then prepare the movement procedure, and starts the movement routine.
 */

void TrajectoryTracer::start() {

    //Don't execute if the movement is already started.
    if (started)
        return;

    disable_stepper_interrupt();


    if (!movement_data_queue.available_objects()) {
        //If no movements are in the queue, no need to start.

        //send an error message;
        debug("No movements are present  in the queue, starting aborted");

        return;
    }

    debug("Movements procedure started.");

    Kinematics::initialise_tracing_procedure();

    //Enable all steppers;
    StepperController::enable();

    //Initialise the end booleans;
    stop_programmed = false;
    final_sub_movement_started = false;

    //Disable the emergency stop;
    emergency_stop_flag = false;

    //Initialise the K2Physics for the first movement;
    process_next_movement();

    //Update the movement environment (jerk and tools data)
    update_movement_environment();

    //Set up the movement procedure;
    prepare_first_sub_movement();

    //Setup properly the interrupt procedure;
    setup_stepper_interrupt(prepare_next_sub_movement, (uint32_t) delay_us);

    //Mark the movement procedure as started;
    started = true;

    //enable the stepper timer;//TODO TIMER
    //enable_stepper_timer();

    //Start the interrupt sequence.
    if (!emergency_stop_flag) {
        enable_stepper_interrupt();
    }

}


/*
 * stop : this function terminates the movement procedure, by simply disabling the interrupt.
 *
 *  After disabling the interrupt, it disables the timer, and set the state flag to disabled;
 *
 */

void TrajectoryTracer::stop() {

    //Interrupt the movement routing, by stopping the interrupt sequence;
    disable_stepper_interrupt()

    //Disable the stepper timer, now that the interrupt is disabled;//TODO TIMER
    //disable_stepper_timer();

    //Trigger an emergency stop, in case stop was called during the movement routine (in case of movement errors);
    emergency_stop_flag = true;

    //Stop all currently enabled tools;
    stop_tools();

    //Mark the movement routine as stopped;
    started = false;

    //Enable the movement enqueuing;
    movement_queue_lock_flag = false;

    //Display the steppers position;
    StepperController::send_position();

    //Notify that the movement is stopped;
    debug("Movement Stopped");

    //If movements have been added to the movement queue during the last sub_movements :
    if (movement_data_queue.available_objects()) {

        //We must restart.

        //Notify the restart
        debug("Warning : movements still present in the queue");

        //restart a new movement procedure;
        start();

    }

}


/*
 * stop : this function stops the movement procedure, by disabling the interrupt and setting the emergency flag,
 *      that will disable the interrupt reprogramation.
 *
 *  After disabling the interrupt, it disables the timer, and set the state flag to disables.
 *
 *  It can be called during the emergency procedure, to prevent the machine from moving.
 */

void TrajectoryTracer::emergency_stop() {

    //Trigger an emergency stop : will stop and deprogram any interrupt in execution.
    emergency_stop_flag = true;

    //Interrupt the movement routing, by stopping the interrupt sequence;
    disable_stepper_interrupt()

    //Disable the stepper timer, now that the interrupt is disabled;//TODO TIMER
    //disable_stepper_timer();

    //Stop all currently enabled tools;
    stop_tools();

    //Mark the movement routine as stopped;
    started = false;

    //Enable the movement enqueuing;
    movement_queue_lock_flag = false;

    debug("\nEmergency Stop triggered\n");

    //Display the steppers position;
    StepperController::send_position();

}

//------------------------------------------------- Queue manipulation -------------------------------------------------


/*
 * enqueue_movement : this function adds the movement provided in argument_t to the motion queue.
 *
 *  The movement is provided in the form of :
 *      - beginning and ending, the index variable minimum and maximum values, for (resp) the beginning and end positions
 *      - increment : the index increment for the first movement;
 *      - movement_initialisation : the movement's in_real_time initialisation function;
 *      - trajectory_function : the function that will be used to compute new positions in_real_time.
 */

task_state_t TrajectoryTracer::enqueue_movement(float min, float max, void (*movement_initialisation)(),
                                                void (*movement_finalisation)(),
                                                void(*pre_process_trajectory_function)(float, float *),
                                                void(*trajectory_function)(float, float *)) {


    //First, we must check that the movement queue has almost one space available.
    if (!enqueue_authorised()) {
        //If no more space is available in the data queue, reprogram the task;

        CI::echo("Error in TrajectoryTracer::enqueue_movement : There is no space left in the movement queue.");

        //Emergency stop;
        //TODO EMERGENCY_STOP
        TrajectoryTracer::emergency_stop();

        //Fail;
        return error;

    }

    //After, we must check that movement queue is unlocked.
    if (movement_queue_lock_flag) {

        //Send an error message.
        CI::echo("Error in TrajectoryTracer::enqueue_movement : "
                         "The movement queue was locked when the function was called.");

        //Emergency stop;
        //TODO EMERGENCY_STOP
        TrajectoryTracer::emergency_stop();

        //Fail;
        return error;
    }


    //---------------Movement container pointers Initialisation-----------------

    //Declare a flag;
    bool queue_flag = false;

    //Query the insertion address;
    movement_data_t *current_movement = movement_data_queue.get_insertion_address(&queue_flag);

    //Integrity check :
    if (!queue_flag) {

        //Log;
        CI::echo("Error in TrajectoryTracer::enqueue_movement : The insertion element was not allocated.");

        //Emergency stop;
        TrajectoryTracer::emergency_stop();

        //Fail;
        return error;
    }


    //---------------kernel-invariant data-----------------

    //Fill trajectory related data;
    current_movement->beginning = min;
    current_movement->ending = max;
    current_movement->trajectory_function = trajectory_function;
    current_movement->pre_process_trajectory_function = pre_process_trajectory_function;

    //Movement initialisation - finalisation;
    current_movement->movement_initialisation = movement_initialisation;
    current_movement->movement_finalisation = movement_finalisation;
    current_movement->jerk_point = false;

    //Reset the flag;
    queue_flag = false;

    //Set the current insertion position in the linear_powers storage, as the same position than in movement_data_queue;
    float *tools_linear_powers =
            tools_linear_powers_storage + NB_CONTINUOUS * movement_data_queue.get_insertion_index(&queue_flag);

    //Integrity check :
    if (!queue_flag) {

        //Log;
        CI::echo("Error in TrajectoryTracer::enqueue_movement : "
                         "The element at the insertion index was already allocated.");

        //Emergency Stop;
        //TODO EMERGENCY_STOP
        TrajectoryTracer::emergency_stop();

        //Fail;
        return error;

    }

    //Get the action signature and fill the linear_powers storage.
    current_movement->tools_signatures = MachineInterface::get_tools_data(tools_linear_powers);



    //---------------Increment computing, kernel invariant-----------------

    //The increment computer will determine the beginning and ending increment;
    if (!IncrementComputer::determine_increments(current_movement)) {

        //If the increment computer detects a micro movement, the machine will not move;
        return complete;

    }


    //---------------Kinematics variable data-----------------

    //Let the kinematics manager initialise kinetics data;
    Kinematics::initialise_kinetics_data(current_movement);


    //---------------Jerk computation-----------------

    /* The jerk computation will occur only if the previous movement is not entirely processed.
     *  As movements are discarded only when they are effectively processed, this is equialent to checking
     *  that the queue is not empty.
     */
    if (movement_data_queue.available_objects()) {

        //Get the previous movement, now that we know that the queue is not empty (the previous movement exists).

        //Reset the flag;
        queue_flag = false;

        //Query the last movement address;
        movement_data_t *previous_movement = movement_data_queue.read_inserted_object(1, &queue_flag);

        //Integrity check :
        if (!queue_flag) {

            //Log;
            CI::echo("Error in TrajectoryTracer::enqueue_movement : The previous insertion element was not allocated.");

            //Emergency stop,
            //TODO EMERGENCY STOP
            TrajectoryTracer::emergency_stop();

            //Fail;
            return error;

        }

        //Compute the jerk for the previous movement;
        JerkPlanner::compute_jerk(current_movement, previous_movement);


        /*
         * TODO JERK_PROPAGATION
         *
         */

        //If we currently are executing the previous movement, we need to update the movement environment manually.
        if (movement_data_queue.available_objects() == 1) {

            update_movement_environment();

        }


    }

    //Anyway, we must save the current movement's ending jerk ratios;
    JerkPlanner::save_final_jerk_offsets(current_movement);


    //---------------Persisting in Queue-----------------

    //Enqueue the movement.

    //Reset the flag;
    queue_flag = false;

    //Insert the movement;
    movement_data_queue.insert(&queue_flag);

    //Integrity check :
    if (!queue_flag) {

        //Log;
        CI::echo("Error in TrajectoryTracer::enqueue_movement : the insertion element was already allocated.");

        //Emergency stop;
        //TODO EMERGENCY STOP
        TrajectoryTracer::emergency_stop();

        //Fail;
        return error;

    }

    return complete;

}


void TrajectoryTracer::discard_movement() {

    debug("Discarding  : size  : " + str(movement_data_queue.available_objects()) + " nb_spaces " +
          String(movement_data_queue.available_spaces()));

    //Reset the flag;
    bool queue_flag = false;

    //leave a space for a future movement_data;
    movement_data_queue.discard(&queue_flag);

    //Integrity check :
    if (!queue_flag) {

        //Log;
        CI::echo("Error in TrajectoryTracer::discard_movement : "
                         "The discarded element was not allocated.");

        //Emergency stop;
        //TODO EMERGENCY STOP
        emergency_stop();

        //Fail;
        return;

    }

    CI::echo("\n----------------------------------\n\nDISCARD\n\n-----------------------------\n");

    debug("Discarded  : size  : " + str(movement_data_queue.available_objects()) + " nb_spaces " +
          String(movement_data_queue.available_spaces()));

}



//------------------------------------------------ Movement Processing -------------------------------------------------


/*
 * process_next_movement : this function pops a movement from the movement queue, and initialises :
 *      - the real time processor;
 *      - the movement;
 */

void TrajectoryTracer::process_next_movement() {

    if (movement_data_queue.available_objects()) {

        //declare a flag;
        bool queue_flag = false;

        //Pull the next movement;
        movement_data_t *movement_data = movement_data_queue.get_reading_address(&queue_flag);

        //Integrity check :
        if (!queue_flag) {

            //Log;
            CI::echo("Error in TrajectoryTracer::process_next_movement : The reading element was not allocated.");

            //Emergency stop;
            //TODO EMERGENCY STOP
            TrajectoryTracer::emergency_stop();

            //Fail;
            return;

        }

        //Initialise the new movement;
        (*(movement_data->movement_initialisation))();

        //update the finalisation function;
        movement_finalisation = movement_data->movement_finalisation;

        //Reset the flag;
        queue_flag = false;

        //Update the tools index;
        next_tools_powers_index = movement_data_queue.get_reading_index(&queue_flag);

        //Integrity check :
        if (!queue_flag) {

            //Log;
            CI::echo(
                    "Error in TrajectoryTracer::process_next_movement : The reading element at the reading index was not allocated.");

            //Emergency stop;
            //TODO EMERGENCY STOP
            TrajectoryTracer::emergency_stop();

            //Fail;
            return;

        }

        //Enable the future movement switch;
        movement_switch_flag = true;

        //Trigger the kernel's movement update, and get the number of sub_movements to wait before switching;
        movement_switch_counter = SubMovementManager::update_current_movement(movement_data);

        //Update now the pre_processing data, the real_time will be in the function below;
        Kinematics::load_pre_process_kinetics_data(movement_data);

        //Don't discard_sub_movement the movement struct for instance, it will be done in update_real_time_jerk_environment;

    }

}


/*
 * update_movement_environment : this function actualises the jerk and tools environment
 *  for the current movement.
 *
 *  It starts by reading the movement queue, and then it updates the environment accordingly.
 */

void TrajectoryTracer::update_movement_environment() {

    //declare a flag;
    bool queue_flag = false;

    //Read the current movement (readonly mode)
    const movement_data_t *movement_data = movement_data_queue.get_reading_address(&queue_flag);

    //Integrity check :
    if (!queue_flag) {

        //Log
        CI::echo("Error in TrajectoryTracer::update_movement_environment : The reading element was not allocated.");

        //Emergency stop;
        //TODO EMERGENCY STOP
        emergency_stop();

        //Fail;
        return;

    }

    //------Tools------

    //Update tool_environment;
    update_tools_data(movement_data);


    //------Jerk------

    //Update Jerk positions;
    SubMovementManager::update_jerk_position(movement_data->jerk_position);

    //Update the jerk environment;
    Kinematics::load_real_time_kinetics_data(movement_data);

    //------Clean------

    //Disable the movement_data switch;
    movement_switch_flag = false;

}


//----------------------------------------SUB_MOVEMENT_PRE_COMPUTATION--------------------------------------------------


void TrajectoryTracer::prepare_first_sub_movement() {

    CI::echo("PREPARING FIRST");

    //Push the first sub_movement (increment pre-processed to be correct);
    SubMovementManager::push_new_sub_movement();

    //Step 1 : Get a new position to reach;
    sub_movement_data_t *sub_movement_data = SubMovementManager::read_next_sub_movement();

    //Initialise the step_distances array we will give to the kernel;
    uint8_t *sub_movement_distances = sub_movement_data->step_distances;

    //Copy the direction signature in cache;
    next_direction_signature = sub_movement_data->direction_signature;


    //Step 2 : Update the end_distances with this step_distances array and compute the heuristic step_distances to jerk/end points;
    SubMovementManager::update_end_jerk_distances(next_direction_signature, sub_movement_distances);

    //-------------------Kinematics call-------------------

    //Give the hand to the kernel who will compute the sub_movement_time_us for the sub-movement;
    float sub_movement_time_us = Kinematics::compute_us_time_for_first_sub_movement(sub_movement_data);


    //Compute the signatures for the next movement;
    process_signatures(sub_movement_distances, es0);
    is_es_0 = false;

    //Determine the delay_us sub_movement_time_us for the next sub_movement :
    delay_us = (sub_movement_time_us) / (float) trajectory_index;


    //Discard the current sub_movement in the sub-movements queue;
    SubMovementManager::discard_sub_movement();

    //Push as much sub_movements as possible;
    SubMovementManager::fill_sub_movement_queue();

}


void TrajectoryTracer::prepare_next_sub_movement() {

    //Disable the stepper interrupt for preventing infinite call (causes stack overflow);
    disable_stepper_interrupt();

    //-------------------Initialisation-------------------

    //update the current movement data, and get the correct signature container for the current sub_movement;
    //3us 4 steppers, 8us 17 stepper : 1.5 us + 0.37us per stepper
    sig_t *step_signatures = initialise_sub_movement();

    //Step 1 : Get a new position to reach
    sub_movement_data_t *sub_movement_data = SubMovementManager::read_next_sub_movement();

    //Wait for the next timer overflow;
    STEP_AND_WAIT

    //Initialise the step_distances array we will give to the kernel;
    uint8_t *elementary_distances = sub_movement_data->step_distances;

    //Copy the direction signature in cache;
    next_direction_signature = sub_movement_data->direction_signature;

    //Step 2 : Update the end_distances with this step_distances array and compute the heuristic step_distances to jerk/end points;
    SubMovementManager::update_end_jerk_distances(next_direction_signature, elementary_distances);

    //Wait for the next timer overflow;
    STEP_AND_WAIT


    //-------------------Kinematics call-------------------

    //Give the hand to the kernel who will compute the time_us for the sub-movement;
    float time_us = Kinematics::compute_us_time_for_sub_movement(sub_movement_data);


    //Wait for the next timer overflow;
    STEP_AND_WAIT


    //-------------------Signature extraction-------------------

    //Compute the step_signatures corresponding to the step_distances given by the kernel;
    process_signatures(elementary_distances, step_signatures);

    //Determine the delay_us time_us for the next sub_movement;
    delay_us = time_us / (float) trajectory_index;

    //Discard the current sub_movement in the sub-movements queue;
    SubMovementManager::discard_sub_movement();

    //If no more pre-process is required :
    if (SubMovementManager::is_movement_processed()) {

        //Go to the end
        goto exit;

    }

    //Wait for the next timer overflow;
    STEP_AND_WAIT


    //-------------------Sub-movements pre-computation-------------------

    //Process a first movement for the one we made;
    SubMovementManager::push_new_sub_movement();

    //If no more pre-process is required;
    if (SubMovementManager::is_movement_processed()) {
        goto exit;
    }

    //Wait for the next timer overflow;
    STEP_AND_WAIT

    //Process a second movement, to fill the queue if needed;
    SubMovementManager::push_new_sub_movement();


    //-------------------Exit-------------------

    exit:

    //Set the light interrupt function to give time_us to background processes;
    set_stepper_int_function(finish_sub_movement);

    //Re-enable the stepper interrupt :
    if (!emergency_stop_flag) {
        enable_stepper_interrupt();
    }

}


/*
 * initialise_sub_movement : sets the delay_us, the trajectory index, the steppers directions
 *      and returns the pointer to the elementary signatures processed before
 */

sig_t *TrajectoryTracer::initialise_sub_movement() {

    //If the sub_movement is the first of a new movement :
    if (movement_switch_flag) {

        //If we must now change the movement environment :
        if (!(movement_switch_counter--)) {

            //Update the movement environment;
            update_movement_environment();

        }
    }

    //Update the trajectory index;
    saved_trajectory_index = trajectory_index;

    //Set the correct direction;
    StepperController::set_directions(next_direction_signature);

    //update the interrupt period with the float delay_us, that is computed to provide the most accurate period;
    set_stepper_int_period(delay_us);

    //save the motion scheme computed previously, so that new values won't erase the current ones.

    //If the current array is es_0 :
    if (is_es_0) {

        //It becomes es_1, and the flag is updated accordingly;
        saved_elementary_signatures = es1, is_es_0 = false;

        //return the an_allocated array;
        return es0;

    } else {
        //If the current array is es_1 :

        //It becomes es_0, and the flag is updated accordingly;
        saved_elementary_signatures = es0, is_es_0 = true;

        //return the an_allocated array;
        return es1;

    }

}


/*
 * process_signatures : this function pre_processes a plan_movement that will be executed later.
 *
 *  It determines the signatures of the movement.
 *
 *  More details about signatures meaning and the elementary movement algorithm can be found in TRACER's documentation.
 */

//TODO COMMENT !!!!!!!!!!!!!!!!

void TrajectoryTracer::process_signatures(uint8_t *const elementary_dists, sig_t *elementary_signatures) {
    uint8_t motion_depth = 0;
    sig_t pre_signatures[8];

    while (true) {
        sig_t sig = 0;

        //Get signature for current enqueue_movement

#define STEPPER(i, signature, ...) if (*(elementary_dists+i) & (uint8_t) 1) { sig |= signature; }

#include <config.h>

#undef STEPPER

        //If a enqueue_movement is required to branch :
        pre_signatures[motion_depth] = sig;
        motion_depth++;

        bool end_move = true;

        //Step 2 : shift right and check if last enqueue_movement is reached
#define STEPPER(i, signature, ...) if ((*(elementary_dists+i) >>= 1)) { end_move = false; }

#include  <config.h>

#undef STEPPER

        if (end_move) {//if next_move is null
            trajectory_index = trajectory_beginning_indices[motion_depth - 1];
            break;
        }
    }

    //Writing axis_signatures in the correct order
    int i = 0;
    for (; motion_depth--;) {
        elementary_signatures[i++] = pre_signatures[motion_depth];
    }

}


//----------------------------------------------Movement_ending---------------------------------------------------------
int k2_position_indice = 4;

/*
 *
 * MOVEMENT SWITCHING PROCEDURE :
 *  After a sub movement :
 *      - if the last position of the current movement hasn't been processed, then start the sub_movement interrupt
 *      - if the last position has been processed, then load a new movement
 *
 *  This procedure comprises the following steps :
 *      - change the trajectory function;
 *      - change the trajectory variables (beginning, ending, index)
 *      - init the new movement
 *
 *
 *
 * finish_sub_movement : this function is called repeatedly when the next sub_movement has been fully planned.
 *
 *  The only thing it does is stepping steppers according to the current sub_movement signatures, and end
 *      to leave time for background tasks.
 *
 *  If the current sub_movement is finished, then it programs the processing of the next sub_movement.
 *
 */

void TrajectoryTracer::finish_sub_movement() {

    //Disable the stepper interrupt for preventing infinite call (causes stack overflow);
    disable_stepper_interrupt();

    //Get the correct signature;
    StepperController::fastStep(saved_elementary_signatures[trajectory_array[saved_trajectory_index]]);

    //If the current sub_movement is finished :
    if (!saved_trajectory_index--) {

        //If the movement procedure's stop has been programmed :
        if (stop_programmed) {

            if (final_sub_movement_started) {
                //if the final sub_movement is now executed :

                //Send the position;
                Kinematics::send_position();

                //Stop the routine;
                stop();

                return;

            } else if (movement_data_queue.available_objects()) {
                //If another movement has been pushed just after stop_programmed was set (rare case) :

                //Process the next movement;
                process_next_movement();

                //Unschedule the stop
                stop_programmed = false;

                //Remove the final sub_movement flag;
                final_sub_movement_started = false;

            } else if (SubMovementManager::available_sub_movements() == 0) {
                //if the last position has just been popped :

                //engage the last sub_movement;
                initialise_sub_movement();

                //Mark the final sub_movement;
                final_sub_movement_started = true;

                //re-interrupt on this function, as no more process is required;
                if (!emergency_stop_flag) {
                    enable_stepper_interrupt();
                }

                //Complete;
                return;

            } else if (SubMovementManager::available_sub_movements() == 1) {
                movement_queue_lock_flag = true;
            }


        } else if (SubMovementManager::is_movement_processed()) {
            //If the movement pre-processing is finished :

            //Discard the current movement, that is now entirely processed;
            discard_movement();

            //finalise the current movement;
            (*movement_finalisation)();

            if (movement_data_queue.available_objects()) {
                //If another movement can be loaded :

                //Process the next movement;
                process_next_movement();

            } else {
                //If no more movement have been planned :

                //Plan the stop of the movement procedure.
                stop_programmed = true;
            }
        }


        //interrupt on the normal routine
        set_stepper_int_function(prepare_next_sub_movement);

    }

    if (!emergency_stop_flag) {
        enable_stepper_interrupt();
    }
}


//--------------------------------------------------------TOOLS---------------------------------------------------------


/*
 * update_tools_data : this function changes tools linear speeds, at an effective movement switch.
 *
 *  It stops tools that are not used, and  updates the number of tools, speeds, and update function,
 *
 */

void TrajectoryTracer::update_tools_data(const movement_data_t *movement) {

    //Cache for the tools signature;
    sig_t next_tools_signature = movement->tools_signatures;

    //determine the tools that must be stopped : those are present in the current signature and not in the next one;
    sig_t stop_signature = current_tools_signature & (~next_tools_signature);

    //Stop these tools;
    MachineInterface::stop_tools(stop_signature);

    //Update the tool number and the update functions;
    tools_nb = MachineInterface::set_tools_updating_function(next_tools_signature, tools_update_functions);

    //update linear powers;
    tools_linear_powers = tools_linear_powers_storage + NB_CONTINUOUS * next_tools_powers_index;

    //update the tools signature;
    current_tools_signature = next_tools_signature;
}


/*
 * stop_tools : this function stops the currently enabled tools.
 */

void TrajectoryTracer::stop_tools() {

    //Update each tool power with the value 'regulation_speed * linear_power';
    for (uint8_t action = 0; action < tools_nb; action++) {
        (*tools_update_functions[action])(0);
    }

    current_tools_signature = 0;
}


/*
 * update_tools_powers : this function updates the number of tools, and the action_update functions.
 */

void TrajectoryTracer::update_tools_powers(float speed) {

    //Update each tool power with the value 'regulation_speed * linear_power';
    for (uint8_t action = 0; action < tools_nb; action++) {
        (*tools_update_functions[action])(tools_linear_powers[action] * speed);
    }

}


//-----------------------------------------Static declarations - definitions--------------------------------------------

//TODO COMMENT
#define m TrajectoryTracer

//Acceleration Fields

volatile bool m::started = false;

//
volatile bool m::emergency_stop_flag = false;


bool m::stop_programmed = false;
bool m::final_sub_movement_started = false;
bool m::movement_queue_lock_flag = false;


Queue<movement_data_t> m::movement_data_queue(MOVEMENT_DATA_QUEUE_SIZE);

uint8_t ctraj[255] = {
        0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5,
        0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6,
        0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5,
        0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 7,
        0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5,
        0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6,
        0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5,
        0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0};

const uint8_t *const m::trajectory_array = ctraj;

sig_t tces0[8], tces1[8];
sig_t *const m::es0 = tces0, *const m::es1 = tces1;
bool m::is_es_0;

const uint8_t ctti[8]{0, 2, 6, 14, 30, 62, 126, 254};
const uint8_t *const m::trajectory_beginning_indices = ctti;

sig_t *m::saved_elementary_signatures = tces0;
uint8_t m::saved_trajectory_index;
uint8_t m::trajectory_index;

sig_t m::next_direction_signature;

void (*m::movement_finalisation)();

float m::delay_us = 0;

uint8_t m::tools_nb;

void (*k2tf[NB_CONTINUOUS]);

void (**m::tools_update_functions)(float) = (void (**)(float)) k2tf;

float t_a_ls[NB_CONTINUOUS * MOVEMENT_DATA_QUEUE_SIZE];
float *const m::tools_linear_powers_storage = t_a_ls;

float *m::tools_linear_powers;

uint8_t m::next_tools_powers_index = 0;
sig_t m::current_tools_signature = 0;

bool m::movement_switch_flag = false;
uint8_t m::movement_switch_counter = 0;


#undef m
#endif