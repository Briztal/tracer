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

#include "../config.h"

#ifdef ENABLE_STEPPER_CONTROL

#ifndef TRACER_SPEEDMANAGER_H
#define TRACER_SPEEDMANAGER_H

class SpeedManager {

public :

    static delay_t delay0;

    static uint32_t speed_offset;

    static int32_t *const end_distances;
    static int32_t *const jerk_distances;


    static void heuristic_jerk_distance();

public :

    //static int32_t *const end_distances;

    static void heuristic_end_distance();

    static void regulate_speed();

    static void update_delay_0();

    static void set_speed_distance(uint32_t distance_to_end);

    static void go_to_speed_distance(uint32_t distance_to_end);

    static void set_speed_distance_fast(bool dir, uint16_t increment);


    static void init_speed_management(delay_t tmp_delay_0, delay_t tmp_delay_numerator, float speed_factor, float ratio,
                                      uint8_t processing_steps, bool jerk_point, uint32_t jerk_distance_offset);

    static uint16_t distance_square_root;

    static bool delay0_update_required;

    static void print_speed_distance();

    static void begin();

    static void setActionsSpeeds();

    static uint16_t square_increments;

    static void updateActions();



private :

    static bool watch_for_jerk_point;

    static uint8_t processing_steps;

    static uint32_t distance_to_end_point;
    static uint32_t offseted_distance_to_jerk_point;

    static delay_t regulation_delay;
    static bool speed_incr;
    static bool regulation_stop_enabled;

    static bool regulation_unreached;
    static uint32_t speed_distance;
    static uint16_t square_inf, square_sup;

    static delay_t delay_numerator;
    static float speed_factor;


    static uint8_t linear_tools_nb;

    static void (**linear_set_functions)(float);

};

#endif //TRACER_SPEEDMANAGER_H

#endif