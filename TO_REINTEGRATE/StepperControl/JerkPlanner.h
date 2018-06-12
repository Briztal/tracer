/*
  JerkPlanner.h - Part of TRACER

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

/*
 * The JerkPlanner class. This class handles the jerk planning, and for this, has two public functions :
 *
 *  - save_final_jerk_offsets : this function is called for every movement planned. Its goal is to prepare the jerk
 *      computation for this movement's ending point. The eventual computation will eventually occur if another
 *      movement is planned right after this one, if the movement procedure is not ended.
 *
 *  - compute_jerk : this function called when a movement is being planned, and almost another one has already been
 *      planned. It handles the actual jerk computation for the beginning point of this movement, which explains why
 *      it is not called for the first movement planned, whose first point begins the movement procedure (no jerk).
 *
 *  ----------------------------------------- IMPORTANT -----------------------------------------
 *
 *      The save_final_jerk_offsets function  couldn't be called at the same time than the jerk computation,
 *          as both function use pre-process_t trajectory functions, during the jerk ratios computation, and there
 *          functions are static. Those use static data, that is not available during the planning of a movement.
 *
 *          If both movements were lines, and we tried to execute this function at the same time than compute_jerk,
 *          we would be calling the same function (the EXACT same function) for both movements.
 *
 */

#ifndef TRACER_JERKPLANNER_H
#define TRACER_JERKPLANNER_H

#include <config.h>

#ifdef ENABLE_STEPPER_CONTROL

#include "_core_include.h"

class JerkPlanner {


public :

    //Memorise a the current movement's final point jerk offsets.
    static void save_final_jerk_offsets(movement_data_t *current_movement);

    //compute the jerk offsets using previously saved jerk offsets.
    static void compute_jerk(const movement_data_t *current_movement, movement_data_t *previous_movement);


private :

    //Get the steppers distances between positions indexed by p0 and p1 over the trajectory function.
    static void get_stepper_distances(const float p0, const float p1, void (*trajectory_function)(float, float *),
                                      float *distances);

    //Get the final jerk ratios
    static void get_ending_jerk_ratios(float *final_steps_distances, const uint8_t speed_group,
                                       float *final_jerk_ratios);

    //Get the maximum acceptable speed at a jerk point
    static float get_jerk_point_speed(const float *initial_steps_distances, const uint8_t speed_group,
                                                   const float *final_jerk_ratios);

    //Array containing
    static float *const saved_final_stepper_positions;



private:


};


#endif //TRACER_JERKPLANNER_H

#endif