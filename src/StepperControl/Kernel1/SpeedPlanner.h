/*
  MotionScheduler.h - Part of TRACER

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
#include <config.h>
#include "../../DataStructures/Queue.h"

#ifdef ENABLE_STEPPER_CONTROL

#ifndef CODE_MOTION_H
#define CODE_MOTION_H

class SpeedPlanner {


public :

    static int32_t *const positions;

    static sig_t *const axis_signatures;//Not used in optimised functions

    static void begin();

    static void set_speed_group(uint8_t speed_group);

    static void set_speed_for_group(uint8_t group_id, float speed);

    static void send_position();

    static float get_adjusted_regulation_speed_linear(float *const relative_distsmm, const float sqrt_square_dist_sum);

    static void pre_set_speed_axis(uint8_t new_axis, float *relative_distsmm, float sqrt_square_dist_sum, int32_t *movement_distances,
                            float regulation_speed, uint8_t processing_steps);

    static void pull_jerk_point();

    static float last_regulation_speed;

    static bool must_watch_for_jerk_points();

private :

    //Scheduling fields

    static Queue<uint8_t> speed_distances_offsets;

    static int32_t *const enqueued_end_distances;

    static int32_t *const next_jerk_distances;

    static float *dist_ratios;

    static float last_steps, last_acceleration;

    //

    static uint8_t speed_group;

    static uint8_t linear_tools_nb;

    static void (**linear_set_functions)(float);

    static float *const theorical_regulation_speeds;

    static sig_t *const speed_groups_signatures;

    static float get_regulation_speed(float *const relative_distsmm, const float sqrt_square_dist_sum);

    static float verifiy_speed_limits(float *const relative_distsmm, const float sqrt_square_dist_sum, float theorical_regulation_speed);

    static float verify_acceleration_limits(float regulation_speed);

    static bool verify_jerk_limits(float *relative_distsmm, float sqrt_square_dist_sum, const float regulation_speed);

    static void push_jerk_point();

    static void add_jerk_distances(int32_t *movement_distances);

};


#endif //CODE_MOTION_H


#endif