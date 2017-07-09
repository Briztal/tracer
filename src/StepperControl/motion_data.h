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

        //First_sub_movement
    unsigned char first_signatures[8];//8
    unsigned char last_signatures[8];//8
    //-------------2*4--------------8
    unsigned char beginning_indice;//1
    unsigned char ending_indice;//1

               //Count
    unsigned int count;//2
    // --------------4---------------12

             //processors
    void (*init_processor)();//2
    unsigned char (*position_processor)(unsigned char *);//2
    //--------------4---------------16
    void (*speed_processor)(void);//2
    void (*step)(unsigned char);//2
    //--------------4---------------20
    void (*sub_move_init)(unsigned char);//2

               //Speed
    unsigned int delay_numerator;//2
    //-------------4----------------24
    unsigned int regulation_delay;//2
    unsigned char processing_steps;//1
    unsigned char padding;//1
    //-------------4----------------28
    float ratio;//4
    //-------------4----------------32

} motion_data;//28 bytes




typedef struct{
    unsigned char shift_nb;
    unsigned char direction_signature;
    unsigned char max_axis;
    unsigned char padding;
    //---------------4-----------------
    unsigned char first_pos[NB_STEPPERS];
    unsigned int last_pos[NB_STEPPERS];
    unsigned int slopes[NB_STEPPERS];
    //-----------3*NB_STEPPERS-----------
} linear_data;


#endif //TRACER_MOTION_DATA_H
