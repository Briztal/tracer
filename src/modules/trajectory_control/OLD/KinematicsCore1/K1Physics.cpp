/*
  SpeedManager.cpp - Part of TRACER

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

#include <Config/stepper_control_config.h>

#if defined(ENABLE_STEPPER_CONTROL) && (CORE_VERSION == 1)

#include <math.h>
#include <Actuators/PWMGPIO.h>
#include <hardware_language_abstraction.h>
#include <StepperControl/StepperController.h>
#include <EEPROM/EEPROMStorage.h>
#include "K1Physics.h"
#include <StepperControl/SubMovementManager.h>
#include <StepperControl/_kinematics_data.h>
#include "mathProcess.hpp"


void K1Physics::initialise_data() {

    //The reference axis;
    reference_axis = 0;

    //The current speed group;
    last_speed_group = 0;

    //The evolution coefficient
    evolution_coefficient = 1;

    //Thelast evolution coefficient
    saved_evolution_coefficient = 1;

    last_movement_first_sub_movement_hl_distances = 0;

    current_movement_first_sub_movement_hl_distances = 0;


    //The trajectory_control jerk offset;
    jerk_distance_offset = 0;

    //The heuristic distance to the end point;
    heuristic_end_distance = 0;

    //The offset heuristic distance to the next jerk point
    offset_heuristic_jerk_distance = 0;

    //The jerk flag : if true, the jerk point proximity is checked in real time
    watch_for_jerk_point = false;

    //The acceleration_step
    acceleration_step = 0;

    //The acceleration distances's square_root
    acceleration_speed_distance_sqrt = 0;

    //The speed distances
    deceleration_speed_distance = 0;
    acceleration_speed_distance = 0;

    //Delay numerator
    acceleration_delay_numerator = 0;

    //The acceleration distance sqrt fast computer
    acceleration_distance_computer = SqrtFastComputer();

    //The distances ratios.
    deceleration_to_acceleration = 1;
    acceleration_to_deceleration = 1;

    //The current sub_movement time
    sub_movement_time = 0;

    //The current regulation (targetVector) sub_movement time
    regulation_sub_movement_time = 0;

    //A flag to mention if the routine has just started_flag.
    first_movement = true;

    //A flag to mention if the speed currently increases
    speed_increasing_flag = true;

    //A flag to disable the regulation for the current sub_movement.
    no_regulation_flag = false;

    //A flag to enable the speed regulation
    speed_regulation_enabled = true;
}


void K1Physics::initialise_tracing_procedure() {
    //Set the first movement flag;
    first_movement = true;

    //The speed will increase.
    speed_increasing_flag = true;

    //The regulation is enabled
    speed_regulation_enabled = true;

    //The regulation is fully enabled
    no_regulation_flag = false;


}


//---------------------------------------------Movement initialisation----------------------------------------------


/*
 * initialise_kinetics_data : this function determines all kinetics data required by the kinetics regulation, namely :
 *      - step_period_us numerators (acceleration, deceleration);
 *      - conversion ratios for step_period_us numerators (both ways);
 *      - regulation_sub_movement_time;
 *
 */

void K1Physics::initialise_kinetics_data(k1_movement_data *movement_data) {

    //Extract step_period_us numerators and the conversion ratios;
    get_delay_numerator_data(movement_data);

    //compute the regulation step_period_us;
    get_sub_movement_time(movement_data, StepperController::get_speed_group(), StepperController::get_speed());

}

/*
 * load_pre_process_kinetics_data : this function loads a new movement.
 *
 *  It must save the evolution factor, and initialise_hardware the new evolution factor to 1;
 *
 *  It also must save the first sub movement distance in the
 */

void K1Physics::load_pre_process_kinetics_data(const k1_movement_data *movement_data) {
    saved_evolution_coefficient = evolution_coefficient;
    evolution_coefficient = 1;

    //Update the last movement's first sub movement's hl distance;
    last_movement_first_sub_movement_hl_distances = current_movement_first_sub_movement_hl_distances;

    //Save the current movement's first sub movement's hl distance;
    current_movement_first_sub_movement_hl_distances = movement_data->first_sub_movement_hl_distance_current_speed_group;
}


//-------------------------------------------real_time_data update-------------------------------------------

void K1Physics::load_real_time_jerk_data(const k1_movement_data *movement_data) {

    watch_for_jerk_point = movement_data->jerk_point;

    jerk_distance_offset = movement_data->jerk_distance_offset;

}


/*
 * load_real_time_kinetics_data : this function updates the real time data, namely :
 *      - the acceleration step_period_us numerator;
 *      - the acceleration/deceleration translation coefficients;
 *      - the regulation step_period_us;
 *      - the current step_period_us;
 *
 */
void K1Physics::load_real_time_kinetics_data(const k1_movement_data *movement_data) {

    //Reference axis.
    reference_axis = movement_data->reference_axis;

    //step_period_us numerator;
    acceleration_delay_numerator = movement_data->acceleration_delay_numerator;

    //Translation coefficients
    acceleration_to_deceleration = movement_data->dn_a_to_dn_d;
    deceleration_to_acceleration = movement_data->dn_d_to_dn_a;

    //Regulation time
    regulation_sub_movement_time = movement_data->regulation_sub_movement_time;



    //acceleration_step
    acceleration_step = movement_data->acceleration_step;

    //If we are at the first movement, we do not need to convert the current step_period_us.
    if (first_movement) {
        first_movement = false;
        return;
    }

    //Update the flags :
    speed_increasing_flag = sub_movement_time > regulation_sub_movement_time;

    speed_regulation_enabled = true;

    //Update the current time
    /*
     * FORMULA : new_time= (new_sub_movement_hl_distance / last_sub_movement_hl_distance) * current_time
     *
     * with :
     *      - new_time / current_time : the times of the new / current sub_movement;
     *      - new/last_sub_movement_hl_distances : the high level distance for the new/last sub movement.
     *      - the high level distances are expressed in the SAME speed group. if not, the result has no sense.
     *
     * We need to compute the last sub movement hl distance, as the increment may have changed during the movement.
     *
     * FORMULA :
     *      last_sub_movement_hl_distance = (last_increment / first_increment) * first_sub_movement_distance.
     *
     *      the quantity (last_increment / first_increment) is updated during each movement.
     *
     */

    //cache for the new sub movement's hl distance
    float new_sub_movement_hl_distance = movement_data->first_sub_movement_hl_distance_last_speed_group;

    //cache for last movement's first sub_movement hl distance;
    float first_sub_movement_distance = last_movement_first_sub_movement_hl_distances;

    //Get last last_movement's sub movement's hl distance;
    float last_sub_movement_hl_distance = saved_evolution_coefficient * first_sub_movement_distance;

    //Update the regulation time;
    sub_movement_time *= (new_sub_movement_hl_distance / last_sub_movement_hl_distance);

}

void K1Physics::update_evolution_coefficient(float multiplier) {

    evolution_coefficient *= multiplier;

    regulation_sub_movement_time *= multiplier;

    sub_movement_time *= multiplier;

    acceleration_delay_numerator *= multiplier;

}



//--------------------------------------------------Delay numerators----------------------------------------------------



/*
 * get_delay_numerator_data : this function determines :
 *      - the acceleration step_period_us numerator;
 *      - the deceleration step_period_us numerator;
 *      - the square ratios, used to determine one speed distance from the other.
 */

void K1Physics::get_delay_numerator_data(k1_movement_data *movement_data) {

    //TODO ACCELERATION CORRECTION

    //TODO MAX SPEED CHECKING

    //Cache vars
    float min = movement_data->initial, maximum = movement_data->final;
    float min_increment = movement_data->beginning_increment, max_increment = movement_data->ending_increment;

    //Get step_period_us numerators
    float dn_acceleration = get_delay_numerator(movement_data->pre_process_trajectory_function, min,
                                                min + min_increment, &movement_data->reference_axis,
                                                &movement_data->acceleration_step);

    float dn_deceleration = get_delay_numerator(movement_data->pre_process_trajectory_function, maximum,
                                                maximum + max_increment, 0, 0);

    //Save the acceleration step_period_us numerators. The other is not used in real time.
    movement_data->acceleration_delay_numerator = dn_acceleration;


    //--------coefficient to get la from ld--------

    //divide dn_a by dn_d
    float tmp = dn_acceleration / dn_deceleration;

    //FORMULA : la = (Da / Dd)^2 * ld
    movement_data->dn_d_to_dn_a = tmp * tmp;


    //--------coefficient to get ld from la--------

    //divideBy dn_a by dn_d
    tmp = dn_deceleration / dn_acceleration;

    //FORMULA : ld = (Dd / Da)^2 * la
    movement_data->dn_a_to_dn_d = tmp * tmp;

}


/*
 * get_delay_numerator : this function computes the step_period_us numerator for a couple of points and a trajectory function.
 *
 */
float
K1Physics::get_delay_numerator(void (*trajectory_function)(float, float *), float p0, float p1, uint8_t *sav_max_axis,
                               float *sav_max_distance) {

    float t_point[NB_STEPPERS];
    float t_dist[NB_STEPPERS];
    uint8_t max_axis;

    //get the positions for the minimal position
    StepperController::get_stepper_positions_for(trajectory_function, p0, t_point);

    //get the positions for the second minimal position
    StepperController::get_stepper_positions_for(trajectory_function, p1, t_dist);

    //extract the distances and get the maximal_axis.
    max_axis = get_distances(t_point, t_dist);

    if (sav_max_axis != 0) {
        *sav_max_axis = max_axis;
        *sav_max_distance = t_dist[max_axis];
    }

    //finally, compute the step_period_us numerator.
    return _get_delay_numerator(max_axis, t_dist[max_axis]);
}


/*
 * _get_delay_numerator : this function computes (litteraly this time) the step_period_us numerator, for a particular axis.
 *
 *  The step_period_us resulting from the division of this step_period_us numerator by the acceleration distance
 *      is the step_period_us for the SUB_MOVEMENT and not for the tic. That's why the distance in feedrate is
 *      appearing in the formula.
 *
 */

float K1Physics::_get_delay_numerator(uint8_t trajectory_control, float distance) {

    /*
     * FORMULA : D = 10^6 * distance / (feedrate * sqrt(2 * acceleration)) with :
     *      - D : the step_period_us numerator;
     *      - distance (feedrate) : the distance on the trajectory_control;
     *      - feedrate (feedrate/unit) : the feedrate per unit of trajectory_control;
     *      - acceleration (unit / s^2) : the acceleration on the trajectory_control.
     *
     */

    stepper_data_t *data = EEPROMStorage::steppers_data + trajectory_control;

    return (float) 1000000 * distance / (sqrtf(2 * data->feedrate * data->acceleration));
}


/*
 * get_distances : this function computes the distances between positions t0 and t1, and saves them in t1.
 *
 *  It returns the index of the maximal axis.
 */

uint8_t K1Physics::get_distances(const float *const t0, float *const t1) {

    uint8_t max_index = 0;
    float max_dist = 0;

    //get distances in t_dist
    for (uint8_t trajectory_control = 0; trajectory_control < NB_STEPPERS; trajectory_control++) {

        //Get the algebric distance
        float dist = t1[trajectory_control] - t0[trajectory_control];

        //Get the absolute distance
        if (dist < 0) dist = -dist;

        //Save it
        t1[trajectory_control] = dist;

        //Update the maximal axis index.
        if (dist > max_dist) {
            max_index = trajectory_control;
        }
    }

    return max_index;
}


//-------------------------------------------------Sub movement times---------------------------------------------------


/*
 * get_first_sub_movement_time : this function will determine the fist sub movement time.
 *
 *  A particular algorithm is required, as the first sub movement has no predecessor.
 *
 *  A jerk verification in all axis is, as a consequence, required.
 *
 *  This function determines the smallest (or if the smallest is smaller than the regulation time, it'll simply give
 *      return the regulation time) sub movement time that respects the jerk bounds.
 *
 */

float K1Physics::get_first_sub_movement_time(sub_movement_data_t *sub_movement_data) {




    //Cache vars
    float *f_step_distances = sub_movement_data->f_step_distances;

    //The final time
    float min_time = 0;

    for (uint8_t trajectory_control = 0; trajectory_control < NB_STEPPERS; trajectory_control++) {

        stepper_data_t *data = EEPROMStorage::steppers_data+trajectory_control;
        float maximum_speed = data->feedrate * data->jerk;

        //update the minimum time :
        //Formula : low_time_bound = stepper_distance / maximum_speed
        float down_time = f_step_distances[trajectory_control] / maximum_speed;

        //update minimum time, as the maximum of the new time and the current initial time :
        min_time = (down_time < min_time) ? min_time : down_time;

    }

    float min_time_us = (float) 1000000 * min_time;


    sub_movement_time = (uint32_t) ((regulation_sub_movement_time > min_time_us) ? regulation_sub_movement_time
                                                                                 : min_time_us);

    return sub_movement_time;

}


/*
 * get_sub_movement_time : this function determines the first sub movement time for a particular movement.
 *
 *  It is called during a movement's preprocessing.
 *
 */

void K1Physics::get_sub_movement_time(movement_data_t *movement_data, uint8_t speed_group, float speed) {

    float t_point[NB_STEPPERS];
    float t_dist[NB_STEPPERS];

    float min = movement_data->initial, min_increment = movement_data->beginning_increment;

    //get the positions for the minimal position
    (*movement_data->pre_process_trajectory_function)(min, t_point);

    //get the positions for the second minimal position
    (*movement_data->pre_process_trajectory_function)(min + min_increment, t_dist);

    //extract the distances and get the maximal_axis.
    get_distances(t_point, t_dist);

    //get and memorise the high level distance on and last speed groups.
    float hl_sub_movement_distance_current_speed_group = StepperController::get_movement_distance_for_group(speed_group,
                                                                                                           t_dist);
    movement_data->first_sub_movement_hl_distance_current_speed_group = hl_sub_movement_distance_current_speed_group;

    //get and memorise the high level distance on last speed groups.
    if (!first_movement) {
        float hl_sub_movement_distance_last_speed_group = StepperController::get_movement_distance_for_group(
                last_speed_group, t_dist);
        movement_data->first_sub_movement_hl_distance_last_speed_group = hl_sub_movement_distance_last_speed_group;
    }


    last_speed_group = speed_group;

    movement_data->regulation_sub_movement_time = (uint32_t) ((float) 1000000 *
                                                              hl_sub_movement_distance_current_speed_group / speed);

}


//------------------------------------------------End - jerk distances-------------------------------------------------


/*
 * TODO
 */

void K1Physics::update_heuristic_end_distance() {

    int32_t td;
    uint32_t distance = 0;
    uint32_t dist = 0;

#define STEPPER(i, ...) \
    td = SubMovementManager::end_distances[i];\
    distance=((uint32_t) (td<0) ? -td : td );\
    if (distance>dist) dist = distance;

#include <Config/stepper_control_config.h>

#undef STEPPER

    heuristic_end_distance = dist;

}

/*
 * TODO
 */

void K1Physics::update_heuristic_jerk_distance() {

/*
#define STEPPER(i, ...) \
    td = SubMovementManager::jerk_distances[i];\
    distance=((uint32_t) (td<0) ? -td : td );\
    if (distance>dist) dist = distance;

#include <config_files.h>

#undef STEPPER
    */

    int32_t dist = SubMovementManager::jerk_distances[reference_axis];
    if (dist < 0) dist = -dist;

    offset_heuristic_jerk_distance = (uint32_t) dist + jerk_distance_offset;


}


void K1Physics::compute_jerk_offsets(float speed, k1_movement_data *previous_movement) {

    //-----------maximum trajectory_control speeds------------

    //Now that we know the maximum regulation_speed, we can determine the deceleration step_distances :

    //Cache var for the reference axis :
    uint8_t ref_stepper = previous_movement->reference_axis;

    //Cache var for ending_jjerk_ratios
    float *final_jerk_ratios = previous_movement->ending_jerk_ratios;

    //Determine the speed we must be at the end of the previous movement, on the reference axis
    float stepper_speed = speed * final_jerk_ratios[ref_stepper];

    //Determine the offset on the deceleration distance
    stepper_data_t *data = EEPROMStorage::steppers_data+ref_stepper;
    previous_movement->jerk_distance_offset = (uint32_t)
            (stepper_speed * stepper_speed / (2 * data->feedrate * data->acceleration));


}


bool K1Physics::regulate_speed() {

    //CI::echo("jerk : "+string(watch_for_jerk_point)+" - en : "+string(speed_regulation_enabled)+" - inc : "+string(speed_increasing_flag)+" - sub : "+string(sub_movement_time)+" - reg : "+string(regulation_sub_movement_time));
    //CI::echo("jerk : "+string(watch_for_jerk_point) +" - jd : "+string(offset_heuristic_jerk_distance)+" of : "+string(jerk_distance_offset));

    //CI::echo("acc : "+string(acceleration_speed_distance)+" - dec : "+string(deceleration_speed_distance));

    if (heuristic_end_distance < deceleration_speed_distance) {
        //If the machine must decelerate_of, due to the proximity of the end point :



        //decelerate_of
        decelerate_to(heuristic_end_distance);

        //Re-enable the regulation
        speed_regulation_enabled = true;

        //setup the regulation for an acceleration
        speed_increasing_flag = true;

        //Disable the
        no_regulation_flag = true;

        //The sub_movement time was updated.
        return true;

    } else if ((watch_for_jerk_point) && (offset_heuristic_jerk_distance < deceleration_speed_distance)) {
        //If the machine must decelerate_of, due to the proximity of the jerk point :

        //decelerate_of
        decelerate_to(offset_heuristic_jerk_distance);

        //Re-enable the regulation
        speed_regulation_enabled = true;

        //setup the regulation for an acceleration
        speed_increasing_flag = true;

        //Disable the
        no_regulation_flag = true;

        //The sub_movement time was updated.
        return true;

    } else if (speed_regulation_enabled) {
        //If regulation_speed still has to be modified :

        //If we decelerated during the last sub_movement, we do not correct the regulation_speed for this one.
        if (no_regulation_flag) {

            //disable the flag for the next sub_movement
            no_regulation_flag = false;

            //The sub_movement time was not updated.
            return false;

        }

        bool low_speed = (sub_movement_time > regulation_sub_movement_time);


        //We must verify that the targetVector hasn't been reached
        //Reached when the regulation_speed is too high and has been increased before, or too low and has been decreased before
        if (low_speed != speed_increasing_flag) {

            //Set the step_period_us to the targetVector
            sub_movement_time = regulation_sub_movement_time;

            //Updating the acceleration distance;
            acceleration_speed_distance_sqrt = (uint16_t) (acceleration_delay_numerator / sub_movement_time);
            acceleration_speed_distance = acceleration_speed_distance_sqrt * acceleration_speed_distance_sqrt;
            update_deceleration_distances();

            //mark the targetVector as reached
            speed_regulation_enabled = false;

            //The sub_movement time was updated.
            return false;

        } else if (low_speed) {
            //if regulation_speed is still too low :


            //acceleration
            accelerate_of(acceleration_step);

            //mark the regulation_speed as increasing
            speed_increasing_flag = true;

            //The sub_movement time was updated.
            return true;

        } else {
            //if regulation_speed is still too high :

            //deceleration
            decelerate_of(acceleration_step);

            //mark the regulation_speed as decreasing
            speed_increasing_flag = false;

            //The sub_movement time was updated.
            return true;

        }

    }

    return false;
}


/*
 * decelerate_to : this function updates the deceleration distance with the given distance.
 *
 *  It then updates the acceleration distance and its square root.
 *
 */

void K1Physics::decelerate_to(uint32_t deceleration_distance) {
    deceleration_speed_distance = deceleration_distance;
    update_acceleration_distances();
}


/*
 * accelerate_of : this function increases the acceleration distance of the given value.
 *
 *  It then updates the deceleration distance and the acceleration distance'ssquare root.
 *
 */

void K1Physics::accelerate_of(float distance_step) {

    acceleration_speed_distance += distance_step;
    update_deceleration_distances();

}


/*
 * decelerate_of : this  function decreases the acceleration distance of the given value.
 *
 *  It then updates the deceleration distance and the acceleration distance'ssquare root.
 */

void K1Physics::decelerate_of(float distance_step) {
    acceleration_speed_distance -= distance_step;
    update_deceleration_distances();

}


/*
 * update_acceleration_distances : this function updates the following variables, according to the current deceleration
 *  distances :
 *      - the acceleration distance;
 *      - the acceleration distance's square root;
 */

void K1Physics::update_acceleration_distances() {
    acceleration_speed_distance = deceleration_speed_distance * deceleration_to_acceleration;
    acceleration_speed_distance_sqrt = acceleration_distance_computer.sqrt_fast((uint32_t) acceleration_speed_distance);

}


/*
 * update_deceleration_distances : this function updates the following variables, according to the current deceleration
 *  distances :
 *      - the acceleration distance's square root;
 */

void K1Physics::update_deceleration_distances() {
    deceleration_speed_distance = acceleration_speed_distance * acceleration_to_deceleration;
    acceleration_speed_distance_sqrt = acceleration_distance_computer.sqrt_fast(
            (uint32_t) acceleration_speed_distance);//TODO BAD RESOLUTION

}


/*
 * get_sub_movement_time : this function updates the sub_movement time.
 *
 *  FORMULA : sub_movement_time = acceleration_delay_numerator / sqrt(acceleration_speed_distance)
 *
 */
float K1Physics::get_sub_movement_time(bool update) {

    if (update) {
        //Update the sub movement time;
        sub_movement_time = (delay_t) (acceleration_delay_numerator * invert(acceleration_speed_distance_sqrt));

    }

    //Return the sub_movement time;
    return sub_movement_time;
}


//-----------------------------------------Static declarations - definitions--------------------------------------------

#define m K1Physics

uint8_t m::reference_axis = 0;

uint8_t m::last_speed_group = 0;

float m::evolution_coefficient = 1;

float m::saved_evolution_coefficient = 1;

float m::last_movement_first_sub_movement_hl_distances = 0;

float m::current_movement_first_sub_movement_hl_distances = 0;


//The trajectory_control jerk offset;
uint32_t m::jerk_distance_offset;

//The heuristic distance to the end point;
uint32_t m::heuristic_end_distance;

//The offseted heuristic distance to the next jerk point
uint32_t m::offset_heuristic_jerk_distance;

//The jerk flag : if true, the jerk point proximity is checked in real time
volatile bool m::watch_for_jerk_point;


//The acceleration_step
float m::acceleration_step = 0;

//The acceleration distances's square_root
uint16_t m::acceleration_speed_distance_sqrt = 0;

//The speed distances
float m::deceleration_speed_distance = 0;
float m::acceleration_speed_distance = 0;


//Delay numerator
float m::acceleration_delay_numerator = 0;

//The acceleration distance sqrt fast computer
SqrtFastComputer m::acceleration_distance_computer;

//The distances ratios.
float m::deceleration_to_acceleration = 1;
float m::acceleration_to_deceleration = 1;

//The current sub_movement time
delay_t m::sub_movement_time = 0;

//The current regulation (targetVector) sub_movement time
delay_t m::regulation_sub_movement_time = 0;

//A flag to mention if the routine has just started_flag.
bool m::first_movement = true;

//A flag to mention if the speed currently increases
bool m::speed_increasing_flag = true;

//A flag to disable the regulation for the current sub_movement.
bool m::no_regulation_flag = false;

//A flag to enable the speed regulation
bool m::speed_regulation_enabled = true;


#undef m


#endif