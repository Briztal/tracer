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
#ifndef TRACER_COMPLEX_MOTION_DATA_H
#define TRACER_COMPLEX_MOTION_DATA_H

#include <stdint.h>
#include <config.h>


/*
typedef struct{//TODO REORG
        //First_last_sub_movement
    sig_t initial_signatures[8];//8
    //-------------2*4--------------8
    uint8_t initial_indice;//1
    sig_t initial_dir_signature;//1
    //-------------4--------------12
             //processors
    void (*movement_initialisation)();//2
    //--------------4---------------16
    sig_t (*trajectory_function)(float, float *);//2
    void (*speed_processor)(void);//2
    //--------------4---------------20
    void (*sub_move_init)(uint8_t);//2
    //-------------4----------------24
               //Speed
    float ratio;//4
    //-------------4----------------28
    delay_t delay_numerator;//2
    delay_t regulation_delay;//2
    uint32_t jerk_distance_offset;
    bool jerk_point;//1
    //-------------4----------------
    float speed_factor;//2
    //-------------4----------------32
    uint8_t processing_steps;//1

} complex_motion_data;//36 bytes
*/

typedef struct{//TODO REORG
    float speed;
    float min;
    float max;
    float increment;
    void (*movement_initialisation)();
    void (*movement_finalisation)();
    void (*trajectory_function)(float, float *);
} complex_motion_data;


typedef struct{
    uint8_t max_axis;
    float offsets[NB_AXIS];
    float slopes[NB_STEPPERS];
} complex_linear_data;


typedef struct{
    float distance;
    sig_t negative_signature;
    bool jerk_point;
    float speed;
} pre_processor_data;


#endif //TRACER_COMPLEX_MOTION_DATA_H
