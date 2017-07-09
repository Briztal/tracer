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
    static void begin();


    //----------------------------------------------IMPLEMENTED METHODS-------------------------------------------------

public :

    static void setSpeed(float speed);

protected:

    //StepperControl functions assingment according to axis_t


    //-----------------------------------------------FIELDS-------------------------------------------------------------

public :

    static long *const positions;

    static unsigned char *const axis_signatures;

    static void send_position();

    static float get_regulation_speed(float *const distsmm, const float inv_sqrt_square_dist_sum);

    static void pre_set_speed_axis(unsigned char new_axis, float distance_coefficient, float regulation_speed);


protected :

    static float theorical_regulation_speed; //mm per second

    static unsigned char linear_tools_nb;

    static void (**linear_set_functions)(float);

};


#endif //CODE_MOTION_H


#endif