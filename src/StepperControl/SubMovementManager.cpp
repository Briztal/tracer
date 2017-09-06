//
// Created by root on 03/09/17.
//

#include "SubMovementManager.h"
#include "MachineInterface.h"
#include "TrajectoryTracer.h"
#include <hardware_language_abstraction.h>
#include <interface.h>
#include <StepperControl/KinematicsCore1/KinematicsCore1.h>
#include <StepperControl/KinematicsCore2/KinematicsCore2.h>

/*
 * initialise_movement : this function is called when the current movement is finished.
 *
 *  Parameters passed update all variables used to compute positions :
 *      - min and max, the minimum and the maximum of index (beginning and ending of movement)
 *      - incr : the index increment for the first prepare_movement (pre computed and supposedly accurate)
 *      - trajectory_function : the function that actually computes stepper positions.
 *
 */

uint8_t SubMovementManager::update_current_movement(movement_data_t *movement_data) {

    //----------------Movement data initialisation-----------------------

    //Index data
    index_limit = movement_data->max;
    index = movement_data->min;
    increment = movement_data->min_increment;
    positive_index_dir = (increment > 0);

    //update position provider
    get_new_position = movement_data->trajectory_function;

    //flag
    movement_processed_flag = false;

    //Return the number of sub_movements to wait before updating the movement environment
    return sub_movement_queue.available_elements();

}


void SubMovementManager::display_distances() {
    for (int i = 0; i < NB_STEPPERS; i++) {
        CI::echo(String(i) + " ed : " + String(end_distances[i]) + " jd : " + String(jerk_distances[i]));
    }

    for (int i = 0; i < NB_STEPPERS; i++) {
        CI::echo(String(i) + " ed : " + String(end_position[i]) + " jd : " + String(jerk_position[i]));
    }
}



//-----------------------------------Intermediary_Positions_Pre_Computation---------------------------------------------


void SubMovementManager::fill_sub_movement_queue() {

    while (sub_movement_queue.available_spaces() && !movement_processed_flag) {
        push_new_position();
    }
}


bool SubMovementManager::movement_processed() {
    return movement_processed_flag;
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
 *      In all cases, the increment is updated to converge to the distance target.
 *
 *      Notes :
 *          - The distance is given by the maximum of all step_distances on each stepper.
 *          - The second criteria is (far) more restrictive than the first, but gives a better regulation_speed regulation
 *
 */



void SubMovementManager::push_new_position() {
    sub_movement_data_t *sub_movement_data = sub_movement_queue.get_input_ptr();


    //8us 4 steppers, 11us 17 steppers ; 7.07us + 0.23us per stepper
    //get a new sub-movement
    bool b = compute_new_sub_movement(sub_movement_data);

    //5us 4 steppers, 11us 17steppers : 3.15us + 0.46us per stepper
    //validate the sub-movement
    if (b) verify_sub_movement(sub_movement_data);


}


bool SubMovementManager::compute_new_sub_movement(sub_movement_data_t *sub_movement_data) {

    if ((!sub_movement_queue.available_spaces()) || (movement_processed_flag)) {
        return false;
    }


    //------------------Index computing-----------------------------

    //Get the new index candidate;
    float local_index_candidate = index + increment;

    //If the index is close of less than one increment of its limit, go to the end position
    if (((positive_index_dir) && (local_index_candidate + increment > index_limit)) ||
        ((!positive_index_dir) && (local_index_candidate + increment < index_limit))) {
        movement_processed_flag = true;
        local_index_candidate = index_limit;
    }

    //Update the index candidate
    sub_movement_data->index_candidate = local_index_candidate;


    //------------------Position computing-----------------------------

    //Get the high level position corresponding to the computed index
    get_new_position(local_index_candidate, sub_movement_data->candidate_high_level_positions);

    //Translate the high level position into steppers position;
    MachineInterface::translate(sub_movement_data->candidate_high_level_positions,
                                sub_movement_data->future_steppers_positions);

    //Call the kernel, to fill the kernel-reserved part of the data.
    Kinematics::initialise_sub_movement(sub_movement_data);

    return true;
}


void SubMovementManager::verify_sub_movement(sub_movement_data_t *sub_movement_data) {



    //Get the steppers step_distances, high level step_distances, and the maximal stepper and distance
    float max_distance = get_steppers_distances(current_stepper_positions, sub_movement_data);


    //Distance Validity_Verification : fail if an error is detected
    if (distance_bounds_error(max_distance))
        return;


    //Now that validity checks are made, enqueue the sub_movement data
    sub_movement_queue.enqueue();

    //Update the index
    index = sub_movement_data->index_candidate;

    //Update the kernel data
    Kinematics::accept_sub_movement(sub_movement_data);

}


bool SubMovementManager::distance_bounds_error(float max_distance) {

    float up_error = (max_distance >= MAXIMUM_DISTANCE_LIMIT);

    //Increment adjustment according to the target
    if ((uint16_t)max_distance != (uint16_t)DISTANCE_TARGET) {

        //get the distance ratio
        float ratio = (float) DISTANCE_TARGET / max_distance;

        //Update the increment with the ratio;
        increment *= ratio;

        //Notify the KinematicsCore that the increment has been changed (not used in KinematicsCore2)
        Kinematics::update_evolution_coefficient(ratio);

    }

    //If the maximal distance is below the lower limit :
    if (up_error && (!movement_processed_flag)) {
        return true;
    }

    //If the maximal distance is below the lower limit :
    if ((!movement_processed_flag) && (max_distance <= MINIMUM_DISTANCE_LIMIT)) {
        return true;
    }

    return false;

}


/*
 * get_steppers_distances : this function determines the distance between a position and a target, for all stepper.
 *
 * It also computes the direction signature, as step_distances are positive numbers, and saves the maximum stepper
 *      and the maximum distance.
 *
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
            int_dist = (uint8_t) ((uint32_t) p - (uint32_t) d);
        } else {
            int_dist = (uint8_t) ((uint32_t) d - (uint32_t) p);
        }


        //persist step_distances
        real_dists[stepper] = distance;
        int_dists[stepper] = int_dist;

        //Update max dist
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

    uint8_t size = sub_movement_queue.available_elements();

    if (!size) {

        //if the queue is empty : error
        CI::echo("ERROR : IN TrajectoryTracer::read_next_sub_movement : THE SUB_MOVEMENT QUEUE IS EMPTY");

        return 0;

    } else {

        return sub_movement_queue.read_output();

    }

}

void SubMovementManager::discard() {
    sub_movement_queue.discard();
}


/*
 * update_end_position : this function updates the end point position.
 *
 */

void SubMovementManager::update_end_position(const float *const new_hl_position) {

    float stepper_end_position[NB_STEPPERS];

    MachineInterface::translate(new_hl_position, stepper_end_position);

    if (TrajectoryTracer::started) {
        disable_stepper_interrupt();
    }

    for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {
        int32_t d = (int32_t) stepper_end_position[stepper];
        end_distances[stepper] += d - end_position[stepper];
        end_position[stepper] = d;
    }

    if (TrajectoryTracer::started) {
        enable_stepper_interrupt()
    }
}


/*
 * update_jerk_position : this function updates the jerk point position.
 *
 */

void SubMovementManager::update_jerk_position(const int32_t *const new_stepper_position) {


    if (TrajectoryTracer::started) {
        disable_stepper_interrupt();
    }

    for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {
        int32_t d = (int32_t) new_stepper_position[stepper];
        jerk_distances[stepper] += d - jerk_position[stepper];
        jerk_position[stepper] = d;
    }

    if (TrajectoryTracer::started) {
        enable_stepper_interrupt()
    }
}




/*
 * update_end_jerk_distances : this function updates step_distances to end point and jerk point.
 *
 *  It takes in arguments the absolute step_distances on each stepper of the next sub_movement, and their negative signature.
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


#include <config.h>

#undef STEPPER

}

//-----------------------------------------Static declarations - definitions--------------------------------------------

//Static declarations - definitions;

#define m SubMovementManager


//Current stepper position;
float t_cur_pos[NB_STEPPERS]{0};
float *const m::current_stepper_positions = t_cur_pos;

//movement data queue;
Queue<sub_movement_data_t> m::sub_movement_queue(MOVEMENT_DATA_QUEUE_SIZE);


//Indexation variables
float m::index_limit = 0, m::index = 0, m::increment = 0;
bool m::positive_index_dir = false;

//Trajectory function
void (*m::get_new_position)(float, float *);

//Queue state
bool m::movement_processed_flag = false;


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

#include <config.h>

        0};

#undef STEPPER

const sig_t *const m::axis_signatures = k2t_sig;