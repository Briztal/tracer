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
  along with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/


#include "../config.h"

#ifdef ENABLE_STEPPER_CONTROL

#ifndef CODE_MOTION_H
#define CODE_MOTION_H

class MotionScheduler {


public:


public :

    static long *const positions;

    static unsigned char *const axis_signatures;//Not used in optimised functions

    static void begin();

    static void set_speed_group(unsigned char speed_group);

    static void set_speed_for_group(unsigned char group_id, float speed);

    static void send_position();

    static float get_regulation_speed_linear(float *const distsmm, const float sqrt_square_dist_sum);

    static void pre_set_speed_axis(unsigned char new_axis, float distance_coefficient, float regulation_speed, unsigned char processing_steps);


protected :

    static unsigned char speed_group;

    static unsigned char linear_tools_nb;

    static void (**linear_set_functions)(float);

    static float *const theorical_regulation_speeds;

    static unsigned char *const speed_groups_signatures;

};


#endif //CODE_MOTION_H


#endif