/*
  Kernel2.cpp - Part of TRACER

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

#if defined(ENABLE_STEPPER_CONTROL) && (KERNEL == 2)

#include "Kernel2.h"
#include "K2RealTimeProcessor.h"
#include <StepperControl/TrajectoryTracer.h>
#include "JerkPlanner.h"

#include <StepperControl/StepperController.h>
#include <hardware_language_abstraction.h>
#include <StepperControl/MachineInterface.h>


void Kernel2::initialise_tracing_procedure() {
    K2RealTimeProcessor::start();
}

//-----------------------------------------------Kernel status flags------------------------------------------------


//The function to call to verify that sub_movements are available in the queue.
uint8_t Kernel2::available_sub_movements() {

    return K2RealTimeProcessor::available_sub_movements();

}

//The function to call to know if the current movement has been processed.
bool Kernel2::movement_processed() {

    return K2RealTimeProcessor::movement_processed;

}


//---------------------------------------------Current movement update----------------------------------------------


/*
 * initialise_movement_data : updates the kernel2 specific data in the provided movement container;
 *
 *  It initialises :
 *      - the speed;
 *      - the speed group;
 */

void Kernel2::initialise_movement_data(k2_movement_data *movement_data) {

    //Initialisation of speed variables
    movement_data->speed = MachineInterface::get_speed();
    movement_data->speed_group = MachineInterface::get_speed_group();

}

/*
 * compute_jerk_data : updates the previous movement's jerk data, according to the current movement.
 *
 *  This task is delegated to the JerkPlanner class, as the Jerk determination algorithm is a bit heavy;
 */

void Kernel2::compute_jerk_data(const k2_movement_data *current_movement, k2_movement_data *previous_movement)  {

    //Do all the pre-processing for the movement, and throw the eventual error
    JerkPlanner::determine_jerk(current_movement, previous_movement);

}

/*
 * update_current_movement : this function updates the movement that Kernel2 is currently pre-processing.
 */

uint8_t Kernel2::update_current_movement(k2_movement_data *movement_data) {

    //Update the trajectory variables
    K2RealTimeProcessor::initialise_movement(movement_data->min, movement_data->max, movement_data->min_increment, movement_data->trajectory_function);

    //Update the speed according to the movement type
    K2RealTimeProcessor::set_regulation_speed(movement_data->speed_group, movement_data->speed);

    //Return the number of sub_movements to wait before updating the movement environment
    return K2RealTimeProcessor::available_sub_movements();

}


//------------------------------------------------environment update------------------------------------------------



void Kernel2::update_movement_environment(k2_movement_data *movement_data) {

    //Jerk
    if (movement_data->jerk_point) {
        update_jerk_environment(movement_data);
    }

    //Send position
    K2RealTimeProcessor::send_position();

}

/*
 * update_jerk_environment : this function actualises the jerk environment for the current move
 *      with the provided data.
 */

void Kernel2::update_jerk_environment(k2_movement_data *movement_data) {

    K2RealTimeProcessor::update_jerk_position(movement_data->jerk_position);
    K2RealTimeProcessor::update_jerk_offsets(movement_data->jerk_offsets);

}


//--------------------------------------------sub_movements preparation---------------------------------------------


/*
 * prepare_first_sub_movement : this function sets all variables up for the movement routine.
 *
 *  It processes the first sub_movement, and then fills the sub_movement queue;
 *
 */
void Kernel2::prepare_first_sub_movement(uint8_t *elementary_distances, sig_t *negative_signature, float *time) {
    //Push the first position;
    K2RealTimeProcessor::push_new_position();

    //start by initialising vars for processing
    float real_dists[NB_STEPPERS];
    float distance = 0;

    //pop the stored position
    K2RealTimeProcessor::pop_next_position(elementary_distances, real_dists, negative_signature, &distance);


    //Update end distances with the computed distances.
    K2RealTimeProcessor::update_end_jerk_distances(*negative_signature, elementary_distances);

    //update the speeds
    *time = K2RealTimeProcessor::get_first_sub_movement_time(distance, real_dists);

    K2RealTimeProcessor::update_speeds(real_dists, *time);

    //Push as much sub_movements as possible.
    K2RealTimeProcessor::fill_sub_movement_queue();

}


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

void Kernel2::prepare_next_sub_movement(uint8_t *elementary_distances, sig_t *negative_signatures, float *time) {
    //Disable the stepper interrupt for preventing infinite call (causes stack overflow)
    disable_stepper_interrupt();

    float real_dists[NB_STEPPERS];
    float distance = 0;
    

    //Step 1 : Get a new position to reach
    K2RealTimeProcessor::pop_next_position(elementary_distances, real_dists, negative_signatures, &distance);


    //Step 2 : Update the end_distances with this distances array and compute the heuristic distances to jerk/end points
    K2RealTimeProcessor::update_end_jerk_distances(*negative_signatures, elementary_distances);

    STEP_AND_WAIT;

    //4us 4 steppers, 13us 17 steppers : 1.23us + 0.7 per stepper

    //Step 3 : Update the speed distance with the new heuristic distances
    *time = K2RealTimeProcessor::get_sub_movement_time(distance, real_dists);


    STEP_AND_WAIT;

    //5us 4 steppers, 9us 17 steppers : 3.76us + 0.3us per stepper

    //Step 4 : Update the steppers speeds
    K2RealTimeProcessor::update_speeds(real_dists, *time);

    //Update tools powers
    float current_speed = distance / *time;
    ComplexTrajectoryExecuter::update_tools_powers(current_speed);//TODO NOT KERNEL'S JOB! THE KERNEL MUST ONLY PROVIDE THE SPEED.

    //If no more pre-process is required
    if (K2RealTimeProcessor::movement_processed) return;

    STEP_AND_WAIT;

    //8us 4 steppers, 11us 17 steppers ; 7.07us + 0.23us per stepper
    //Step 7 : get a new position part 1
    K2RealTimeProcessor::push_new_position_1();//8us 4 steppers, 11us 17 steppers

    STEP_AND_WAIT;

    //5us 4 steppers, 11us 17steppers : 3.15us + 0.46us per stepper
    //get a new position part 2
    K2RealTimeProcessor::push_new_position_2();

    STEP_AND_WAIT;

    //8us 4 steppers, 11us 17 steppers ; 7.07us + 0.23us per stepper
    //Step 8 : if the position queue is not full, get a new position part 1;
    K2RealTimeProcessor::push_new_position_1();//8us

    STEP_AND_WAIT;

    //5us 4 steppers, 11us 17steppers : 3.15us + 0.46us per stepper
    //get a new position part 2
    K2RealTimeProcessor::push_new_position_2();//5us

    return;
}




//----------------------------------------------------positon log---------------------------------------------------



void Kernel2::send_position() {
    K2RealTimeProcessor::send_position();
}




#endif