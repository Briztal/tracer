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

#include "../../config.h"
#ifdef ENABLE_STEPPER_CONTROL

#include <stdint.h>
#include "ComplexTrajectoryExecuter.h"
#include "../StepperController.h"
#include "../../Actions/ContinuousActions.h"
#include "../MovementExecuter.h"
#include "PreProcessor.h"

/*
 * TODOs for a correct motion setup :
 *      set end distances
 *      set first elementary_distances, calling ComplexTrajectoryExecuter::fill_movement_data(true,  ... );
 *      set last  elementary_distances, calling ComplexTrajectoryExecuter::push_first_sub_movement(false, ... );w
 *      set speed data, calling SpeedPlanner::pre_set_speed_axis(...);
 *      set processing functions, calling ComplexTrajectoryExecuter::fill_processors(...);
 *      call ComplexTrajectoryExecuter::enqueue_movement_data
 *
 */

//------------------------------------------------movement_queue_management---------------------------------------------

/*
 * push_first_sub_movement : this function hashes the first sub_movement of the future motion, and adds the resulting data
 *      to the future movement data
 */

void ComplexTrajectoryExecuter::push_first_sub_movement(uint8_t *elementary_dists, sig_t nsig) {
    motion_data_to_fill.initial_dir_signature = nsig;

    disable_stepper_interrupt()

    update_end_distances(nsig, elementary_dists);
    enable_stepper_interrupt()

    //TODO CHECK IF THE INDICE IS CORRECTLY WRITTEN
    process_signatures(elementary_dists, motion_data_to_fill.initial_signatures, &(motion_data_to_fill.initial_indice));

}

void ComplexTrajectoryExecuter::fill_processors(void (*init_f)(), sig_t (*position_f)(float, float *),
                                                void (*speed_f)()) {
    motion_data_to_fill.init_processor = init_f;
    motion_data_to_fill.position_processor = position_f;
    motion_data_to_fill.speed_processor = speed_f;
}

void ComplexTrajectoryExecuter::enqueue_movement_data() {
    motion_data_queue.push(motion_data_to_fill);
    MovementExecuter::enqueue_trajectory_movement();
}

complex_motion_data *ComplexTrajectoryExecuter::peak_last_motion_data() {
    return motion_data_queue.peak_pushed();
}

/*
 * The movement initialisation function :
 *  - if possible, starts a movement;
 *  - if not, returns, and will re-check later (interrupt)
 */

void ComplexTrajectoryExecuter::start_movement() {
    in_motion = true;

    popped_data = motion_data_queue.pull();

    /*
     * Motion initialisation - according to the pulled motion data, set :
     *
     *      Motion data :
     * - count : number of sub_movements;
     * - axis_signatures = the first elementary movement axis_signatures (pre_processed);
     * - get_new_position, update_speed, functions used during movement initialise (depend on calling class);
     * - step : the function used to step (depends on the calling class;
     *
     *      Speed data :
     * - delay_numerator : the fraction numerator in delay processing;
     * - regulation_delay : the delay to reach, in speed regulation process
     * - ratio : the ratio speed_axis_dist(sqrt(sum(axis_dists^2)). Used.
     */

    /*
     * Count doesn't take its maximum value,
     *    because the pre_computation doesn't need to happen during the penultimate and the ultimate sub_movements.
     */

    PreProcessor::get_new_position = popped_data.position_processor;

    //Copy all axis_signatures in es0, and set is_es_0 so that es0 will be saved at the beginning of "prepare_next_sub_movement"
    es0[0] = popped_data.initial_signatures[0];
    es0[1] = popped_data.initial_signatures[1];
    es0[2] = popped_data.initial_signatures[2];
    es0[3] = popped_data.initial_signatures[3];
    es0[4] = popped_data.initial_signatures[4];
    es0[5] = popped_data.initial_signatures[5];
    es0[6] = popped_data.initial_signatures[6];
    es0[7] = popped_data.initial_signatures[7];
    is_es_0 = false;

    StepperController::enable(65535);
    StepperController::set_directions(popped_data.initial_dir_signature);

    trajectory_indice = popped_data.initial_indice;

    (*popped_data.init_processor)();

    //Set number of tools in continuous modes, and set action functions related
    PreProcessor::updateActions();

    set_stepper_int_function(prepare_next_sub_movement);

    enable_stepper_interrupt();

}


//----------------------------------------SUB_MOVEMENT_PRE_COMPUTATION--------------------------------------------------


/*
 * prepare_next_sub_movement : This function prepares the next sub_movement.
 *
 *  Steps to enqueue a sub movement :
 *      - Get the next distances > task given to the subclass
 *      - check if obtained distances are long enough
 *      - If true, enqueue the distance array
 *      - if not, discard the distance array and start_movement with a new increment
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
 *
 */

void ComplexTrajectoryExecuter::prepare_next_sub_movement() {

    //Disable the stepper interrupt for preventing infinite call (causes stack overflow)
    disable_stepper_interrupt();

    uint8_t elementary_dists[NB_STEPPERS];

    //Step 0 : update signatures for the current move;
    sig_t *elementary_signatures = initialise_sub_movement();

    sig_t negative_signatures = 0;
    float distance = 0;

    //Step 1 : Get a new position to reach
    PreProcessor::pop_next_position(elementary_dists, &negative_signatures, &distance);

    StepperController::set_directions(negative_signatures);

    STEP_AND_WAIT

    //Step 2 : Update the end_distances with this distances array and compute the heuristic distances to jerk/end points
    update_end_distances(negative_signatures, elementary_dists);

    STEP_AND_WAIT;

    //Step 3 : Update the speed distance with the new heuristic distances
    float time = PreProcessor::pre_process_speed(distance, elementary_dists);

    STEP_AND_WAIT;

    //Step 3 : Update the speed distance with the new heuristic distances
    PreProcessor::update_speeds(elementary_dists, time);

    STEP_AND_WAIT;

    //Step 5 : Extract signatures from this distances array
    process_signatures(elementary_dists, elementary_signatures, &trajectory_indice);

    //Step 6 : determine the dela time for the next sub_movement :
    delay = (uint32_t) (time) / (uint32_t) trajectory_indice;

    //If no more pre-process is required
    if (PreProcessor::last_position_popped) {
        goto end;
    }

    STEP_AND_WAIT;

    //Step 7 : get a new position
    PreProcessor::push_new_position();

    STEP_AND_WAIT;

    //Step 8 : if the position queue is not full, get a new position;
    PreProcessor::push_new_position();


    //Final steps :
    end :

    //Set the light interrupt function to give time to background processes
    set_stepper_int_function(finish_sub_movement);

    //Re-enable the stepper interrupt
    enable_stepper_interrupt();

}

/*
 * initialise_sub_movement : sets the delay, the trajectory indice, a
 *      nd returns the pointer to the elementary signatures processed before
 */
sig_t *ComplexTrajectoryExecuter::initialise_sub_movement() {
    saved_trajectory_indice = trajectory_indice;

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
 * update_end_distances : this function updates distances to end point and jerk point.
 *
 *  It takes as arguments the absolute distances on each axis of the next sub_movement, and their negative signature.
 *
 *      Reminder : the i_th bit of negative_signature is 1 if the distance on the i_th axis is negative.
 *
 *  The global distance is given by the maximum of all distance on each axis.
 *
 */

void ComplexTrajectoryExecuter::update_end_distances(const sig_t negative_signatures, const uint8_t *elementary_dists) {

#define STEPPER(i, sig, ...)\
    if (negative_signatures&sig) {SpeedManager::end_distances[i] += elementary_dists[i];SpeedManager::jerk_distances[i] += elementary_dists[i];}\
    else {SpeedManager::end_distances[i] -= elementary_dists[i];SpeedManager::jerk_distances[i] -= elementary_dists[i];}\


#include "../../config.h"

#undef STEPPER

    PreProcessor::heuristic_end_distance();
    PreProcessor::heuristic_jerk_distance();

}


/*
 * process_signatures : this function pre_processes a move that will be executed later.
 *
 *  It determines the signatures of the movement.
 *
 *  More details about signatures meaning and the elementary movement algorithm can be found in TRACER's documentation.
 *
 */

void ComplexTrajectoryExecuter::process_signatures(uint8_t *const elementary_dists, sig_t *elementary_signatures,
                                                   uint8_t *trajectory_indice) {
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
            *trajectory_indice = trajectory_indices[motion_depth - 1];
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
int i = 4;

/*
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
    while (!(s_w_signature = saved_elementary_signatures[trajectory_array[saved_trajectory_indice]])) {
        saved_trajectory_indice--;
    }

    StepperController::fastStep(s_w_signature);

    //If the current sub_movement is finished
    if (!saved_trajectory_indice--) {

        //Position log
#ifdef position_log
        if (!(i--)) {
            send_position();//TODO
            i = 20;
        }
#endif

        if (PreProcessor::last_position_popped) {
            //If the no more pre_process is required :

            if (PreProcessor::last_sub_move_engaged) {
                //if the last movement has already been done : finish

                SpeedPlanner::send_position();
                set_stepper_int_function(MovementExecuter::process_next_move);
                PreProcessor::reset_vars();

            } else {
                //If last move hasn't been engaged : engage last move
                PreProcessor::last_sub_move_engaged = true;

                in_motion = false;
                saved_trajectory_indice = trajectory_indice;
                saved_elementary_signatures = (is_es_0) ? es1 : es0;

            }
        }
    }
    enable_stepper_interrupt();
}




//---------------------------------------------DECLARATIONS_DEFINITIONS-------------------------------------------------


#define m ComplexTrajectoryExecuter

//Acceleration Fields

bool m::in_motion = false;

Queue<complex_motion_data> m::motion_data_queue(MOTION_DATA_QUEUE_SIZE);
complex_motion_data m::motion_data_to_fill;
complex_motion_data m::popped_data;

sig_t *saved_elementary_signatures;
uint8_t saved_trajectory_indice;

uint8_t traj[255] = {
        0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5,
        0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6,
        0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5,
        0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 7,
        0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5,
        0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6,
        0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5,
        0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0};

uint8_t *const m::trajectory_array = traj;

sig_t tes0[8], tes1[8];
sig_t *const m::es0 = tes0, *const m::es1 = tes1;
bool m::is_es_0;

const uint8_t tti[8]{0, 2, 6, 14, 30, 62, 126, 254};
const uint8_t *const m::trajectory_indices = tti;

sig_t *m::saved_elementary_signatures = tes0;
uint8_t m::saved_trajectory_indice;
uint8_t m::trajectory_indice;


#undef m
#endif