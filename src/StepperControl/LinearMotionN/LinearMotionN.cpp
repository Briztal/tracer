/*
  LinearMotionN.cpp - Part of TRACER

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
  along with TRACER.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "../../config.h"

#ifdef ENABLE_STEPPER_CONTROL


#include "LinearMotionN.h"
#include "../../Interfaces/CommandInterface.h"
#include "../../Actions/ContinuousActions.h"
#include "../MotionExecuter.h"
#include "../SpeedManager.h"
#include "../MotionScheduler.h"
#include "../../Core/EEPROMStorage.h"
#include "../StepperController.h"
#include "../mathProcess.hpp"
#include "../motion_data.h"


#define INT_OVF 65535

#define PROCESSING_STEPS 31

/*
 * Prepare_motion : takes a float[NB_STEPPERS] in argument.
 *
 *      It goes to the specified coordinates.
 *
 */

void LinearMotionN::prepare_motion(const float *destinations_t) { //GO TO

    //TODO LINEAR_TOOLS

    /*
    //Set number of tools in continuous modes, and set action functions related
    if (ContinuousActions::linear_modes_enabled()) {
        linear_tools_nb = ContinuousActions::getSetFunctions(linear_set_functions);
    } else
        linear_tools_nb = 0;
        */

    //Distances array

    unsigned long absolute_distances[NB_STEPPERS]{0};

    //Extract motion distances, and set greater axis to 0.
    unsigned char max_axis = setup_movement_data(destinations_t, absolute_distances);

    //null move, nothing to do.
    if (max_axis == 255)
        return;

    //Move choice : a m
    if (absolute_distances[max_axis] < PROCESSING_STEPS) {
        //max distance < steps per elementary-move -> only one micro move
        //TODO micro_move(absolute_distances);
    } else if (absolute_distances[max_axis] < INT_OVF) {
        //A move is indexed on an int value -> max distance must not be > INT_OVF
        //max distance < max value for int : only one move
        simple_move(absolute_distances);
    } else {
        CI::echo("ERROR : MOVEMENT SEQUENCE NOT IMPLEMENTED FOR INSTANCE.");
        //max distance > max_value for int : multiple moves
        //TODO multiple_moves(dimension, absolute_distances);
    }

}

/*----------------------------------------------------------------------------------------------------------------------
------------------------------------------------PREPARATORS-------------------------------------------------------------
----------------------------------------------------------------------------------------------------------------------*/
/*
 * Parameter set function. It :
 * - extracts distances;
 * - sets steppers directions
 * - converts distances to absolute values
 * - set speed according to the movement (movement dimensions and greater axis)
 */

unsigned char LinearMotionN::setup_movement_data(const float *destinations_t, unsigned long *absolute_distances) {

    //As distance on one axis can be null, an axis axis variable must be introduced. incremented after axis processed

    float distsmm[NB_STEPPERS]{0};

    //TODO USE MOTION_SIZE PROVIDED BY THE PARSER

    float sq_dist_sum = 0;
    unsigned char direction_signature = 0;
    unsigned char max_axis = 255;
    unsigned long max_dist = 0;

    //Order determination : Using an insertion sort on end_distances, and extract axis_data at the same time
    for (unsigned char axis = 0; axis < NB_STEPPERS; axis++) {
        float steps;
        long steps_destination, distance;

        steps = EEPROMStorage::steps[axis];
        steps_destination = (long) (destinations_t[axis] * steps);
        distance = steps_destination - MotionScheduler::positions[axis];

        //If distance is not null :
        if (distance) {

            //Update position
            MotionScheduler::positions[axis] = steps_destination;

            //Wait for distances to be unlocked
            while (MotionExecuter::distances_lock);
            //Update destination

            disable_stepper_interrupt()

            MotionExecuter::end_distances[axis] += distance;

            enable_stepper_interrupt()
            /*
             * Direction : builds the direction signature :
             * binary where the nth msb is 0 if axis n is positive, 1 if direction is negative
             */
            if (distance < 0) {
                distance = -distance;
                direction_signature |= MotionScheduler::axis_signatures[axis];
            }

            //Take the absolute distance, and compare if it is the greatest distance. If true, memorise the max axis.
            unsigned long absolute_distance = (unsigned long) distance;
            if (absolute_distance > max_dist) {
                max_dist = absolute_distance;
                max_axis = axis;
            }
            absolute_distances[axis] = absolute_distance;

            //Millimeter distances computation
            float distmm = distsmm[axis] = (float) absolute_distance / steps;
            sq_dist_sum += distmm * distmm;
        }
    }

    //If null move : fail with error value;
    if (max_axis == 255)
        return 255;

    //Direction memorising :
    negative_signatures = direction_signature;

    data_to_fill.max_axis = max_axis;

    //Local variable for speed datacomputation
    float sqrt_sq_dist_sum = sqrt(sq_dist_sum);
    float distance_coefficient = distsmm[max_axis] / sqrt_sq_dist_sum;

    //Get the adjusted regulation speed;
    float regulation_speed = MotionScheduler::get_regulation_speed_linear(distsmm, sqrt_sq_dist_sum);

    //Calculate and fill the speed data
    MotionScheduler::pre_set_speed_axis(max_axis, distance_coefficient, regulation_speed, PROCESSING_STEPS);

    MotionExecuter::fill_processors(initialise_motion, StepperController::fastStep, process_position, process_speed);
    return max_axis;

}


void LinearMotionN::step_and_delay(unsigned char sig) {
    //simple_delay(delay0);
    StepperController::fastStep(sig);
}

/*----------------------------------------------------------------------------------------------------------------------
---------------------------------------------------MOVES----------------------------------------------------------------
----------------------------------------------------------------------------------------------------------------------*/

void LinearMotionN::micro_move(unsigned long *dists) {//TODO FOX PASSER EN ELEMENTARY_DISTS
    /*
    //Setting destination
    for (unsigned char axis = 0; axis < dimension; axis++)
        next_t[axis] = (unsigned int) dists[axis];

    //Effective StepperControl.
    elementary_motion(0);
     */
}

void LinearMotionN::simple_move(unsigned long *dists) {


    //No need to divide the motion -> copy of dist in motion_dists
    unsigned int motion_dists[NB_STEPPERS];
    for (int axis = 0; axis < NB_STEPPERS; axis++) {
        motion_dists[axis] = (unsigned int) dists[axis];
    }


    //Position process preparation : set slopes and shift_nb
    set_position_data(motion_dists);;


    //motion data setting
    set_motion_data(motion_dists);

    //Enqueue motion_data
    data_queue.push(data_to_fill);

    //Enqueue motion_data
    MotionExecuter::enqueue_movement_data();


}

/*
 * The position processing init function : it sets parameters required to process position :
 *  - slopes
 *  - shift_nb
 */
void LinearMotionN::set_position_data(unsigned int *dists) {

    //max_axis must have been defined previously

    unsigned char max_axis = data_to_fill.max_axis;

    unsigned int *slopes = data_to_fill.slopes;

    slopes[max_axis] = 1;

    //Determining shift_nb
    unsigned int count = dists[max_axis];
    unsigned long pow = 1;//unsigned long because it needs to be greater than an unsigned int, which can equal 65535...
    unsigned char shift_nb = 0;
    while (pow < count) {
        pow <<= 1;
        shift_nb++;
    }


    data_to_fill.shift_nb = shift_nb;

    //Determine all slopes
    unsigned long temp_slope;
    for (unsigned int axis = 0; axis < NB_STEPPERS; axis++) {
        if (axis != max_axis) {
            temp_slope = pow * dists[axis];
            temp_slope /= count;
            slopes[axis] = (unsigned int) temp_slope;
        }
    }
}

/*
 * set_motion_data : this function fills movement data on the motion_data_to_fill
 *
 * It computes count, and the first elementary distances.
 *
 * Afterwards, it calls MotionExecuter::fill_movement_data, that hashes the first sub_movement, and fills data.
 */
void LinearMotionN::set_motion_data(unsigned int *motion_dists) {

    unsigned int count = motion_dists[data_to_fill.max_axis] / PROCESSING_STEPS;

    unsigned char nsig = negative_signatures;

    const unsigned char shift_nb = data_to_fill.shift_nb;
    const unsigned char max_axis = data_to_fill.max_axis;
    const unsigned int *slopes = data_to_fill.slopes;
    unsigned char *const f_pos = data_to_fill.first_pos;

    //Initial elementary distances
    unsigned char elementary_dists[NB_STEPPERS];
    for (unsigned int axis = 0; axis < NB_STEPPERS; axis++) {
        f_pos[axis] = elementary_dists[axis] = (axis == max_axis) ? (unsigned char) PROCESSING_STEPS : (unsigned char) (
                ((unsigned long) slopes[axis] * PROCESSING_STEPS) >> shift_nb);
    }

    //Fill beginning signatures
    MotionExecuter::fill_movement_data(true, elementary_dists, count, nsig);

    unsigned int last_pos_max = count*PROCESSING_STEPS;

    //Initial elementary distances
    for (unsigned int axis = 0; axis < NB_STEPPERS; axis++) {
        if (axis!=max_axis) {
            elementary_dists[axis] = (unsigned char) (motion_dists[axis] -
                                                      (((unsigned long) slopes[axis] * last_pos_max) >> shift_nb));
        } else {
            elementary_dists[axis] = (unsigned char) (motion_dists[axis] - last_pos_max);
        }
    }

    //Fill ending signatures
    MotionExecuter::fill_movement_data(false, elementary_dists, count, nsig);
}


void LinearMotionN::initialise_motion() {
//TODO COPIED... BETTER TAKE DIRECTLY POINTER

    linear_data data = data_queue.pull();

    MR_shift_nb = data.shift_nb;
    MR_max_axis = data.max_axis;

    for (int axis = 0; axis<NB_STEPPERS; axis++) {
        MR_positions[axis] = (unsigned int) data.first_pos[axis];
    }
    memcpy(MR_slopes, data.slopes, 2 * NB_STEPPERS);

}

/*
 * Position processing function.
 * It takes 2*dimension-1 processing windows to determine all positions
 */
unsigned char LinearMotionN::process_position(unsigned char *elementary_dists) {//2n-2

    int i1 = (MR_positions[MR_max_axis] += (elementary_dists[MR_max_axis] = PROCESSING_STEPS));
    int i2;
#define STEPPER(i, ...) \
    if (i!=MR_max_axis){\
        i2 = (unsigned int) (((unsigned long) MR_slopes[i] * i1) >> MR_shift_nb);\
        STEP_AND_WAIT\
        MR_positions[i] += (elementary_dists[i] = (unsigned char) ((i2 - MR_positions[i])));\
        STEP_AND_WAIT\
    }\

#include "../../config.h"

#undef STEPPER

    return MR_negative_signatures;
}

/*
 * The Speed processing function
 * It takes 2 tics to compute delay0;
 */
long l;
void LinearMotionN::process_speed() {//2

    float inverse = invert(SpeedManager::distance_square_root);
    STEP_AND_WAIT
    SpeedManager::delay0 = (unsigned int) (SpeedManager::delay_numerator * inverse);
    set_stepper_int_period(SpeedManager::delay0);
    STEP_AND_WAIT
}


#define m LinearMotionN

Queue<linear_data> m::data_queue(MOTION_DATA_QUEUE_SIZE);
linear_data m::data_to_fill;

unsigned int *const m::MR_positions = new unsigned int[NB_STEPPERS];
unsigned int *const m::MR_slopes = new unsigned int[NB_STEPPERS];
int m::MR_shift_nb;
unsigned char m::MR_max_axis;
unsigned char m::MR_negative_signatures;
unsigned char m::negative_signatures;

#undef m;

/*

void LinearMotionN::multiple_moves(unsigned char dimension, unsigned long *dists) {
    unsigned int motion_dists[dimension];

    //Division of the motion in 2^dec_count sub_moves
    //calculation of dec_count
    unsigned long dist_0 = dists[0];
    int dec_count = 0;
    while (dist_0 > INT_OVF) {
        dist_0 >>= 1;
        dec_count++;
    }

    //set motion_dists for the [motion_nb]-1 sub-moves
    *motion_dists = (unsigned int) dist_0;
    for (int axis = 1; axis < dimension; axis++)
        motion_dists[axis] = (unsigned int) (dists[axis] >> dec_count);

    int motion_nb = (1 << dec_count) - 1;

    set_position_data(motion_dists);
    memset(pos_t, 0, NB_STEPPERS);

    //first n_1 submoves
    set_initial_dists();

    for (int sub_motion = 0; sub_motion < motion_nb; sub_motion++) {

        //StepperControl preparation : set initial destinations, and draw parameters

        //Effective motion

        setup_motion(motion_dists);

        draw();

    }
    //Last sub-move

    //Calculation of remaining end_distances
    for (int axis = 0; axis < dimension; axis++)
        motion_dists[axis] = (unsigned int) (dists[axis] - motion_nb * motion_dists[axis]);

    //StepperControl Preparation : setting slopes and draw parameters
    set_position_data(motion_dists);

    //Elementary dists setting
    set_initial_dists();

    //Effective StepperControl
    set_motion_data(motion_dists);
    draw();

    regulate_speed();

}
 */

#endif