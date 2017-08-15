/*
  CSpeedManager.h - Part of TRACER

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

#ifdef ENABLE_STEPPER_CONTROL

#ifndef TRACER_SPEEDMANAGER_H
#define TRACER_SPEEDMANAGER_H



class PreProcessor {

public :

    //--------------------------------------------current_stepper_positions---------------------------------------------

private:
    static int32_t *const current_stepper_positions;

    //-----------------------------------------------sub_movement_queue-------------------------------------------------

private:
    static Queue<sig_t> sub_movement_queue;
    static uint8_t * sub_movement_distances;


    //---------------------------------Intermediary_Positions_Pre_Computation-------------------------------------------

public :

    static bool last_position_popped = false;
    static bool last_sub_move_engaged = false;

    static void push_new_position();

    static uint32_t pop_next_position(uint8_t *elementary_dists);

    static void reset_vars();

private :

    static sig_t (*get_new_position)(float index, float *positions);

    static bool last_position_processed = false;


    //The movement index and its bounds
    static float index_min, index_max, index;

    static float increment;


    //-------------------------------------------Inter-positions distances----------------------------------------------


#define DISTANCE_TARGET 15

#define MINIMUM_DISTANCE_LIMIT 10

#define MAXIMUM_DISTANCE_LIMIT 256


    static bool get_distances(const int32_t *const pos, const int32_t *const dest, uint8_t *const dists,
                              sig_t *dir_dignature_p, uint8_t *max_axis_p, uint8_t *max_distance_p);



    //-----------------------------------------------End_Jerk_Distances-------------------------------------------------


public :

    static void heuristic_end_distance();

    static void heuristic_jerk_distance();

private :

    static int32_t *const end_distances;

    static int32_t *const jerk_distances;

    static uint32_t distance_to_end_point;

    static uint32_t offseted_distance_to_jerk_point;

    static uint32_t speed_offset;

    static volatile bool watch_for_jerk_point;





    //-------------------------------------------------------Actions----------------------------------------------------

public :

    static void updateActions();


private:

    static uint8_t linear_tools_nb;

    static void (**linear_set_functions)(float);



};

#endif //TRACER_SPEEDMANAGER_H

#endif