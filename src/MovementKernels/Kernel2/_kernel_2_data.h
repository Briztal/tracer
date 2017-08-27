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
#include <wiring.h>

/*
 * The structures used to save and update information through the movement procedure of the kernel2.
 *
 *  Reminder : data sizes (in bytes)
 *      - uint8_t :     1
 *      - uint16_t :    2
 *      - uint32_t :    4
 *      - float :       4
 *      - pointer :     4
 */


//---------------------------------------------------Kernel Structures--------------------------------------------------

/*
 * k2_movement_data : this structure contains all data related to one movement :
 *  - the index variables : min, max, increment;
 *  - the initialisation and finailisation function, called (resp) at the beginning and ending of the movement;
 *  - the trajectory function, used continuously during the movement to get positions;
 *  - The speed variables : the speed and the speed group,
 *  - The tools signature, indicating tools enabled during this movement.
 *
 */

typedef struct {
    float min; //4
    float max;//8
    float increment; //12
    void (*movement_initialisation)(); //16
    void (*movement_finalisation)(); //20
    void (*trajectory_function)(float, float *); //24
    void (*pre_process_trajectory_function)(float, float *); //24
    float speed; //28
    uint8_t speed_group; //29
    sig_t tools_signatures; //30 -> 33
    bool jerk_point = false;
    uint32_t jerk_offsets[NB_STEPPERS]{0};
    int32_t jerk_position[NB_STEPPERS]{0};
    //-----------end : 32 to 36 bytes----------
} k2_movement_data;


/*
 * k2_real_time_data : this structure contains all data related to sub_movements :
 *  - the distance of the sub_movement
 *  - the speed of the sub_movement
 *  - if the starting point is a jerk point
 *  - the signature of the movement distances (1 for negative distance, 0 for positive)
 *
 * float and integer distances are also stored, in a separate queue.
 *
 */

typedef struct {
    float distance; //4
    float speed; //8
    sig_t negative_signature; //10 -> 13
    //--------end : 12 to 16 bytes-------
} k2_real_time_data;


//-------------------------------------------------Movements Structures-------------------------------------------------

/*
 * k2_linear_data : this structure contains all data related to a linear movement :
 *  - offsets : the stepper positions at the starting point of the line;
 *  - slopes : the slopes of the line, relatively to the greater axis;
 *  - max_axis : the greater axis, in term of movement distance.
 *
 */

typedef struct {
    float offsets[NB_AXIS]; //4*NB_AXIS
    float slopes[NB_STEPPERS]; //4 * (NB_AXIS + NB_STEPPERS)
    uint8_t max_axis; //4 * (NB_AXIS + NB_STEPPERS) + 1
    //--end : 4 * (NB_AXIS + NB_STEPPERS + 1) bytes--
} k2_linear_data;


typedef struct {
    uint8_t *elementary_dists;
    float *real_dists;
    float index_candidate;
    float movement_distance;
    float future_steppers_positions[NB_STEPPERS];
    float candidate_high_level_positions[NB_AXIS];

} position_data_struct;

#endif //TRACER_COMPLEX_MOTION_DATA_H

#endif