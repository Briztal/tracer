/*
  motion_data.h - Part of TRACER

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


#ifndef TRACER_COMPLEX_MOTION_DATA_H
#define TRACER_COMPLEX_MOTION_DATA_H

#include <stdint.h>
#include <config.h>

//TODO REORG AND COMMENT

typedef struct{
    float speed;
    uint8_t speed_group;
    float min;
    float max;
    float increment;
    void (*movement_initialisation)();
    void (*movement_finalisation)();
    void (*trajectory_function)(float, float *);
    sig_t action_signatures;
} k2_movement_data;



typedef struct{
    float distance;
    sig_t negative_signature;
    bool jerk_point;
    float speed;
} k2_real_time_data;



typedef struct{
    uint8_t max_axis;
    float offsets[NB_AXIS];
    float slopes[NB_STEPPERS];
} k2_linear_data;


#endif //TRACER_COMPLEX_MOTION_DATA_H

#endif