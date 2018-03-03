/*
  SubMovementManager.h - Part of TRACER

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

#include "SubMovementManager.h"
#include "StepperController.h"
#include "TrajectoryTracer.h"
#include "MovementCoordinator.h"
#include <StepperControl/KinematicsCore1/KinematicsCore1.h>
#include <StepperControl/KinematicsCore2/KinematicsCore2.h>
#include <Interaction/Interaction.h>
#include <Kernel/Kernel.h>


/*
 * solve all fields;
 */

void SubMovementManager::initialise_data() {

    //Reset the current stepper position;
    memset(current_steppers_positions, 0, NB_STEPPERS * sizeof(float));

    //Reset the sub movement data queue;
    sub_movement_queue.clear();

    //Reset Indexation variables
    index_limit = index = increment = 0;
    positive_index_dir = false;

    //Reset the trajectory function
    get_new_position = nullptr;

    //Reset the queue flag state
    movement_last_position_processed_flag = false;

    //Reset the end distances;
    memset(end_distances, 0, NB_STEPPERS * sizeof(float));

    //Reset the current end position;
    memset(end_position, 0, NB_STEPPERS * sizeof(float));

    //Reset the current jerk position;
    memset(jerk_position, 0, NB_STEPPERS * sizeof(float));

    //Reset the current jerk distances;
    memset(jerk_distances, 0, NB_STEPPERS * sizeof(float));

}


/*
 * initialise_movement : this function is called when the current movement is finished.
 *
 *  Parameters passed update all variables used to compute positions :
 *      - beginning and ending, the minimum and the maximum of index (beginning and ending of movement)
 *      - incr : the index increment for the first plan_movement (pre computed and supposedly accurate)
 *      - trajectory_function : the function that actually computes stepper positions.
 *
 */

uint8_t SubMovementManager::update_current_movement(movement_data_t *movement_data) {

    //----------------Movement data initialisation-----------------------

    //Index data
    index_limit = movement_data->ending;
    index = movement_data->beginning;
    increment = movement_data->beginning_increment;
    positive_index_dir = (increment > 0);

    //update position provider
    get_new_position = movement_data->trajectory_function;

    //flag
    movement_last_position_processed_flag = false;

    //Return the number of sub_movements to wait before updating the movement environment
    return sub_movement_queue.available_objects();

}


void SubMovementManager::display_distances() {
    for (int i = 0; i < NB_STEPPERS; i++) {
        std_out(str(i) + " ed : " + str(end_distances[i]) + " jd : " + str(jerk_distances[i]));
    }

    for (int i = 0; i < NB_STEPPERS; i++) {
        std_out(str(i) + " ed : " + str(end_position[i]) + " jd : " + str(jerk_position[i]));
    }
}



//-----------------------------------Intermediary_Positions_Pre_Computation---------------------------------------------

/*
 * fill_sub_movement_queue : this function will push new sub movement until the sub_movement queue is full.
 */

void SubMovementManager::fill_sub_movement_queue() {


    std_out("FILLING THE MOVEMENT QUEUE");
    while (sub_movement_queue.available_spaces() && !movement_last_position_processed_flag) {
        push_new_sub_movement();
    }

    std_out("END FILL");

}


/*
 * is_movement_processed : this function returns true if the current movement's last position has been processed.
 */

bool SubMovementManager::is_movement_processed() {
    return movement_last_position_processed_flag;
}


/*
 * compute_new_sub_movement : this function gets the next position, defined by the image of index+increment
 *      by the trajectory function.
 *
 *      If the image is at a distance from the current position comprised on
 *          MINIMUM_DISTANCE_LIMIT and MAXIMUM_DISTANCE_LIMIT
 *
 *      Then the position is accepted and enqueued in the position queue.
 *
 *      If not, the position is discarded.
 *
 *      In all cases, the increment is updated to converge to the distance targetVector.
 *
 *      Notes :
 *          - The distance is given by the maximum of all step_distances on each stepper.
 *          - The second criteria is (far) more restrictive than the first, but gives a better regulation_speed regulation
 */

void SubMovementManager::push_new_sub_movement() {

    if ((!sub_movement_queue.available_spaces()) || (movement_last_position_processed_flag)) {

        return;

    }

    //Initialise the queue flag;
    bool queue_flag = false;

    //Get the insertion address;
    sub_movement_data_t *sub_movement_data = sub_movement_queue.get_insertion_address(&queue_flag);

    //Interaction the flag, for safety:
    if (!queue_flag) {

        //Log if the flag is reset (error)
        std_out("ERROR in SubMovementManager::push_new_sub_movement : the insertion element is already assigned");

        //Emergency stop
        Kernel::emergency_stop();

        return;

    }


    //8us 4 steppers, 11us 17 steppers ; 7.07us + 0.23us per stepper
    //get a new sub-movement;
    compute_new_sub_movement(sub_movement_data);

    //Declare a flag that will allow us to control good execution;
    bool execution_flag;

    //As the computation occurred without errors, we must now verify the computed movement.

    //5us 4 steppers, 11us 17steppers : 3.15us + 0.46us per stepper
    //validate the sub-movement;
    execution_flag = confirm_sub_movement(sub_movement_data);

    //Fail if the movement was not added
    if (!execution_flag) return;

    //As the verification occurred without errors, we must now enqueue the computed movement.

    //Initialise a queue flag;
    queue_flag = false;

    //Now that validity checks are made, process the sub_movement data
    sub_movement_queue.insert(&queue_flag);

    queue_flag = false;

    sub_movement_queue.get_reading_address(&queue_flag);

    //Interaction the flag for safety
    if (!queue_flag) {

        //Log
        std_out("ERROR in SubMovementManager::confirm_sub_movement : The insertion position is already allocated.");

        //Emergency stop
        Kernel::emergency_stop();

    }

}


/*
 * compute_new_sub_movement : this function will take a pointer to a sub_movement_data_t, and fill it
 *  with a new sub_movement.
 *
 *  It first updates the index candidate, and then gets a new position to reach.
 *
 *  It then updates the position data according to the translation of the previous position, and
 *      finally fills the kinetics part, with the help of the kinetics manager.
 */

void SubMovementManager::compute_new_sub_movement(sub_movement_data_t *sub_movement_data) {

    //------------------Index computing-----------------------------

    //Get the new index candidate;
    float local_index_candidate = index + increment;

    //If the index is close of less than one increment of its limit, go to the end position
    if (((positive_index_dir) && (local_index_candidate + increment > index_limit)) ||
        ((!positive_index_dir) && (local_index_candidate + increment < index_limit))) {
        movement_last_position_processed_flag = true;
        local_index_candidate = index_limit;
    }

    //Update the index candidate
    sub_movement_data->index_candidate = local_index_candidate;


    //------------------Position computing-----------------------------

    //Get the high level position corresponding to the computed index
    get_new_position(local_index_candidate, sub_movement_data->candidate_high_level_positions);


    //Translate the high level position into steppers position;
    StepperController::translate(sub_movement_data->candidate_high_level_positions,
                                 sub_movement_data->future_steppers_positions);

    //Call the Kinematics Manager, to fill the kernel-reserved part of the data.
    Kinematics::initialise_sub_movement(sub_movement_data);

}


/*
 * confirm_sub_movement : this function will validate
 */

bool SubMovementManager::confirm_sub_movement(sub_movement_data_t *sub_movement_data) {


    //Get the steppers step_distances, high level step_distances, and the maximal stepper and distance
    float max_distance = get_steppers_distances(current_steppers_positions, sub_movement_data);

    //Distance Validity_Verification : fail if an error is detected
    if (distance_bounds_error(max_distance)) {
        return false;
    }

    //Update the index
    index = sub_movement_data->index_candidate;

    //Update the kernel data
    Kinematics::accept_sub_movement(sub_movement_data);

    return true;

}


/*
 * distance_bounds_error : this function verifies that the maximal distance (for the current movement) verifies
 *  the distances bounds.
 */

bool SubMovementManager::distance_bounds_error(float max_distance) {

    float up_error = (max_distance >= MAXIMUM_DISTANCE_LIMIT);

    //Increment adjustment according to the targetVector
    if ((uint16_t) max_distance != (uint16_t) DISTANCE_TARGET) {

        //get the distance ratio
        float ratio = (float) DISTANCE_TARGET / max_distance;

        //Update the increment with the ratio;
        increment *= ratio;

        //Notify the KinematicsCore that the increment has been changed (not used in KinematicsCore2)
        Kinematics::update_evolution_coefficient(ratio);

    }

    //If the maximal distance is below the lower limit :
    if (up_error && (!movement_last_position_processed_flag)) {
        return true;
    }

    //If the maximal distance is below the lower limit :
    if ((!movement_last_position_processed_flag) && (max_distance <= MINIMUM_DISTANCE_LIMIT)) {
        return true;
    }

    return false;

}


/*
 * get_steppers_distances : this function determines the distance between a position and a targetVector, for all stepper.
 *
 * It also computes the direction signature, as step_distances are positive_rotation numbers, and saves the maximum stepper
 *      and the maximum distance.
 */

float SubMovementManager::get_steppers_distances(float *const pos, sub_movement_data_t *sub_movement_data) {

    float *dest = sub_movement_data->future_steppers_positions;
    float *real_dists = sub_movement_data->f_step_distances;

    uint8_t *int_dists = sub_movement_data->step_distances;

    //Cache variable, to avoid pointer access.
    uint8_t max_int_dist = 0;
    float max_f_dist = 0;
    sig_t dir_signature = 0;


    //We must determine the distance for every stepper
    for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {


        //get destination and position
        float d = dest[stepper], p = pos[stepper];

        //update position with destination;
        pos[stepper] = d;

        //get the real distance
        float distance = d - p;

        uint8_t int_dist;

        //get absolute distance
        if (distance < 0) {
            distance = -distance;
            dir_signature |= axis_signatures[stepper];
            int_dist = (uint8_t) ((int32_t) p - (int32_t) d);
        } else {

            int_dist = (uint8_t) ((int32_t) d - (int32_t) p);
        }


        //persist step_distances
        real_dists[stepper] = distance;
        int_dists[stepper] = int_dist;

        //Update ending dist
        if (int_dist > max_int_dist) {
            max_int_dist = int_dist;
            max_f_dist = distance;
        }

    }

    //Save the signature
    sub_movement_data->direction_signature = dir_signature;

    return max_f_dist;

}


//----------------------------------------------------pop---------------------------------------------------------------

/*
 * read_next_sub_movement : this function pops a previously enqueued position.
 *
 *      It returns an error if the queue is empty.
 *
 *  It copies the step_distances in the provided array, and returns the negative signature assaoiated with those step_distances.
 *
 * TIME OF EXECUTION : 2us with 4 steppers
 *
 * Costly operations : memcpys.
 */

sub_movement_data_t *SubMovementManager::read_next_sub_movement() {

    //Get the number of available sub movements
    uint8_t size = sub_movement_queue.available_objects();

    //If no more sub movements are present in the queue :
    if (!size) {

        //if the queue is empty : error
        std_out("ERROR : IN SubMovementManager::read_next_sub_movement : THE SUB_MOVEMENT QUEUE IS EMPTY");

        //Emergency stop
        Kernel::emergency_stop();

        //Fail
        return 0;

    } else {

        //Define a flag :
        bool queue_flag = false;

        //Get the reading adress
        sub_movement_data_t *address = sub_movement_queue.get_reading_address(&queue_flag);

        //Interaction the data integrity
        if (!queue_flag) {

            //Log;
            std_out("ERROR : IN SubMovementManager::read_next_sub_movement : the reading element was not allocated");

            //Emergency stop
            Kernel::emergency_stop();

            //Fail;
            return 0;

        }

        return address;

    }

}

void SubMovementManager::discard_sub_movement() {

    //Define a flag;
    bool queue_flag = false;

    //Discard the sub_movement;
    sub_movement_queue.discard(&queue_flag);

    //Integrity control :
    if (!queue_flag) {

        //Log;
        std_out("ERROR : IN SubMovementManager::discard_sub_movement : the reading element was not allocated");

        //Emergency stop
        Kernel::emergency_stop();

    }

}


/*
 * update_end_position : this function updates the end point position.
 *
 */

void SubMovementManager::update_end_position(const float *const new_hl_position) {

    float stepper_end_position[NB_STEPPERS];

    StepperController::translate(new_hl_position, stepper_end_position);

    //Disable the stepper routine for safety, as we will modify volatile data;
    MovementCoordinator::disable_interrupt();

    for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {
        int32_t d = (int32_t) stepper_end_position[stepper];
        end_distances[stepper] += d - end_position[stepper];
        end_position[stepper] = d;
    }


}


/*
 * update_jerk_position : this function updates the jerk point position.
 *
 */

void SubMovementManager::update_jerk_position(const int32_t *const new_stepper_position) {

    //Disable the stepper routine for safety, as we will modify volatile data;
    MovementCoordinator::disable_interrupt();

    for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {
        int32_t d = (int32_t) new_stepper_position[stepper];
        jerk_distances[stepper] += d - jerk_position[stepper];
        jerk_position[stepper] = d;
    }

    //Re-enable the stepper routine (will take effect only if it was already started;
    MovementCoordinator::enable_interrupt();
}


/*
 * update_end_jerk_distances : this function updates step_distances to end point and jerk point.
 *
 *  It takes in content the absolute step_distances on each stepper of the next sub_movement, and their negative signature.
 *
 *      Reminder : the i_th bit of direction_signature is 1 if the distance on the i_th stepper is negative.
 *
 *  The global distance is given by the maximum of all distance on each stepper.
 *
 */

void SubMovementManager::update_end_jerk_distances(const sig_t negative_signatures, const uint8_t *elementary_dists) {

#define STEPPER(i, sig, ...)\
    if (negative_signatures&sig) {end_distances[i] += elementary_dists[i]; jerk_distances[i] += elementary_dists[i];}\
    else {end_distances[i] -= elementary_dists[i];jerk_distances[i] -= elementary_dists[i];}\


#include <Config/stepper_control_config.h>

#undef STEPPER

}

//-----------------------------------------Static declarations - definitions--------------------------------------------

//Static declarations - definitions;

#define m SubMovementManager

//Current stepper position;
float t_cur_pos[NB_STEPPERS]{0};
float *const m::current_steppers_positions = t_cur_pos;

//movement data queue;
Queue<sub_movement_data_t> m::sub_movement_queue(SUB_MOVEMENT_DATA_QUEUE_SIZE);


//Indexation variables
float m::index_limit = 0, m::index = 0, m::increment = 0;
bool m::positive_index_dir = false;

//Trajectory function
void (*m::get_new_position)(float, float *) = nullptr;

//Queue state
bool m::movement_last_position_processed_flag = false;


//End step_distances
int32_t k2ted[NB_STEPPERS]{0};
int32_t *const m::end_distances = k2ted;

int32_t k2tep[NB_STEPPERS]{0};
int32_t *const m::end_position = k2tep;

//The stepper jerk positions;
int32_t k2jpt[NB_STEPPERS]{0};
int32_t *const m::jerk_position = k2jpt;

//the stepper jerk step_distances;
int32_t k2jdt[NB_STEPPERS]{0};
int32_t *const m::jerk_distances = k2jdt;



//Axis signatures

#define STEPPER(i, sig, ...) sig,
sig_t k2t_sig[NB_STEPPERS + 1]{

#include <Config/stepper_control_config.h>

        0};

#undef STEPPER

const sig_t *const m::axis_signatures = k2t_sig;
