/*
  ComplexLinearMovement.cpp - Part of TRACER

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

#ifndef TRACER_LINEAR_MOVEMENT_H
#define TRACER_LINEAR_MOVEMENT_H


#include <DataStructures/Queue.h>
#include "MovementKernels/Kernel2/_kernel_2_data.h"

class ComplexLinearMovement {

//--------------------------------------------------Pre-Processing--------------------------------------------------

public:

    //The function to call to trace a line from the current position to the destination provided in argument.
    static void prepare_movement(const float *const destination);


private :

    //offsets, slopes, and max_axis used during increment computation
    static float *pre_process_offsets;
    static float *pre_process_slopes;
    static uint8_t pre_process_max_axis;


    //distances computation
    static bool get_distances(float *position, const float *destination, float *distances, uint8_t *maximum_axis,
                              float *maximum_distance);

    //slopes pre-computation
    static void get_slopes(float *slopes, const float *const distances, uint8_t max_axis, float max_distance);

    //position provider for increment computing
    static void get_position(float indice, float *positions);


    //--------------------------------------------Pre-processed_Data_Storage--------------------------------------------

    //The Queue containing the movement data.
    static Queue<k2_linear_data> linear_data_queue;


    //-----------------------------------------------Real_time_Processing-----------------------------------------------

private :

    //offsets, slopes, and max_axis used during real-time postion computation
    static float *real_time_offsets;
    static float *real_time_slopes;
    static uint8_t real_time_max_axis;

    //the function to initialise movement
    static void initialise_movement();

    //The function to finalise the movement
    static void finalise_movement();

    //real time position provider
    static void get_real_time_position(float index, float *positions);


};


#endif //TRACER_LINEAR_MOVEMENT_H

#endif