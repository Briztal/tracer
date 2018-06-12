/*
  JerkPlanner.cpp - Part of TRACER

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


#include <StepperControl/StepperController.h>
#include <StepperControl/_kinematics_data.h>
#include "JerkPlanner.h"
#include "SteppersData.h"

#include <StepperControl/KinematicsCore1/KinematicsCore1.h>
#include <StepperControl/KinematicsCore2/KinematicsCore2.h>

/*
 * save_final_jerk_offsets : this function is called for every movement planned.
 *
 *  It determines and memorises the jerk offsets, for an eventual jerk computation.
 *
 *  This computation will be made using the method compute_jerk, below.
 *
 *  ----------------------------------------- IMPORTANT -----------------------------------------
 *
 *      This step couldn't be called at the same time than the jerk computation, as both function use
 *          pre-process_t trajectory functions, during the jerk ratios computation, and there functions are static.
 *          Those use static data, that is not available during the planning of a movement.
 *
 *          If both movements were lines, and we tried to execute this function at the same time than compute_jerk,
 *          we would be calling the same function (the EXACT same function) for both movements.
 */

void JerkPlanner::save_final_jerk_offsets(movement_data_t *current_movement) {

    //---------------------Stepper positions computation---------------------

    //Cache var for ending index and increment;
    const float ending = current_movement->ending;
    const float ending_increment = current_movement->ending_increment;

    //Cache for the current movement's trajectory function;
    void (*trajectory_function)(float, float *) = current_movement->pre_process_trajectory_function;


    float stepper_positions[NB_STEPPERS];

    //Get the step_distances for the last sub_movement;
    get_stepper_distances(ending, ending - ending_increment, trajectory_function, stepper_positions);

    //---------------------Jerk ratios computation---------------------

    //Cache for speed group;
    const uint8_t speed_group = current_movement->speed_group;

    //Get ending jerk ratios;
    get_ending_jerk_ratios(stepper_positions, speed_group, current_movement->ending_jerk_ratios);

}


/*
 * control_and_initialise_jerk : this function is called for every movement planned, except for the first movement.
 *
 *  It used data computed with the function save_final_jerk_offsets right behind, to determine jerk bounds,
 *
 *      for the current movement's first point.
 */

void JerkPlanner::compute_jerk(const movement_data_t *current_movement, movement_data_t *previous_movement) {

    //First, let's enable the jerk watching for the previous movement;
    previous_movement->jerk_point = true;


    //---------------------Speed Group---------------------

    //The chosen speed group will determine in which referential we calculate the maximum jerk speed;

    //As the deceleration will be made in the previous movement's referential, the previous speed group is chosen;
    const uint8_t speed_group = current_movement->speed_group;


    //---------------------Final Jerk Ratios computation---------------------

    //Local data;
    float final_jerk_ratios[NB_STEPPERS];

    //Update the final jerk ratios with the final sub movement data;
    get_ending_jerk_ratios(saved_final_stepper_positions, speed_group, final_jerk_ratios);


    //---------------------Initial Jerk Ratios pre computation---------------------

    //cache vars for beginning index and increment;
    const float beginning = current_movement->beginning;
    const float beginning_increment = current_movement->beginning_increment;

    //Declare an array that will contain the steppers positions;
    float stepper_positions[NB_STEPPERS];

    //Cache for the current movement's trajectory function;
    void (*trajectory_function)(float, float *) = current_movement->pre_process_trajectory_function;

    //Get the jerk position;
    StepperController::get_stepper_positions_for(trajectory_function, beginning, stepper_positions);

    //cache var for jerk pos;
    int32_t *jerk_pos = previous_movement->jerk_position;

    //Convert to int32_t;
    for (uint8_t steppers = 0; steppers < NB_STEPPERS; steppers++) {
        jerk_pos[steppers] = (int32_t) stepper_positions[steppers];
    }

    //We need an array to contain the jerk distances.
    //For optimisation purposes, we will reuse stepper_positions, that isn't used anymore;
    float *jerk_distances = stepper_positions;

    //Get the step_distances for the first sub_movement;
    get_stepper_distances(beginning, beginning + beginning_increment, trajectory_function, jerk_distances);


    //---------------------Limitation speed computation---------------------

    //Get the initial jerk ratios;
    float speed = get_jerk_point_speed(jerk_distances, speed_group, final_jerk_ratios);


    //---------------------Jerk offsets computation---------------------

    //get the correct jerk offsets for the current movement;
    Kinematics::compute_jerk_offsets(speed, previous_movement);


}


//---------------------------------------------------Private Methods----------------------------------------------------



void JerkPlanner::get_ending_jerk_ratios(float *final_steps_distances, const uint8_t speed_group,
                                         float *final_jerk_ratios) {

    //First, determine the inverse of the high level distance for the sub_movement;
    float distance_inverse =
            1 / StepperController::get_movement_distance_for_group(speed_group, final_steps_distances);

    //Then, update the end ratios :
    for (uint8_t steppers = 0; steppers < NB_STEPPERS; steppers++) {
        final_jerk_ratios[steppers] = final_steps_distances[steppers] / distance_inverse;
    }

}

/*
 * get_stepper_distances : this function gets the algebraic step_distances on each steppers, between the two provided
 *      positions of the provided trajectory.
 */

void JerkPlanner::get_stepper_distances(const float p0, const float p1, void (*trajectory_function)(float, float *),
                                        float *distances) {

    //Local arrays to contain positions;
    float t0[NB_STEPPERS]{0}, t1[NB_STEPPERS]{0};

    //Get the steppers position for the point p0;
    StepperController::get_stepper_positions_for(trajectory_function, p0, t0);

    //Get the steppers position for the point p1;
    StepperController::get_stepper_positions_for(trajectory_function, p1, t1);

    //Get distances;
    for (uint8_t steppers = 0; steppers < NB_STEPPERS; steppers++) {
        distances[steppers] = t1[steppers] - t0[steppers];
    }

}


/*
 * get_jerk_point_speed : this function determines the maximum acceptable speed for the provided jerk point.
 *
 *  It uses the formula that is explained at the bottom of this file.
 *
 */
float JerkPlanner::get_jerk_point_speed(const float *initial_steps_distances, const uint8_t speed_group,
                                        const float *final_jerk_ratios) {

    //First, determine the inverse of th high level distance for the sub_movement;
    float distance_inverse =
            (float) 1 / StepperController::get_movement_distance_for_group(speed_group, initial_steps_distances);

    //The maximum speed;
    float maximum_speed = 0;

    //A flag for the first iteration;
    bool first = true;

    for (uint8_t steppers = 0; steppers < NB_STEPPERS; steppers++) {

        //determine the initial jerk_ratio for the current steppers;
        float initial_jerk_ratio = initial_steps_distances[steppers] * distance_inverse;

        //determine the algebric ratio difference;
        float algebraic_difference = final_jerk_ratios[steppers] - initial_jerk_ratio;

        //extract the absolute difference;
        if (algebraic_difference < 0) algebraic_difference = -algebraic_difference;

        //Get the maximum regulation_speed;
        //Formula : regulation_speed(unit/s) < max_jerk(steps_per_unit/s) / abs( final_ratio(steps_per_unit/unit) - initial_ratio(steps_per_unit/unit) ).
        stepper_data_t *data = SteppersData::steppers_data + steppers;
        float max_speed = data->jerk * data->steps_per_unit / algebraic_difference;

        //Update the maximum regulation_speed;
        maximum_speed = (first) ? maximum_speed : ((maximum_speed < max_speed) ? maximum_speed : max_speed);

        //first steppers is done;
        first = false;

    }

    //return the computed speed;
    return maximum_speed;

}

/*

 FORMULAS FOR JERK REGULATION :

    Definition 1 : global speed V. The global speed is the speed of the machine on a particular speed group.
        This speed has a physical meaning (the speed of a carriage in space for example), has its unit (unit)
        and is regulated during the movement.

    Definition 1 : global distance D. The global distance during a movement is the distance that the machine on
        a particular group of axis, this group axis being a speed group.
        This distance has a physical meaning (the distance a carriage traveled in space for example), has its unit (unit)
        and is controlled during the movement.

     Definition 2 : jerk limit Ji. A steppers mustn't have its speed changing more than its jerk limit in a brief time
        period.

        CoefficientArray (E1) : |vi+ - vi-| < Ji;

    Notations :
        - V- (unit/s)   : global speed before the jerk movement;
        - V+ (unit/s)   : global speed after the jerk movement;
        - D- (unit)     : global distance of the sub movement before the jerk point;
        - D+ (unit)     : global distance of the sub movement after the jerk point;
        - vi- (steps_per_unit/s) : steppers motor i speed before the movement (steps_per_unit/s);
        - vi+ (steps_per_unit/s) : steppers motor i speed after the movement (steps_per_unit/s);
        - si- (steps_per_unit)   : steppers motor i distance for the sub movement before the jerk point;
        - si+ (steps_per_unit)   : steppers motor i distance for the sub movement after the jerk point;
        - Ji (steps_per_unit/s)  : jerk limit on steppers i;
        - t- (s)        : the duration of the sub movement before the jerk point;
        - t+ (s)        : the duration of the sub movement after the jerk point;

    Prerequisites :
        V(unit/s) = D(unit/s) / t(s) <=> t(s) = D(unit) / V(unit/s);
        vi(steps_per_unit/s) = si(steps_per_unit) / t(s);


    Proposition 1 : The global speed before a jerk point and after a jerk point is the same.

        Explanation : The speed variation is regulated by the machine, and can only occur continuously (or, as the
            movement is discrete, during a movement). A jerk point is (as its name says), a point, and the global speed
            must, as a consequence, remain the same before and after.
            This means that only steppers speeds will vary during the jerk.

        ie :  V+ = V- = V;

        Consequence :
            - t+ = D+ / V
            - t- = D- / V

    Definition 3 : targetVector speed. The targetVector speed Vt, for a jerk point, is the maximum speed that allow all steppers to
        stay beyond their jerk limit.


    Determination of Vt :

        For all steppers, we must have

        (E1)    |vi+ - vi-| < Ji
            =>  |si+ / t+ - si- / t-| < Ji
            =>  |si+ / D+ - si- / D-| * Vt < Ji
            =>  Vt < Ji / |si+ / D+ - si- / D-|


    Result :

        Vt is the greater speed that verifies for all steppers the following condition :

            Vt < Ji / |si+ / D+ - si- / D-|




 */


float t_sfsp[NB_STEPPERS];
float *const JerkPlanner::saved_final_stepper_positions = t_sfsp;


#endif
