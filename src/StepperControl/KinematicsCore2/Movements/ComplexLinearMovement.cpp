/*
  ComplexLinearMovement.h - Part of TRACER

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

#include "ComplexLinearMovement.h"

#include <interface.h>
#include <StepperControl/MachineInterface.h>
#include <StepperControl/TrajectoryTracer.h>


/*
 * plan_movement : this function takes a destination position in argument_t and prepares a linear movement,
 *      from the current position to the given destination, if the required movement is not a null movement.
 *
 */

task_state_t ComplexLinearMovement::plan_movement(const float *const destination) {



    //get the movement step_distances
    float distances[NB_AXIS];

    //extract the array case address (more efficient than by-object-process)
    k2_linear_data *d = linear_data_queue.get_input_ptr();

    //get the positions and slopes pointer;
    float *positions = d->offsets;
    float *slopes = d->slopes;

    //define maximum distance and maximum axis, that will be correctly set in the function below.
    float max_distance = 0;
    uint8_t max_axis = 0;

    //Get distance relative variables
    bool null_move = get_distances(positions, destination, distances, &max_axis, &max_distance);


    //end if the machine is already at the destination;
    if (null_move) {

        //Send an error message.
        CI::echo("ERROR : THE MACHINE IS ALREADY AT ITS DESTINATION POSITION THE MOVEMENT WILL BE IGNORED.");

        return complete;

    }

    d->max_axis = max_axis;

    //fill the slopes array
    get_slopes(slopes, distances, max_axis, max_distance);

    //update pre_process vars for increment pre_processing
    pre_process_slopes = slopes;
    pre_process_offsets = positions;
    pre_process_max_axis = max_axis;

    //Wait for the enqueuing to be authorised in TrajectoryTracer.
    while (TrajectoryTracer::enqueue_unauthorised());


    //The state of the process must be saved, as in determies if the task must be reprogrammed
    task_state_t enqueue_state = TrajectoryTracer::enqueue_movement(0, max_distance, initialise_movement,
                                                                    finalise_movement, get_position,
                                                                    get_real_time_position);

    //If the process has complete,
    if (enqueue_state == complete) {

        //Push the local data
        linear_data_queue.enqueue();

        //Update the final position
        MachineInterface::update_position(destination);

    }

    return enqueue_state;

}


//--------------------------------------------------Pre-Processing--------------------------------------------------

/*
 * get_distances : this function computes the current move's step_distances.
 *
 * At the same time, it determines the maximum axis, and return "the current move is a null plan_movement".
 *
 */

bool
ComplexLinearMovement::get_distances(float *position, const float *destination, float *distances, uint8_t *maximum_axis,
                                     float *maximum_distance) {


    //Get the current position
    MachineInterface::get_current_position(position);

    //Initialise the maximum axis and maximum distance.
    uint8_t max_axis = 0;
    float max_dist = 0;

    //boolean for null move detection
    bool null_move = true;

    //for each axis
    for (uint8_t axis = 0; axis < NB_AXIS; axis++) {

        //get the distance
        float distance = destination[axis] - position[axis];

        float a_dist = (distance > 0) ? distance : -distance;

        //if distance is not zero, the movement is not null
        if (a_dist) {
            null_move = false;
        }

        //Update max_axis and max_dist if needed
        if (a_dist > max_dist) {
            max_axis = axis;
            max_dist = a_dist;
        }

        //save distance
        distances[axis] = distance;

    }

    //update maximum_axis and distance with the determined values
    *maximum_axis = max_axis;
    *maximum_distance = distances[max_axis];

    //return if the current movement is null
    return null_move;

}


/*
 * get_slopes : this function determines the slopes on every axis.
 *
 *  The slope on a particular axis is the ratio of the distance on this axis with the maximum distance of all axis.
 *
 */

void ComplexLinearMovement::get_slopes(float *slopes, const float *const distances, const uint8_t max_axis,
                                       const float max_distance) {

    //get the inverse of the maximum distance
    float inv_max_dist = 1 / max_distance;

    //the maximum axis slope is one;
    slopes[max_axis] = 1;

    //determine slopes on other axis.
    for (uint8_t axis = 0; axis < NB_AXIS; axis++) {
        if (axis != max_axis) {
            slopes[axis] = distances[axis] * inv_max_dist;
        }
    }
}


/*
 * get_position : this function computes the position for the increment calculation procedure, according to
 *      the point provided in argument_t.
 */

void ComplexLinearMovement::get_position(float indice, float *positions) {

    //cache vars : in this function, only pre_process variables are used.
    const uint8_t max_axis = pre_process_max_axis;
    const float *const offsets = pre_process_offsets;
    const float *const slopes = pre_process_slopes;

    //position for the maximum axis;
    positions[max_axis] = offsets[max_axis] + indice;

    //positions for other axis
    for (int axis = 0; axis < NB_AXIS; axis++) {
        if (axis != max_axis) {
            positions[axis] = offsets[axis] + indice * slopes[axis];
        }
    }
}


//-----------------------------------------------Real_time_Processing-----------------------------------------------

/* initialise_movement : this function initialises the movement, in peaking the next movement in the queue, and
 *      updating the slopes and offsets pointers, and the maimum axis.
 *
 */

void ComplexLinearMovement::initialise_movement() {

    //Get the address of the top element
    k2_linear_data *d = linear_data_queue.read_output();

    //Update all real-time data
    real_time_max_axis = d->max_axis;
    real_time_offsets = d->offsets;
    real_time_slopes = d->slopes;

    //Do not discard the current element, of it is likely to be rewritten.

    //The effective discard will be made in the finalisation function below.

}


/*
 * finalise_movement : this function finalises the movement, in discarding the current movement in the queue, peaked
 *      in the function above.
 */

void ComplexLinearMovement::finalise_movement() {

    //the current element is now used, discard it.
    linear_data_queue.discard();

}


/*
 * get_real_time_position : this function computes the position for the increment calculation procedure, according to
 *      the point provided in argument_t.
 */

void ComplexLinearMovement::get_real_time_position(float index, float *positions) {

    //cache vars : in this function, only real time variables are used.
    const uint8_t max_axis = real_time_max_axis;
    const float *const offsets = real_time_offsets;
    const float *const slopes = real_time_slopes;

    //position for the maximum axis;
    positions[max_axis] = offsets[max_axis] + index;

    //positions for other axis
    for (int axis = 0; axis < NB_AXIS; axis++) {

        if (axis != max_axis) {
            positions[axis] = offsets[axis] + index * slopes[axis];
        }
    }

}

//--------------------------------------------Static declaration - definition-------------------------------------------

#define m ComplexLinearMovement

//the data queue
Queue<k2_linear_data> ComplexLinearMovement::linear_data_queue(MOVEMENT_DATA_QUEUE_SIZE);

//Pre_processing data
float t_ppof[NB_AXIS], t_ppsl[NB_AXIS];
float *m::pre_process_offsets = t_ppof;
float *m::pre_process_slopes = t_ppsl;
uint8_t m::pre_process_max_axis = 0;

//Real time data
float t_rtof[NB_AXIS], t_rtsl[NB_AXIS];
float *m::real_time_offsets = t_rtof;
float *m::real_time_slopes = t_rtsl;
uint8_t m::real_time_max_axis = 0;

#undef m

#endif