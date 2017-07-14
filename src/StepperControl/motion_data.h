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
  along with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/
#ifndef TRACER_MOTION_DATA_H
#define TRACER_MOTION_DATA_H


#include "../config.h"

typedef struct{
        //First_last_sub_movement
    unsigned char initial_signatures[8];//8
    unsigned char final_signatures[8];//8
    //-------------2*4--------------8
    unsigned char initial_indice;//1
    unsigned char final_indice;//1
    unsigned char initial_dir_signature;//1
    unsigned char final_dir_signature;//1
    //-------------4--------------12
               //Count
    unsigned int count;//2
             //processors
    void (*init_processor)();//2
    //--------------4---------------16
    unsigned char (*position_processor)(unsigned char *);//2
    void (*speed_processor)(void);//2
    //--------------4---------------20
    void (*step)(unsigned char);//2
    void (*sub_move_init)(unsigned char);//2
    //-------------4----------------24
               //Speed
    float ratio;//4
    //-------------4----------------28
    unsigned int delay_numerator;//2
    unsigned int regulation_delay;//2
    //-------------4----------------32
    unsigned char processing_steps;//1
    unsigned char padding;//1
    //-------------2----------------34

} motion_data;//36 bytes

typedef struct{
    unsigned char shift_nb;
    unsigned char max_axis;
    //---------------2-----------------
    unsigned int last_pos[NB_STEPPERS];
    unsigned int slopes[NB_STEPPERS];
    unsigned char first_pos[NB_STEPPERS];

    //-----------3*NB_STEPPERS-----------
} linear_data;


#endif //TRACER_MOTION_DATA_H
