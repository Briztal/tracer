/*
  RealTimeProcessor.h - Part of TRACER

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

#include <stdint.h>
#include "../../config.h"
#include "../../DataStructures/Queue.h"
#include "complex_motion_data.h"

#ifdef ENABLE_STEPPER_CONTROL

#ifndef TRACER_SPEEDMANAGER_H
#define TRACER_SPEEDMANAGER_H


class RealTimeProcessor {

public:
    static void start();

    //--------------------------------------------current_stepper_positions---------------------------------------------

private:

    static float *const current_stepper_positions;
    static float *const current_hl_position;



public :

    static void send_position();


    //-----------------------------------------------Sub_movement_queue-------------------------------------------------

private:

    static Queue<pre_processor_data> sub_movement_queue;
    static uint8_t *sub_movement_int_distances;
    static float *sub_movement_real_distances;

public :
    static uint8_t elements() {
        return sub_movement_queue.available_elements();
    }
    //--------------------------------------------Positions_Computation-------------------------------------------------

private :

    //The movement index and its bounds
    static float index_min, index_max, index;

    static uint8_t movement_speed_group;

    static float increment;

    static void (*get_new_position)(float index, float *positions);

public :

    static void initialise_movement(float min, float max, float increment,
                                    void (*trajectory_function)(float index, float *positions));

    //---------------------------------------High_level_and_Low_level_distances-----------------------------------------


#define MINIMUM_DISTANCE_LIMIT 10

#define MAXIMUM_DISTANCE_LIMIT 256

    static const int8_t *const speed_groups_indices;

    static float get_movement_distance_for_group(uint8_t speed_group, const float *const distances);

    static bool get_steppers_distances(float *const pos, const float *const dest, uint8_t *const int_dists, float *const real_dists,
                                       sig_t *dir_dignature_p, uint8_t *max_axis_p, uint8_t *max_distance_p);

//---------------------------------------------Pre_Computed_Positions_storage-------------------------------------------

public :

    static bool last_position_popped;

    static bool last_position_processed;

    static void push_new_position();

    static void fill_sub_movement_queue();

    static void pop_next_position(uint8_t *elementary_dists, float *real_dists, sig_t *negative_signature, float *distance);

    static void reset_vars();


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


public :

    //End distances updating
    static void update_end_distances(const sig_t negative_signatures, const uint8_t *elementary_dists);

    static void update_end_position(const float *const new_hl_position);

private :

    static int32_t *const end_distances;
    static int32_t *const end_position;

    //------------------------------------------------Speed_Management--------------------------------------------------

public :

    static void set_regulation_speed(uint8_t speed_group, float speed);

    static void set_regulation_speed_jerk(uint8_t speed_group, float new_speed);

    static void update_speeds(const float *const stepper_distances, float time);

    static float pre_process_speed(float movement_distance, const float *const stepper_distances);


private :

    static bool jerk_point;

    static bool next_push_jerk;

    static float next_regulation_speed;



    //Deceleration Fields,  computed during the heuristic calls;

    static bool deceleration_required;

    //Speed fields.

    static float regulation_speed;
    static float last_time;

    static float *const steppers_speeds;

    static uint32_t *const deceleration_distances;

    static const sig_t *const axis_signatures;


    //-------------------------------------------------------Actions----------------------------------------------------

public :

    static void updateActions();


private:

    static uint8_t linear_tools_nb;

    static void (**linear_set_functions)(float);

    static void update_current_hl_position(float *new_hl_position);
};

#endif //TRACER_SPEEDMANAGER_H

#endif