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
 *  - the index variables : beginning, ending, increment;
 *  - the initialisation and finailisation function, called (resp) at the beginning and ending of the movement;
 *  - the trajectory function, used continuously during the movement to get positions;
 *  - The regulation_speed variables : the regulation_speed and the regulation_speed group,
 *  - The tools signature, indicating tools enabled during this movement.
 *
 */

typedef struct kernel_movement_data {

    /*
     * Trajectory data :
     *
     *  The following lines contain data required to trace a trajectory.
     */

    //The trajectory function : it takes a real value (the index) and outputs a position;
    void (*trajectory_function)(float, float *);

    //The pre processing trajectory function : this function is exactly like the previous one, but is not use during
    //  the movement. It takes a real value (the index) and outputs a position;
    void (*pre_process_trajectory_function)(float, float *);

    //The beginning value of the trajectory index;
    float beginning;

    //The ending value of the trajectory index;
    float ending;

    //The increment at the beginning point;
    float beginning_increment;

    //The increment at the ending point;
    float ending_increment;


    /*
     * The speed group on which the speed regulation is based on.
     */

    uint8_t speed_group;


    /*
     * Movement cleanup function :
     *
     *  The following lines contain functions to initialise_data and finalise correctly the movement;
     */

    //Initialisation, called before the movement processing starts;
    void (*movement_initialisation)();

    //Finalisation, called when the movement is entirely processed;
    void (*movement_finalisation)();


    /*
     * Jerk :
     *
     *  The following lines contain all data required to perform the jerk regulation
     */

    //A boolean, determining whether the ending point is a jerk point
    bool jerk_point = false;

    //The position of the jerk point
    int32_t jerk_position[NB_STEPPERS]{0};

    //The jerk ratios, at the end of the movement.
    float ending_jerk_ratios[NB_STEPPERS];


    /*
     * Tools data : the following signature determines which of tools will move during the movement.
     */

    sig_t tools_signatures;

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
    sig_t direction_signature;
    float index_candidate;

} sub_movement_data;




//---------------------------------------------------KinematicsCore1 Structures-------------------------------------------------
/*
 * Standard structures implementation for the kernel1
 */


/*
 * TODO
 */

typedef struct k1_movement_data : movement_data {//TODO REORG
    uint8_t reference_axis;
    float acceleration_delay_numerator;
    float dn_a_to_dn_d;
    float dn_d_to_dn_a;
    float acceleration_step;
    float first_sub_movement_hl_distance_current_speed_group;
    float first_sub_movement_hl_distance_last_speed_group;
    delay_t regulation_sub_movement_time;

    float ratio;
    uint32_t jerk_distance_offset;
    bool jerk_point;
    float speed_factor;
} k1_movement_data;//36 bytes


/*
 * k1_sub_movement_data : no real time information added.
 */

typedef sub_movement_data k1_sub_movement_data;



//---------------------------------------------------KinematicsCore2 Structures-------------------------------------------------
/*
 * Standard structures implementation for the KinematicsCore2
 *
 */


/*
 * k2_movement_data : this structure contains all data related to one movement :
 *  - the index variables : beginning, ending, increment;
 *  - the initialisation and finailisation function, called (resp) at the beginning and ending of the movement;
 *  - the trajectory function, used continuously during the movement to get positions;
 *  - The regulation_speed variables : the regulation_speed and the regulation_speed group,
 *  - The tools signature, indicating tools enabled during this movement.
 *
 */

typedef struct k2_movement_data : movement_data {
    float speed; //28
    uint32_t jerk_offsets[NB_STEPPERS]{0};
    //-----------end : 32 to 36 bytes----------
} k2_movement_data;



/*
 * k2_movement_data : this structure contains all data related to one movement :
 *  - the index variables : beginning, ending, increment;
 *  - the initialisation and finailisation function, called (resp) at the beginning and ending of the movement;
 *  - the trajectory function, used continuously during the movement to get positions;
 *  - The regulation_speed variables : the regulation_speed and the regulation_speed group,
 *  - The tools signature, indicating tools enabled during this movement.
 *
 */

typedef struct k2_sub_movement_data : sub_movement_data {

    float movement_distance;
    float regulation_speed;
    //-----------end : 32 to 36 bytes----------
} k2_sub_movement_data;



//-------------------------------------------------Movements Structures-------------------------------------------------

/*
 * k2_linear_data : this structure contains all data related to a linear movement :
 *  - offsets : the stepper positions at the starting point of the line;
 *  - slopes : the slopes of the line, relatively to the greater axis;
 *  - max_axis : the greater axis, in term of movement distance.
 *
 */
//TODO COMMENT THIS FILE PROPERLY
typedef struct {
    float offsets[NB_AXIS]; //4*NB_AXIS
    float slopes[NB_STEPPERS]; //4 * (NB_AXIS + NB_STEPPERS)
    uint8_t max_axis; //4 * (NB_AXIS + NB_STEPPERS) + 1
    //--end : 4 * (NB_AXIS + NB_STEPPERS + 1) bytes--
} k2_linear_data;


#endif //TRACER_COMPLEX_MOTION_DATA_H

#endif