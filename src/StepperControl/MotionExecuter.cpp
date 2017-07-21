/*
  MotionExecuter.cpp - Part of TRACER

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

#include "../config.h"

#ifdef ENABLE_STEPPER_CONTROL

#include "MotionExecuter.h"
#include "SpeedManager.h"
#include "../Interfaces/CommandInterface.h"
#include "motion_data.h"
#include "StepperController.h"
#include "LinearMotionN/LinearMotionN.h"
#include "MotionScheduler.h"

/*
 * TODOs for a correct motion setup :
 *      set end distances
 *      set first elementary_distances, calling MotionExecuter::fill_movement_data(true,  ... );
 *      set last  elementary_distances, calling MotionExecuter::fill_movement_data(false, ... );w
 *      set speed data, calling MotionScheduler::pre_set_speed_axis(...);
 *      set processing functions, calling MotionExecuter::fill_processors(...);
 *      call MotionExecuter::enqueue_movement_data
 *
 */


//------------------------------------------------movement_queue_management---------------------------------------------

/*
 * fill_movement_data : this function hashes the first sub_movement of the future motion, and adds the resulting data
 *      to the future movement data
 */

void MotionExecuter::start() {
    init_stepper_interrupt();
    setup_stepper_interrupt(wait_for_movement, 30000);
}

void MotionExecuter::fill_movement_data(bool first, uint8_t *elementary_dists, uint32_t count, uint16_t nsig) {
    if (first) {
        motion_data_to_fill.count = count;
        motion_data_to_fill.initial_dir_signature = nsig;
    } else {
        motion_data_to_fill.final_dir_signature = nsig;
    }

    disable_stepper_interrupt()

#define STEPPER(i, sig, ...)\
    if (nsig&sig) MotionExecuter::end_distances[i] += elementary_dists[i];\
    else MotionExecuter::end_distances[i] -= elementary_dists[i];

#include "../config.h"

#undef STEPPER

    enable_stepper_interrupt()

    //uint8_t local_array_end = 0;
    uint8_t motion_depth = 0;
    uint16_t pre_signatures[8];
    while (true) {
        uint16_t sig = 0;

        //Step 1 : get signature for current move

#define STEPPER(i, signature, ...) if (*(elementary_dists+i) & (uint8_t) 1) { sig |= signature; }

#include "../config.h"

#undef STEPPER

        //If a move is required to branch :
        pre_signatures[motion_depth] = sig;
        motion_depth++;

        bool end_move = true;

        //Step 2 : shift right and check if last move is reached
#define STEPPER(i, sig, ...) if ((*(elementary_dists+i) >>= 1)) { end_move = false; }

#include "../config.h"

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




void MotionExecuter::fill_speed_data(uint16_t delay_numerator, uint16_t regulation_delay, float ratio, uint8_t processing_steps) {
    motion_data_to_fill.processing_steps = processing_steps;
    motion_data_to_fill.delay_numerator = delay_numerator;
    motion_data_to_fill.regulation_delay = regulation_delay;
    motion_data_to_fill.ratio = ratio;
}

void MotionExecuter::fill_processors(void (*init_f)(), void (*step_f)(uint16_t), uint16_t (*position_f)(uint8_t*), void (*speed_f)()) {
    motion_data_to_fill.init_processor = init_f;
    motion_data_to_fill.step = step_f;
    motion_data_to_fill.position_processor = position_f;
    motion_data_to_fill.speed_processor = speed_f;
}

void MotionExecuter::enqueue_movement_data() {
    motion_data_queue.push(motion_data_to_fill);
}

/*
 * The movement initialisation function :
 *  - if possible, starts a movement;
 *  - if not, returns, and will re-check later (interrupt)
 */
void MotionExecuter::wait_for_movement() {

    disable_stepper_interrupt();


    //digitalWrite(13, !digitalRead(13));


    if (motion_data_queue.available_elements()) {


        popped_data = motion_data_queue.pull();

        /*
         * Motion initialisation - according to the pulled motion data, set :
         *
         *      Motion data :
         * - count : number of sub_movements;
         * - axis_signatures = the first elementary movement axis_signatures (pre_processed);
         * - position_processor, speed_processor, functions used during movement initialise (depend on calling class);
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

        position_processor = popped_data.position_processor;
        speed_processor = popped_data.speed_processor;
        step = popped_data.step;

        //Copy all axis_signatures in es0, and set is_es_0 so that es0 will be saved at the beginning of "prepare_next_sub_motion"
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
        SpeedManager::set_delay_parameters(popped_data.regulation_delay, popped_data. delay_numerator, popped_data.ratio, popped_data.processing_steps);

        (*popped_data.init_processor)();



        set_stepper_int_function(prepare_next_sub_motion);
    }

    enable_stepper_interrupt();

}

void MotionExecuter::set_last_sub_motion() {//TODO END POSITION
    //Copy all axis_signatures in es0, and set is_es_0 so that es0 will be saved at the beginning of "prepare_next_sub_motion"
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

void MotionExecuter::prepare_next_sub_motion() {

    disable_stepper_interrupt();



    saved_trajectory_indice = trajectory_indice;
    uint16_t *elementary_signatures;

    //save the motion scheme computed previously, so that new values won't erase the current ones
    if (is_es_0) {
        saved_elementary_signatures = es1, elementary_signatures = es0, is_es_0 = false;
    } else {
        saved_elementary_signatures = es0, elementary_signatures = es1, is_es_0 = true;
    }



    uint8_t elementary_dists[NB_STEPPERS];
    uint16_t negative_signatures = position_processor(elementary_dists);//2*(NB_STEPPERS - 1) tics


    //Distances Processing


    STEP_AND_WAIT






    {
        //TODO TESTER AVEC LE SHIFT SUR LA SIGNATURE DE DISTANCE ET VOIR SI C'EST PLUS EFFICACE

        distances_lock = true;

#define STEPPER(i, sig, ...)\
        if (negative_signatures&sig) end_distances[i] += elementary_dists[i];\
        else end_distances[i] -= elementary_dists[i];\

#include "../config.h"


#undef STEPPER


        SpeedManager::heuristic_distance();
        distances_lock = false;


    }
    STEP_AND_WAIT;

    //next move Processing



    {
        uint8_t motion_depth = 0;
        uint16_t pre_signatures[8];
        while (true) {
            uint16_t sig = 0;

            //Step 1 : get signature for current move

#define STEPPER(i, signature,...) if (*(elementary_dists+i) & (uint8_t) 1) { sig |= signature; }

#include "../config.h"

#undef STEPPER

            //If a move is required to branch :
            pre_signatures[motion_depth] = sig;
            motion_depth++;


            bool end_move = true;

            //Step 2 : shift right and check if last move is reached
#define STEPPER(i, signature,...) if ((*(elementary_dists+i) >>= 1)) { end_move = false; }

#include "../config.h"

#undef STEPPER



            if (end_move) {//if next_move is null
                trajectory_indice = trajectory_indices[motion_depth-1];
                break;
            }


        }

        //Writing axis_signatures in the correct order
        int i = 0;
        for (;motion_depth--;) {
            elementary_signatures[i++] = pre_signatures[motion_depth];
        }



    }
    STEP_AND_WAIT;



    SpeedManager::regulate_speed();

    STEP_AND_WAIT;




    if (SpeedManager::speed_processing_required) {
        SpeedManager::speed_processing_required = false;
        (*speed_processor)();

        /*
        //Actions settings
        {
            float temp_speed = plus noir e qu'une arabesav_c_speed + speed_numerator * distance_square_root;
            MOTION_STEP_AND_WAIT;
            for (uint8_t action_index = linear_tools_nb; action_index--;) {
                linear_set_functions[action_index](temp_speed);
                MOTION_STEP_AND_WAIT;
            }
        }
         */
    }


    set_stepper_int_function(finish_sub_movement);
    enable_stepper_interrupt();

}

//----------------------------------------------SPEED_MANAGEMENT--------------------------------------------------------
int i = 4;
void MotionExecuter::finish_sub_movement() {
    disable_stepper_interrupt();

    uint16_t s_w_signature;
    if (!(s_w_signature = saved_elementary_signatures[trajectory_array[saved_trajectory_indice]]))
        s_w_signature = saved_elementary_signatures[trajectory_array[--saved_trajectory_indice]];

    (*step)(s_w_signature);
    if (!saved_trajectory_indice--) {
        if (count) {
            count--;

#ifdef position_log
            if (!(i--)) {
                MotionScheduler::send_position();
                i=20;
            }
#endif
            set_stepper_int_function(prepare_next_sub_motion);
        } else {
            if (penultimate_movement) {
                saved_trajectory_indice = trajectory_indice;
                saved_elementary_signatures = (is_es_0) ? es1 : es0;
                penultimate_movement = false;
            } else if (ultimate_movement) {
                set_last_sub_motion();
                ultimate_movement = false;
            } else {

                StepperController::echo_positions();
                for (int i = 0; i<NB_STEPPERS; i++) {
                }
                MotionScheduler::send_position();
                set_stepper_int_function(wait_for_movement);
                ultimate_movement = penultimate_movement = true;
            }
        }
    }
    enable_stepper_interrupt();
}


#define m MotionExecuter

//Acceleration Fields
uint32_t m::count;

Queue<motion_data> m::motion_data_queue(MOTION_DATA_QUEUE_SIZE);
motion_data m::motion_data_to_fill;
motion_data m::popped_data;

//Directions
bool tdir[NB_STEPPERS];
bool *const m::axis_directions_negative = tdir;


//End distances
int32_t ted[NB_STEPPERS];
int32_t *const m::end_distances = ted;
volatile bool m::distances_lock;


uint16_t *saved_elementary_signatures;
uint8_t saved_trajectory_indice;

uint8_t traj[255] = {
        0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,
        0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,6,
        0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,
        0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,7,
        0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,
        0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,6,
        0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,
        0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0};

uint8_t *const m::trajectory_array = traj;

uint16_t tes0[8], tes1[8];
uint16_t *const m::es0 = tes0, *const m::es1 = tes1;
bool m::is_es_0;

const uint8_t tti[8] {0, 2, 6, 14, 30, 62, 126, 254};
const uint8_t *const m::trajectory_indices =  tti;

uint16_t *m::saved_elementary_signatures = tes0;
uint8_t m::saved_trajectory_indice;
uint8_t m::trajectory_indice;

//Processors;
void (*m::step)(uint16_t);
uint16_t (*m::position_processor)(uint8_t *);
void (*m::speed_processor)();

bool m::ultimate_movement = true, m::penultimate_movement = true;
#undef m


#endif