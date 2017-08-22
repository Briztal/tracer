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

#include "ComplexTrajectoryExecuter.h"
#include "RealTimeProcessor.h"
#include <interface.h>
#include <MovementKernels/MachineAbstraction.h>
#include <Actions/ContinuousActions.h>



//------------------------------------------------movement_queue_management---------------------------------------------

/*
 * The function to start the movement routine. It will run in interrupts while movements are to trace, and then stop.
 *
 *  It first verifies that movement are enqueued,
 *
 *  If there are, it pops the first one, and initialises it.
 *
 *  It then prepare the movement procedure, and starts the movement routine.
 *
 */

void ComplexTrajectoryExecuter::start() {

    if (!movement_data_queue.available_elements()) {
        //If no movements are in the queue, no need to start.

        //send an error message
        CI::echo("No movements are present in the queue, starting aborted");

        return;
    }

    CI::echo("Movements procedure started.");

    RealTimeProcessor::start();

    //Initialise the end booleans
    stop_programmed = false;
    final_sub_movement_started = false;

    //Initialise the RealTimeProcessor for the first movement
    process_next_movement(true);

    //Set up the movement procedure
    prepare_first_sub_movement();

    //Setup properly the interrupt procedure;
    setup_stepper_interrupt(prepare_next_sub_movement, (uint32_t) delay);

    //Mark the movement procedure as started
    started = true;

    //Start the interrupt sequence.
    enable_stepper_interrupt();

}

/*
 * stop : this function stops the movement procedure, by simply disabling the interrupt.
 *
 *  After disabling the interrupt, it disables the timer, and set the state flag to disables.
 *
 *  If this function is called during a movement, it will be discarded, and won't continue at next start.
 *
 *  It can be called during the emergency procedure, to stop the machine from moving.
 *
 */

void ComplexTrajectoryExecuter::stop() {

    //Interrupt the movement routing, by stopping the interrupt sequence
    disable_stepper_interrupt()

    //Get the half of the delay period
    uint32_t half_delay = (uint32_t) (delay / 2);

    //Wait the half of the delay period
    delay_us(half_delay);

    //re-disable the movement routine, prevents the case where the previous disable occurred just before the interrupt.
    disable_stepper_interrupt()

    //Disable the stepper timer, now that the interrupt is disabled.
    disable_stepper_timer();

    //Stop all currently enabled tools;
    stop_tools();

    //Mark the movement routine as stopped
    started = false;

    //Enable the movement enqueuing
    queue_lock_flag = false;

}


/*
 * enqueue_unauthorised : this function is used by another process, to verify if the movement queue is not locked.
 *      It returns true if the queue is locked, and the enqueuing is not permitted.
 */

bool ComplexTrajectoryExecuter::enqueue_unauthorised() {
    return queue_lock_flag;
}


/*
 * enqueue_movement : this function adds the movement provided in argument to the motion queue.
 *
 *  The movement is provided in the form of :
 *      - min and max, the index variable minimum and maximum values, for (resp) the beginning and end positions
 *      - incr : the index increment for the first movement;
 *      - movement_initialisation : the movement's in_real_time initialisation function;
 *      - trajectory_function : the function that will be used to compute new positions in_real_time.
 *
 */

bool ComplexTrajectoryExecuter::enqueue_movement(float min, float max, float incr, void (*movement_initialisation)(),
                                                 void (*movement_finalisation)(),
                                                 void(*trajectory_function)(float, float *)) {

    if (queue_lock_flag) {

        //Send an error message.
        CI::echo("ERROR : THE MOVEMENT QUEUE WAS LOCKED WHEN THE MOVEMENT WAS PUSHED. THE MOVEMENT WILL BE IGNORED.");

        //Fail
        return false;

    }

    /* First, we must check if the movement is a micro movement. A movement is qualified by micro-movement when
     *      its first sub-movement goes beyond its limit. It is not actually traceable, so we must ignore it.
     */

    //Micro movement check :
    if (((incr>0) && (min + incr > max) || (incr<0) && (min + incr <max))) {

        //Send an error message
        CI::echo("ERROR : THE MOVEMENT PROVIDED IS A MICRO MOVEMENT, AND WILL BE IGNORED.");

        //ignore movement
        return false;

    }

    //Get the insertion adress on the queue (faster than push-by-object)
    k2_movement_data *d = movement_data_queue.get_push_ptr();

    //Update the current case of the queue :

    //Speed vars
    d->speed = MachineAbstraction::get_speed();
    d->speed_group = MachineAbstraction::get_speed_group();

    //Trajectory vars
    d->min = min;
    d->max = max;
    d->increment = incr;
    d->trajectory_function = trajectory_function;

    //Movement initialisation - finalisation
    d->movement_initialisation = movement_initialisation;
    d->movement_finalisation = movement_finalisation;

    //Get the current insertion position in the linear_powers storage
    float *tools_linear_powers = tools_linear_powers_storage + NB_CONTINUOUS * movement_data_queue.push_indice();

    //Get the action signature and fill the linear_powers storage.
    d->action_signatures = MachineAbstraction::get_tools_data(tools_linear_powers);

    //Push
    movement_data_queue.push();

    CI::echo("ENQUEUED : " + String(movement_data_queue.available_elements()));

    //Start the movement procedure if it is not already started.
    if (!started) {
        CI::echo("STARTING");
        start();
    }

    return true;

}


/*
 * process_next_movement : this function pops a movement from the movement queue, and initialises :
 *      - the real time processor;
 *      - the movement;
 */

void ComplexTrajectoryExecuter::process_next_movement(bool first_movement) {

    if (movement_data_queue.available_elements()) {

        //Pull the next movement
        k2_movement_data *d = movement_data_queue.peak();

        //Update the trajectory variables
        RealTimeProcessor::initialise_movement(d->min, d->max, d->increment, d->trajectory_function);

        //Initialise the new movement
        (*(d->movement_initialisation))();

        //update the finalisation function
        movement_finalisation = d->movement_finalisation;

        next_tools_powers_indice = movement_data_queue.pull_indice();
        next_tools_signature = d->action_signatures;

        //Update the speed according to the movement type
        if (first_movement) {
            RealTimeProcessor::set_regulation_speed(d->speed_group, d->speed);
        } else {
            RealTimeProcessor::set_regulation_speed_jerk(d->speed_group, d->speed);
        }

        //leave a space for a future movement.
        movement_data_queue.discard();

    }

}


/*
 * prepare_first_sub_movement : this function sets all variables up for the movement routine.
 *
 *  It processes the first sub_movement, and then fills the sub_movement queue;
 *
 */
void ComplexTrajectoryExecuter::prepare_first_sub_movement() {

    //Push the first position;
    RealTimeProcessor::push_new_position();

    //Process the first position :

    //start by initialising vars for processing
    uint8_t elementary_dists[NB_STEPPERS];
    float real_dists[NB_STEPPERS];
    sig_t negative_signature = 0;
    float distance = 0;

    //pop the stored position
    RealTimeProcessor::pop_next_position(elementary_dists, real_dists, &negative_signature, &distance);

    //Process the signatures for the next movement
    process_signatures(elementary_dists, es0);
    is_es_0 = false;

    //Update end distances with the computed distances.
    RealTimeProcessor::update_end_distances(negative_signature, elementary_dists);

    //update the speeds
    float time = RealTimeProcessor::pre_process_speed(distance, real_dists);

    RealTimeProcessor::update_speeds(real_dists, time);

    //detemine the first delay
    delay = (uint32_t) ((float) 1000000 * time) / (uint32_t) trajectory_indice;

    //save the the signature
    saved_direction_sigature = negative_signature;

    //Push as much sub_movements as possible.
    RealTimeProcessor::fill_sub_movement_queue();

    //update the linear_powers now.
    update_tools_data();

}

//----------------------------------------SUB_MOVEMENT_PRE_COMPUTATION--------------------------------------------------


//The delay macros, used in the function below.

#define WAIT\
    while(!stepper_int_flag) {}\
        stepper_int_flag_clear();


#define STEP_AND_WAIT \
    {\
        sig_t s_w_signature;\
        s_w_signature = saved_elementary_signatures[trajectory_array[saved_trajectory_indice--]];\
        StepperController::fastStep(s_w_signature);\
    }\
    WAIT


/*
 * prepare_next_sub_movement : This function prepares the next sub_movement.
 *
 *  Steps to enqueue a sub movement :
 *      - Get the next distances > task given to the subclass
 *      - check if obtained distances are long enough
 *      - If true, enqueue the distance array
 *      - if not, discard the distance array and start with a new increment
 *
 * IMPORTANT : END_DISTANCE PROCESSING
 *
 *  It processes the next steps :
 *      - Process the next distances
 *      - Adjust the speed
 *      - set the new speed values
 *      - get a new distance array
 *      - if queue not empty, get one more new distance array
 *
 */

void ComplexTrajectoryExecuter::prepare_next_sub_movement() {

    //Disable the stepper interrupt for preventing infinite call (causes stack overflow)
    disable_stepper_interrupt();

    uint8_t elementary_dists[NB_STEPPERS];
    float real_dists[NB_STEPPERS];

    //Step 0 : update signatures for the current prepare_movement;
    sig_t *elementary_signatures = initialise_sub_movement();

    sig_t negative_signatures = 0;
    float distance = 0;

    //Step 1 : Get a new position to reach
    RealTimeProcessor::pop_next_position(elementary_dists, real_dists, &negative_signatures, &distance);

    saved_direction_sigature = negative_signatures;

    STEP_AND_WAIT

    //Step 2 : Update the end_distances with this distances array and compute the heuristic distances to jerk/end points
    RealTimeProcessor::update_end_distances(negative_signatures, elementary_dists);

    STEP_AND_WAIT;

    //Step 3 : Extract signatures from this distances array
    process_signatures(elementary_dists, elementary_signatures);

    STEP_AND_WAIT;

    //Step 4 : Update the speed distance with the new heuristic distances
    float time = RealTimeProcessor::pre_process_speed(distance, real_dists);

    STEP_AND_WAIT;

    //Step 5 : Update the speed distance with the new heuristic distances
    RealTimeProcessor::update_speeds(real_dists, time);

    //Step 6 : determine the delay time for the next sub_movement :
    delay = ((float) 1000000 * time) / (uint32_t) trajectory_indice;

    //Update tools powers
    update_tools_powers(time, distance);

    //If no more pre-process is required
    if (RealTimeProcessor::movement_processed) {
        goto end;
    }

    STEP_AND_WAIT;

    //Step 7 : get a new position
    RealTimeProcessor::push_new_position();

    STEP_AND_WAIT;

    //Step 8 : if the position queue is not full, get a new position;
    RealTimeProcessor::push_new_position();

    STEP_AND_WAIT;

    //Final steps :
    end :

    //Set the light interrupt function to give time to background processes
    set_stepper_int_function(finish_sub_movement);

    //Re-enable the stepper interrupt
    enable_stepper_interrupt();

}


/*
 * initialise_sub_movement : sets the delay, the trajectory indice, the steppers directions
 *      and returns the pointer to the elementary signatures processed before
 */

sig_t *ComplexTrajectoryExecuter::initialise_sub_movement() {

    //Update the trajectory indice
    saved_trajectory_indice = trajectory_indice;

    //Set the correct direction
    StepperController::set_directions(saved_direction_sigature);

    //update the interrupt period with the float delay, that is computed to provde the most accurate period.
    set_stepper_int_period(delay);

    //save the motion scheme computed previously, so that new values won't erase the current ones
    if (is_es_0) {
        saved_elementary_signatures = es1, is_es_0 = false;
        return es0;
    } else {
        saved_elementary_signatures = es0, is_es_0 = true;
        return es1;
    }
}


/*
 * process_signatures : this function pre_processes a prepare_movement that will be executed later.
 *
 *  It determines the signatures of the movement.
 *
 *  More details about signatures meaning and the elementary movement algorithm can be found in TRACER's documentation.
 *
 */

void ComplexTrajectoryExecuter::process_signatures(uint8_t *const elementary_dists, sig_t *elementary_signatures) {
    uint8_t motion_depth = 0;
    sig_t pre_signatures[8];

    while (true) {
        sig_t sig = 0;

        //Get signature for current enqueue_movement

#define STEPPER(i, signature, ...) if (*(elementary_dists+i) & (uint8_t) 1) { sig |= signature; }

#include "../../config.h"

#undef STEPPER

        //If a enqueue_movement is required to branch :
        pre_signatures[motion_depth] = sig;
        motion_depth++;

        bool end_move = true;

        //Step 2 : shift right and check if last enqueue_movement is reached
#define STEPPER(i, signature, ...) if ((*(elementary_dists+i) >>= 1)) { end_move = false; }

#include "../../config.h"

#undef STEPPER

        if (end_move) {//if next_move is null
            trajectory_indice = trajectory_indices[motion_depth - 1];
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
 *      - change the trajectory variables (min, max, index)
 *      - initialise the new movement
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

void ComplexTrajectoryExecuter::finish_sub_movement() {

    //Disable the stepper interrupt for preventing infinite call (causes stack overflow)
    disable_stepper_interrupt();

    //Get the correct signature
    sig_t s_w_signature;

    s_w_signature = saved_elementary_signatures[trajectory_array[saved_trajectory_indice]];

    StepperController::fastStep(s_w_signature);

    //If the current sub_movement is finished
    if (!saved_trajectory_indice--) {

        //Position log
#ifdef position_log
        if (!(k2_position_indice--)) {
            RealTimeProcessor::send_position();
            k2_position_indice = 4;
        }
#endif

        if (stop_programmed) {
            //if (false) {

            if (movement_data_queue.available_elements()) {
                //If another movement has been pushed just after stop_programmed was set (rare case) :

                //Process the next movement
                process_next_movement(false);
                //if the routine will stop at the end of the current movement:

                stop_programmed = false;

                final_sub_movement_started = true;

            } else if (final_sub_movement_started) {
                //if the final sub_movement is now executed :

                RealTimeProcessor::send_position();

                //Stop the routine
                stop();

                CI::echo("STOPPED");
                return;

            } else if (RealTimeProcessor::empty_queue) {

                //if the last position has just been popped :

                //engage the last sub_movement
                initialise_sub_movement();

                //Mark the final sub_movement
                final_sub_movement_started = true;

                //re-interrupt on this function, as no more process is required
                enable_stepper_interrupt();

                return;

            } else if (RealTimeProcessor::elements() == 1) {
                queue_lock_flag = true;
            }


        } else if (RealTimeProcessor::movement_processed) {

            //If the movement pre-processing is finished :

            //finalise the current movement
            (*movement_finalisation)();

            if (movement_data_queue.available_elements()) {
                //If another movement can be loaded :

                movement_switch_flag = true;

                movement_switch_counter = RealTimeProcessor::elements();

                //Process the next movement
                process_next_movement(false);


            } else {
                //If no more movement have been planned :

                stop_programmed = true;
            }
        }

        //interrupt on the normal routine
        set_stepper_int_function(prepare_next_sub_movement);

    }

    enable_stepper_interrupt();
}


//--------------------------------------------------------TOOLS---------------------------------------------------------


/*
 * update_tools_data : this function changes tools linear speeds, at an effective movement switch.
 *
 *  It stops tools that are not used, and  updates the number of tools, speeds, and update function,
 *
 */

void ComplexTrajectoryExecuter::update_tools_data() {

    //determine the tools that must be stopped : those are present in the current signature and not in the next one.
    sig_t stop_signature = current_tools_signature & (~next_tools_signature);

    //Stop these tools
    MachineAbstraction::stop_tools(stop_signature);

    //Update the tool number and the update functions
    tools_nb = MachineAbstraction::set_tools_updating_function(next_tools_signature, tools_update_functions);

    //update linear powers
    tools_linear_powers = tools_linear_powers_storage + NB_CONTINUOUS * next_tools_powers_indice;

    //update the tools signature
    current_tools_signature = next_tools_signature;
}


/*
 * stop_tools : this function stops the currently enabled tools
 */

void ComplexTrajectoryExecuter::stop_tools() {

    //Update each tool power with the value 'speed * linear_power'
    for (uint8_t action = 0; action < tools_nb; action++) {
        (*tools_update_functions[action])(0);
    }

    current_tools_signature = 0;
}


/*
 * update_tools_powers : this function updates the number of tools, and the action_update functions
 */

void ComplexTrajectoryExecuter::update_tools_powers(float time, float distance) {

    if (movement_switch_flag) {
        if (!(movement_switch_counter--)) {
            movement_switch_flag = false;
            update_tools_data();
        }
    }

    //Determine the current speed
    float speed = distance / time;

    //Update each tool power with the value 'speed * linear_power'
    for (uint8_t action = 0; action < tools_nb; action++) {
        (*tools_update_functions[action])(tools_linear_powers[action] * speed);
    }

}


//-----------------------------------------Static declarations - definitions--------------------------------------------

#define m ComplexTrajectoryExecuter

//Acceleration Fields

bool m::started = false;


bool m::stop_programmed = false;
bool m::final_sub_movement_started = false;
bool m::queue_lock_flag = false;


Queue<k2_movement_data> m::movement_data_queue(MOTION_DATA_QUEUE_SIZE);

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
const uint8_t *const m::trajectory_indices = ctti;

sig_t *m::saved_elementary_signatures = tces0;
uint8_t m::saved_trajectory_indice;
uint8_t m::trajectory_indice;

sig_t m::saved_direction_sigature;

void (*m::movement_finalisation)();

float m::delay = 0;

uint8_t m::tools_nb;

void (*k2tf[NB_CONTINUOUS]);

void (**m::tools_update_functions)(float) = (void (**)(float)) k2tf;

float t_a_ls[NB_CONTINUOUS * MOTION_DATA_QUEUE_SIZE];
float *const m::tools_linear_powers_storage = t_a_ls;

float *m::tools_linear_powers;

uint8_t m::next_tools_powers_indice = 0;
sig_t m::current_tools_signature = 0;
sig_t m::next_tools_signature = 0;

bool m::movement_switch_flag = false;
uint8_t m::movement_switch_counter = 0;

#undef m
#endif