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

#include "ComplexTrajectoryExecuter.h"
#include "../../Interfaces/TreeInterface/TreeInterface.h"
#include "../StepperController.h"
#include "../LinearMovement/LinearMovement.h"
#include "../SpeedPlanner.h"
#include "../../Actions/ContinuousActions.h"
#include "../MovementExecuter.h"
#include "../SpeedManager.h"
#include "../StepperAbstraction.h"

/*
 * TODOs for a correct motion setup :
 *      set end distances
 *      set first elementary_distances, calling ComplexTrajectoryExecuter::fill_movement_data(true,  ... );
 *      set last  elementary_distances, calling ComplexTrajectoryExecuter::fill_movement_data(false, ... );w
 *      set speed data, calling SpeedPlanner::pre_set_speed_axis(...);
 *      set processing functions, calling ComplexTrajectoryExecuter::fill_processors(...);
 *      call ComplexTrajectoryExecuter::enqueue_movement_data
 *
 */


//------------------------------------------------movement_queue_management---------------------------------------------

/*
 * fill_movement_data : this function hashes the first sub_movement of the future motion, and adds the resulting data
 *      to the future movement data
 */


void ComplexTrajectoryExecuter::fill_movement_data(bool first, uint8_t *elementary_dists, uint32_t count, sig_t nsig) {
    if (first) {
        motion_data_to_fill.count = count;
        motion_data_to_fill.initial_dir_signature = nsig;
    } else {
        motion_data_to_fill.final_dir_signature = nsig;
    }

    disable_stepper_interrupt()

#define STEPPER(i, sig, ...)\
    if (nsig&sig) SpeedManager::end_distances[i] += elementary_dists[i];\
    else SpeedManager::end_distances[i] -= elementary_dists[i];

#include "../../config.h"

#undef STEPPER

#define STEPPER(i, sig, ...)\
    if (nsig&sig) SpeedManager::jerk_distances[i] += elementary_dists[i];\
    else SpeedManager::jerk_distances[i] -= elementary_dists[i];

#include "../../config.h"

#undef STEPPER

    enable_stepper_interrupt()

    //uint8_t local_array_end = 0;
    uint8_t motion_depth = 0;
    sig_t pre_signatures[8];
    while (true) {
        sig_t sig = 0;

        //Step 1 : get signature for current enqueue_movement

#define STEPPER(i, signature, ...) if (*(elementary_dists+i) & (uint8_t) 1) { sig |= signature; }

#include "../../config.h"

#undef STEPPER

        //If a enqueue_movement is required to branch :
        pre_signatures[motion_depth] = sig;
        motion_depth++;

        bool end_move = true;

        //Step 2 : shift right and check if last enqueue_movement is reached
#define STEPPER(i, sig, ...) if ((*(elementary_dists+i) >>= 1)) { end_move = false; }

#include "../../config.h"

#undef STEPPER

        if (end_move) {//if next_move is null
            if (first) {
                motion_data_to_fill.initial_indice = trajectory_indices[motion_depth - 1];
            } else {
                motion_data_to_fill.final_indice = trajectory_indices[motion_depth - 1];
            }
            break;
        }
    }

    //Writing axis_signatures in the correct order
    int i = 0;
    if (first) {
        for (; motion_depth--;) {
            motion_data_to_fill.initial_signatures[i++] = pre_signatures[motion_depth];
        }
    } else {
        for (; motion_depth--;) {
            motion_data_to_fill.final_signatures[i++] = pre_signatures[motion_depth];
        }
    }
}


void ComplexTrajectoryExecuter::fill_speed_data(delay_t delay_numerator, delay_t regulation_delay, float speed_factor,
                                                float ratio, uint8_t processing_steps) {

    motion_data_to_fill.processing_steps = processing_steps;
    motion_data_to_fill.delay_numerator = delay_numerator;
    motion_data_to_fill.speed_factor = speed_factor;
    motion_data_to_fill.regulation_delay = regulation_delay;
    motion_data_to_fill.ratio = ratio;
    motion_data_to_fill.jerk_point = false;
    motion_data_to_fill.jerk_distance_offset = 0;
}

void ComplexTrajectoryExecuter::fill_processors(void (*init_f)(), void (*step_f)(sig_t), sig_t (*position_f)(uint8_t *),
                                                void (*speed_f)()) {
    motion_data_to_fill.init_processor = init_f;
    motion_data_to_fill.step = step_f;
    motion_data_to_fill.position_processor = position_f;
    motion_data_to_fill.speed_processor = speed_f;
}

void ComplexTrajectoryExecuter::enqueue_movement_data() {
    motion_data_queue.push(motion_data_to_fill);
    MovementExecuter::enqueue_trajectory_movement();
}

motion_data *ComplexTrajectoryExecuter::peak_last_motion_data() {
    return motion_data_queue.peak_pushed();
}

/*
 * The movement initialisation function :
 *  - if possible, starts a movement;
 *  - if not, returns, and will re-check later (interrupt)
 */
void ComplexTrajectoryExecuter::start() {
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

    count = (uint16_t) (popped_data.count - 2);

    /*
     * Count doesn't take its maximum value,
     *    because the pre_computation doesn't need to happen during the penultimate and the ultimate sub_movements.
     */

    get_new_position = popped_data.position_processor;
    update_speed = popped_data.speed_processor;
    step = popped_data.step;

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

    //Speed update according to pre_processed_parameters

    SpeedManager::init_speed_management(popped_data.regulation_delay, popped_data.delay_numerator,
                                        popped_data.speed_factor, popped_data.ratio, popped_data.processing_steps,
                                        popped_data.jerk_point, popped_data.jerk_distance_offset);

    (*popped_data.init_processor)();

    //Set number of tools in continuous modes, and set action functions related
    SpeedManager::updateActions();

    set_stepper_int_function(prepare_next_sub_movement);

    enable_stepper_interrupt();

}

void ComplexTrajectoryExecuter::set_last_sub_motion() {//TODO END POSITION
    //Copy all axis_signatures in es0, and set is_es_0 so that es0 will be saved at the beginning of "prepare_next_sub_movement"
    es0[0] = popped_data.final_signatures[0];
    es0[1] = popped_data.final_signatures[1];
    es0[2] = popped_data.final_signatures[2];
    es0[3] = popped_data.final_signatures[3];
    es0[4] = popped_data.final_signatures[4];
    es0[5] = popped_data.final_signatures[5];
    es0[6] = popped_data.final_signatures[6];
    es0[7] = popped_data.final_signatures[7];
    is_es_0 = false;

    StepperController::set_directions(popped_data.final_dir_signature);

    saved_trajectory_indice = popped_data.final_indice;
    saved_elementary_signatures = es0;
}


//-----------------------------------Intermediary_Positions_Pre_Computation---------------------------------------------


void ComplexTrajectoryExecuter::push_new_position() {


    //High level positions : the new position in the high level system (will be provided by get_new_positions).
    float high_level_positions[NB_AXIS];

    //Stepper positions : the new position in the steppers system (translated by StepperAbstraction).
    int32_t steppers_positions[NB_STEPPERS];


    //Stepper distances : As copying requires time, we directly put the temp distances in the final array.
    uint8_t push_indice = sub_movement_queue.push_indice();
    uint8_t *elementary_dists = sub_movement_distances + push_indice * NB_STEPPERS;

    //The maximal stepper distance, and the maximum axis
    uint8_t max_distance, max_axis;

    sig_t negative_signature;


    //------------------------------------------Distances_computing-----------------------------------------------------

    //Get the new index candidate;
    float index_candidate = index + increment;

    //Get the new high level position;
    get_new_position(index_candidate, high_level_positions);

    //Translate the high level position into steppers position;
    StepperAbstraction::translate(high_level_positions, steppers_positions);

    //Get the steppers distances, and the maximal axis and distance
    bool up_check = get_distances(current_stepper_positions, steppers_positions, elementary_dists,
                                  &negative_signature, &max_axis, &max_distance);


    //-----------------------------------------Validity_Verification----------------------------------------------------

    //If the maximal distance is below the lower limit :
    if (up_check) {
        increment = increment / 2;
        return;
    }

    //If the maximal distance is below the lower limit :
    if (max_distance <= MINIMUM_DISTANCE_LIMIT) {
        increment = 2 * increment;
        return;
    }


    if (max_distance == MAX_DISTANCE_TARGET) {
        //Increment adjustment according to the target
        increment = increment * (float) MAX_DISTANCE_TARGET / (float) max_distance;

        //TODO : if the queue is full, discard the current move
    }

    //-----------------------------------------Movement_Enqueuing----------------------------------------------------

    /*
     * Now that validity checks are made, we can enqueue the sub_movement :
     * As the distances are already in the array, all we need to do is to push the direction signature
     *      and validate the candidate index.
     */

    sub_movement_queue.push(negative_signature);

    index = index_candidate;

}


/*
 * get_distances : this function determines the distance between a position and a target, for all axis.
 *
 * It also computes the direction signature, as distances are positive numbers, and saves the maximum axis
 *      and the maximum distance.
 */

bool
ComplexTrajectoryExecuter::get_distances(const int32_t *const pos, const int32_t *const dest, const uint8_t *const dists, sig_t *dir_dignature_p,
                                         uint8_t *max_axis_p, uint8_t *max_distance_p) {

    //Cache variable, to avoid pointer access.
    uint8_t max_dist = 0;
    uint8_t max_axis = 0;
    sig_t dir_signature = 0;

    //We must determine the distance for every axis
    for (uint8_t axis = 0; axis<NB_STEPPERS; axis++) {

        //get distance on axis
        int32_t distance = pos[axis]-dest[axis];

        //get absolute distance
        if (distance < 0) {
            distance = -distance;
            dir_signature |= SpeedPlanner::axis_signatures[axis];
        }

        //if the distance is greater than the limit : fail with true
        if (distance>=MAXIMUM_DISTANCE_LIMIT) {
            return true;
        }

        //cast to small type
        uint8_t rdist = (uint8_t) distance;

        //Update max dist
        if (rdist>max_dist) {
            max_dist = rdist;
            max_axis = axis;
        }
    }

    //Finally update all data
    *max_axis_p = max_axis;
    *max_distance_p = max_dist;
    *dir_dignature_p = dir_signature;

    //No error
    return false;
}


sig_t ComplexTrajectoryExecuter::pop_next_position(uint8_t *elementary_dists) {
}


//----------------------------------------SUB_MOVEMENT_PRE_COMPUTATION--------------------------------------------------


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
 *
 */

void ComplexTrajectoryExecuter::prepare_next_sub_movement() {

    //Disable the stepper interrupt for preventing infinite call (causes stack overflow)
    disable_stepper_interrupt();

    uint8_t elementary_dists[NB_STEPPERS];

    //Step 0 : update signatures for the current move;
    sig_t *elementary_signatures = get_signatures_array();

    //Step 1 : Get a new position to reach
    sig_t negative_signatures = pop_next_position(elementary_dists);

    STEP_AND_WAIT

    //Step 2 : Update the end_distances with this distances array and compute the heuristic distances to jerk/end points
    update_end_distances(elementary_dists);

    STEP_AND_WAIT;

    //Step 3 : Update the speed distance with the new heuristic distances
    SpeedManager::regulate_speed();

    STEP_AND_WAIT;

    //Step 4 : Extract signatures from this distances array
    process_signatures(elementary_dists, elementary_signatures);

    STEP_AND_WAIT;

    //Step 5 : modify the speed of movement and actions, with the new speed distance
    update_speed_and_actions();

    //Step 6 : get a new position
    push_new_position();

    //Step 7 : if the position queue is not full, get a new position;
    push_new_position();

    //Set the light interrupt function to give time to background processes
    set_stepper_int_function(finish_sub_movement);

    //Re-enable the stepper interrupt
    enable_stepper_interrupt();

}

sig_t *ComplexTrajectoryExecuter::get_signatures_array() {
    saved_trajectory_indice = trajectory_indice;

    //save the motion scheme computed previously, so that new values won't erase the current ones
    if (is_es_0) {
        saved_elementary_signatures = es1, is_es_0 = false;
        return es0;
    } else {
        saved_elementary_signatures = es0, is_es_0 = true;
        return es1;
    }
}


void ComplexTrajectoryExecuter::update_end_distances(const uint8_t *elementary_dists) {

#define STEPPER(i, sig, ...)\
    if (negative_signatures&sig) {SpeedManager::end_distances[i] += elementary_dists[i];SpeedManager::jerk_distances[i] += elementary_dists[i];}\
    else {SpeedManager::end_distances[i] -= elementary_dists[i];SpeedManager::jerk_distances[i] -= elementary_dists[i];}\


#include "../../config.h"

#undef STEPPER

    SpeedManager::heuristic_end_distance();
    SpeedManager::heuristic_jerk_distance();

}

void
ComplexTrajectoryExecuter::process_signatures(uint8_t *const elementary_dists, sig_t *const elementary_signatures) {
    uint8_t motion_depth = 0;
    sig_t pre_signatures[8];

    while (true) {
        sig_t sig = 0;

        //Step 1 : get signature for current enqueue_movement

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


void ComplexTrajectoryExecuter::update_speed_and_actions() {
    if (SpeedManager::delay0_update_required) {

        SpeedManager::update_delay_0();

        //Speed Setting
        (*update_speed)();

        STEP_AND_WAIT;

        //Actions setting
        SpeedManager::setActionsSpeeds();

    }
}

//----------------------------------------------SPEED_MANAGEMENT--------------------------------------------------------
int i = 4;

void ComplexTrajectoryExecuter::finish_sub_movement() {
    disable_stepper_interrupt();

    sig_t s_w_signature;
    if (!(s_w_signature = saved_elementary_signatures[trajectory_array[saved_trajectory_indice]]))
        s_w_signature = saved_elementary_signatures[trajectory_array[--saved_trajectory_indice]];

    (*step)(s_w_signature);
    if (!saved_trajectory_indice--) {
        if (count) {
            count--;

#ifdef position_log
            if (!(i--)) {
                SpeedPlanner::send_position();
                i = 20;
            }
#endif
            set_stepper_int_function(prepare_next_sub_movement);
        } else {
            if (penultimate_movement) {
                in_motion = false;
                saved_trajectory_indice = trajectory_indice;
                saved_elementary_signatures = (is_es_0) ? es1 : es0;
                penultimate_movement = false;
            } else if (ultimate_movement) {
                set_last_sub_motion();
                ultimate_movement = false;
            } else {

                SpeedPlanner::send_position();
                set_stepper_int_function(MovementExecuter::process_next_move);
                ultimate_movement = penultimate_movement = true;
            }
        }
    }
    enable_stepper_interrupt();
}


#define m ComplexTrajectoryExecuter

//Acceleration Fields
uint32_t m::count;

bool m::in_motion = false;

Queue<motion_data> m::motion_data_queue(MOTION_DATA_QUEUE_SIZE);
motion_data m::motion_data_to_fill;
motion_data m::popped_data;

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

//Processors;
void (*m::step)(sig_t);

sig_t (*m::get_new_position)(uint8_t *);

void (*m::update_speed)();

bool m::ultimate_movement = true, m::penultimate_movement = true;

#undef m
#endif