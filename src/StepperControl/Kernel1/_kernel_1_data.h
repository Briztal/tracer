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
#ifndef TRACER_MOTION_DATA_H
#define TRACER_MOTION_DATA_H

#include <stdint.h>
#include <config.h>

typedef struct{//TODO REORG
        //First_last_sub_movement
    sig_t initial_signatures[8];//8
    sig_t final_signatures[8];//8
    //-------------2*4--------------8
    uint8_t initial_indice;//1
    uint8_t final_indice;//1
    sig_t initial_dir_signature;//1
    sig_t final_dir_signature;//1
    //-------------4--------------12
               //Count
    uint32_t count;//2
             //processors
    void (*init_processor)();//2
    //--------------4---------------16
    sig_t (*position_processor)(uint8_t *);//2
    void (*speed_processor)(void);//2
    //--------------4---------------20
    void (*step)(sig_t);//2
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

} k1_movement_data_old;//36 bytes



typedef struct {
    float min; //4
    float max;//8
    float increment; //12
    void (*movement_initialisation)(); //16
    void (*movement_finalisation)(); //20
    void (*trajectory_function)(float, float *); //24
    void (*pre_process_trajectory_function)(float, float *); //24

    float ratio;//4
    delay_t delay_numerator;//2
    delay_t regulation_delay;//2
    uint32_t jerk_distance_offset;
    bool jerk_point;//1
    float speed_factor;//2
    sig_t tools_signatures; //30 -> 33

    bool jerk_point = false;
    uint32_t jerk_offsets[NB_STEPPERS]{0};
    int32_t jerk_position[NB_STEPPERS]{0};
    //-----------end : 32 to 36 bytes----------

} k1_movement_data;

typedef struct{
    float slopes[NB_STEPPERS];
    //--------4*NB_STEPPERS------------
    sig_t negative_signatures;
    uint8_t max_axis;
    uint8_t first_pos[NB_STEPPERS];
    //---------------2-----------------
    uint32_t last_pos[NB_STEPPERS];
    //-----------3*NB_STEPPERS-----------
} linear_data;


typedef struct {
    uint8_t *elementary_dists;
    float *real_dists;
    float index_candidate;
    float movement_distance;
    float future_steppers_positions[NB_STEPPERS];
    float candidate_high_level_positions[NB_AXIS];
} k1_position_data_struct;


#endif //TRACER_MOTION_DATA_H
