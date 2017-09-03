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
#include <stdint.h>

#ifdef ENABLE_STEPPER_CONTROL


#ifndef TRACER_COMPLEX_MOTION_DATA_H
#define TRACER_COMPLEX_MOTION_DATA_H


//The distance bounds

#define MINIMUM_DISTANCE_LIMIT 8

#define DISTANCE_TARGET 11

#define MAXIMUM_DISTANCE_LIMIT 14


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


//-----------------------------------------------Stepper Standatd Structures---------------------------------------------
/*
 * The two structures below are the minimum of data required to make the TrajectoryTracer do its job.
 *
 *  Each Kernel will have its own structure, inherited of the stadard ones, adding the required data they need.
 *
 */


/*
 * movement_data : this structure contains all data related to one movement :
 *  - the index variables : min, max, increment;
 *  - the initialisation and finailisation function, called (resp) at the beginning and ending of the movement;
 *  - the trajectory function, used continuously during the movement to get positions;
 *  - The regulation_speed variables : the regulation_speed and the regulation_speed group,
 *  - The tools signature, indicating tools enabled during this movement.
 *
 */

typedef struct kernel_movement_data {
    float min; //4
    float max;//8
    float min_increment; //12
    float max_increment; //12
    void (*movement_initialisation)(); //16
    void (*movement_finalisation)(); //20
    void (*trajectory_function)(float, float *); //24
    void (*pre_process_trajectory_function)(float, float *); //24
    sig_t tools_signatures; //30 -> 33
    //-----------end : 32 to 36 bytes----------
} movement_data;


/*
 * sub_movement_data : this structure contains all data related to one movement :
 *  - step_distances : the step_distances of the sub_movement;
 *  - future_steppers_positions
 *  - The tools signature, indicating tools enabled during this movement.
 *
 */

typedef struct {
    uint8_t step_distances[NB_STEPPERS];
    float f_step_distances[NB_STEPPERS];
    float future_steppers_positions[NB_STEPPERS];
    float candidate_high_level_positions[NB_AXIS];

} sub_movement_data;




//---------------------------------------------------KinematicsCore1 Structures-------------------------------------------------
/*
 * Standard structures implementation for the kernel1
 */


/*
 * TODO
 */

typedef struct k1_movement_data : movement_data {//TODO REORG

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

} k1_movement_data;//36 bytes




//---------------------------------------------------KinematicsCore2 Structures-------------------------------------------------
/*
 * Standard structures implementation for the KinematicsCore2
 *
 */


/*
 * k2_movement_data : this structure contains all data related to one movement :
 *  - the index variables : min, max, increment;
 *  - the initialisation and finailisation function, called (resp) at the beginning and ending of the movement;
 *  - the trajectory function, used continuously during the movement to get positions;
 *  - The regulation_speed variables : the regulation_speed and the regulation_speed group,
 *  - The tools signature, indicating tools enabled during this movement.
 *
 */

typedef struct k2_movement_data : movement_data {

    float speed; //28
    uint8_t speed_group; //29
    bool jerk_point = false;
    uint32_t jerk_offsets[NB_STEPPERS]{0};
    int32_t jerk_position[NB_STEPPERS]{0};
    //-----------end : 32 to 36 bytes----------
} k2_movement_data;



/*
 * k2_movement_data : this structure contains all data related to one movement :
 *  - the index variables : min, max, increment;
 *  - the initialisation and finailisation function, called (resp) at the beginning and ending of the movement;
 *  - the trajectory function, used continuously during the movement to get positions;
 *  - The regulation_speed variables : the regulation_speed and the regulation_speed group,
 *  - The tools signature, indicating tools enabled during this movement.
 *
 */

typedef struct k2_sub_movement_data : sub_movement_data {

    sig_t direction_signature;
    float index_candidate;
    float movement_distance;
    float regulation_speed;
    //-----------end : 32 to 36 bytes----------
} k2_sub_movement_data;



/*
 * k2_real_time_data : this structure contains all data related to sub_movements :
 *  - the distance of the sub_movement
 *  - the regulation_speed of the sub_movement
 *  - if the starting point is a jerk point
 *  - the signature of the movement step_distances (1 for negative distance, 0 for positive)
 *
 * float and integer step_distances are also stored, in a separate queue.
 *
 */

typedef struct {
    float distance; //4
    float speed; //8
    sig_t negative_signature; //10 -> 13
    //--------end : 12 to 16 bytes-------
} k1_real_time_data;


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


#endif //TRACER_COMPLEX_MOTION_DATA_H

#endif