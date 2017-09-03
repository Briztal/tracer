/*
  SpeedManager.h - Part of TRACER

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
#include <StepperControl/_kinematics_data.h>
#include <DataStructures/Queue.h>
#include "SqrtFastComputer.h"

#ifdef ENABLE_STEPPER_CONTROL

#ifndef TRACER_REAL_TIME_PROCESS_H
#define TRACER_REAL_TIME_PROCESS_H

class K1RealTimeProcessor {

public :


    //---------------------------------------------------End_Distances--------------------------------------------------


    /*  End Distances Conventions :
    *      pos              end_distance>0        destination   -> +
    *      destination      end_distance<0        pos           -> +
    *
    *  if movement direction is
    *      positive, (-> +), end distance decreases
    *      negative, (- <-), end distance increases
    *
    *  if new destination is
    *      greater than the position, (-> +), end distance increases
    *      lesser  than the position, (- <-), end distance decreases
    */

    /*  Axis direction conventions :
     *      1    : negative direction (- <-)
     *      0   : positive direction (-> +)
     */

    //End step_distances update
    static void update_end_jerk_distances(const sig_t negative_signatures, const uint8_t *elementary_dists);

    //End position update
    static void update_end_position(const float *const new_hl_position);

    //Jerk position update
    static void update_jerk_position(const int32_t *const new_hl_position);

    //Jerk offsets update
    static void update_jerk_offset(const uint32_t *const new_jerk_offsets);


    static void update_heuristic_end_distance();

    static void update_heuristic_jerk_distance();

private :

    //The stepper end positions;
    static int32_t *const end_position;

    //the stepper end step_distances;
    static int32_t *const end_distances;

    //The stepper jerk positions;
    static int32_t *const jerk_position;

    //the stepper jerk step_distances;
    static int32_t *const jerk_distances;

    //The stepper jerk offset;
    static uint32_t jerk_offset;

    //The heuristic distance to the end point;
    static uint32_t heuristic_end_distance;

    //The offseted heuristic distance to the next jerk point
    static uint32_t offseted_heuristic_jerk_distance;

    //-----------------------------------------------Sub_movement_queue-------------------------------------------------

private:

    //The sub_movements queue
    static Queue<k1_real_time_data> sub_movement_queue;

    //The arrays to store real and integer step_distances
    static float *sub_movement_real_distances;
    static uint8_t *sub_movement_int_distances;

public :

    //A simple method to return the number of available_sub_movements currently in the sub_movement queue.
    static uint8_t available_sub_movements() {
        return sub_movement_queue.available_elements();
    }

    //------------------------------------------------Speed_Management--------------------------------------------------

private:

    //speed distance management

    static void set_speed_distance(uint32_t distance);

    static void accelerate(uint8_t elapsed_distance);

    static void decelerate(uint8_t elapsed_distance);

    static void update_distance_sqrt();

    static uint16_t distance_square_root;

    //Speed distance sqrt fast computer
    static SqrtFastComputer distance_sqrt_computer;

    static volatile bool watch_for_jerk_point;


public :


    static void regulate_speed(uint8_t);

    static void update_delay_0();


    static void init_speed_management(delay_t tmp_delay_0, delay_t tmp_delay_numerator, float speed_factor, float ratio,
                                      bool jerk_point, uint32_t jerk_distance_offset);


    static bool delay0_update_required;

    static void begin();


private :

    static delay_t delay0;

    static delay_t regulation_delay;

    static bool speed_increasing;

    static bool no_regulation_flag;

    static bool speed_regulation_enabled;

    static uint32_t speed_distance;

    static delay_t delay_numerator;

    static float speed_factor;


    static uint8_t linear_tools_nb;

    static void (**linear_set_functions)(float);
};

#endif //TRACER_REAL_TIME_PROCESS_H

#endif