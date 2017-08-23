/*
  RealTimeProcessor.h - Part of TRACER

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

#ifndef TRACER_REAL_TIME_PROCESS_H
#define TRACER_REAL_TIME_PROCESS_H

#include <DataStructures/Queue.h>
#include "_kernel_2_data.h"

class RealTimeProcessor {

public:

    static void start();

    //--------------------------------------------current_stepper_positions---------------------------------------------


private:

    //The current positions of stepper motors
    static float *const current_stepper_positions;

    //The future positions of stepper motors
    static float *const future_stepper_positions;

    //The current position in the high level system.
    static float *const current_hl_position;

    //Remark : the current stepper position is the translation if the high level positions, by StepperAbstraction.

public :

    //the function to update the high level position
    static void update_current_hl_position(float *new_hl_position);

    //The function to send the current position through the interface.
    static void send_position();


    //-----------------------------------------------Sub_movement_queue-------------------------------------------------

private:

    //The sub_movements queue
    static Queue<k2_real_time_data> sub_movement_queue;

    //The arrays to store real and integer distances
    static float *sub_movement_real_distances;
    static uint8_t *sub_movement_int_distances;

public :

    //A simple method to return the number of elements currently in the sub_movement queue.
    static uint8_t elements() {
        return sub_movement_queue.available_elements();
    }


    //-----------------------------------------Current_Movement_Variables-----------------------------------------------

public :

    //The function to initialise all movement variables, during the movement change.
    static void initialise_movement(float min, float max, float increment,
                                    void (*trajectory_function)(float index, float *positions));

private :


    //The movement index and its limit
    static float index, index_limit;

    //The index increment
    static float increment;

    //The index direction flag : true if the increment is positive.
    static bool positive_index_dir;

    //The speed group for the current movement
    static uint8_t movement_speed_group;

    //The trajectory function for the current movement
    static void (*get_new_position)(float index, float *positions);


    //---------------------------------------High_level_and_Low_level_distances-----------------------------------------

private :

    //The distance bounds

#define MINIMUM_DISTANCE_LIMIT 10

#define MAXIMUM_DISTANCE_LIMIT 256

    //The speed groups indices {i_k} where i_(3j+k) (0<=k<3) if the k_th indice of the speed group j. Compiler constant.
    static const int8_t *const speed_groups_indices;

    //function to compute the high level movement distance for a particular speed group.
    static float get_movement_distance_for_group(uint8_t speed_group, const float *const distances);

    //function to get low level distances.
    static bool
    get_steppers_distances(float *const pos, const float *const dest, uint8_t *const int_dists, float *const real_dists,
                           sig_t *dir_dignature_p, uint8_t *max_axis_p, float *max_distance_p);


    //----------------------------------------Pre_Computed_Positions_storage--------------------------------------------

public :

    //empty_queue flag : true when the sub_movement queue is empty.
    static bool empty_queue;

    //movement_processed flag, true when the current movement's last position have been processed
    static bool movement_processed;

    /*
     * position pushing functions :
     *  As the pushing procedure is costly, it is divided in two procedures. They must be called one after the other
     *      for a new position to be effectively pushed.
     */

    //function to determine and push a candidate position.
    static void push_new_position();

    //function to determine a candidate position.
    static void push_new_position_1();

    //function to push (if the distances bounds are respected) the previously computed position in the queue.
    static void push_new_position_2();

    //function to push the maximum number of positions in the queue.
    static void fill_sub_movement_queue();

    //function to pop the next position in the queue.
    static void
    pop_next_position(uint8_t *elementary_dists, float *real_dists, sig_t *negative_signature, float *distance);


    //---------------------------------------------------End_Distances--------------------------------------------------


    /*  End Distances Conventions :
    *      pos              end_distance>0        destination   -> +
    *      destination      end_distance<0        pos           -> +
    *
    *  if movement direction is
    *      positive, (-> +), end distance decreases
    *      negative, (- <-), end distance increases
    *
    *  if new destination is
    *      greater than the position, (-> +), end distance increases
    *      lesser  than the position, (- <-), end distance decreases
    */

    /*  Axis direction conventions :
     *      1    : negative direction (- <-)
     *      0   : positive direction (-> +)
     */

    //End distances update
    static void update_end_distances(const sig_t negative_signatures, const uint8_t *elementary_dists);

    //End position update
    static void update_end_position(const float *const new_hl_position);


private :

    //The stepper end positions;
    static int32_t *const end_position;

    //the stepper end distances;
    static int32_t *const end_distances;


    //------------------------------------------------Speed_Management--------------------------------------------------

public :

    //The function to set the next regulation speed
    static void set_regulation_speed(uint8_t speed_group, float speed);

    //The function to set the next regulation speed, and to enable the jerk flag for the next transition
    static void set_regulation_speed_jerk(uint8_t speed_group, float new_speed);

    //the function to compute the time for the current movement.
    static float pre_process_speed(float movement_distance, const float *const stepper_distances);

    //the function to update the current speed of the steppers, knowing the current movement time
    static void update_speeds(const float *const stepper_distances, float time);


private :

    //next_jerk_flag : true when the next sub_movement to pop will be after a jerk point
    static bool next_jerk_flag;

    //jerk_flag : true when the current sub_movement is after a jerk point
    static bool jerk_flag;

    //the regulation speed for the current movement
    static float next_regulation_speed;

    //Deceleration Fields,  computed during the heuristic calls
    static bool deceleration_required;

    //Current target speed
    static float regulation_speed;

    //Previous sub_movement duration
    static float last_time;

    //Current speed of steppers
    static float *const steppers_speeds;

    //Current steppers deceleration distances
    static uint32_t *const deceleration_distances;

    //A constant array containing every axis signature
    static const sig_t *const axis_signatures;



    //-------------------------------------------------Speed_Constants--------------------------------------------------


private :

    //The function to actualise the movement constants
    static void pre_compute_speed_constants();

    /*
     * The array containing the deceleration constants : in the deceleration distance formula :
     *      (((v * v) / (2 * EEPROMStorage::accelerations[stepper] * EEPROMStorage::steps[stepper]));
     *
     *  the denominator is constant. This array wil contain the float value
     *      1.0 / (2.0 * EEPROMStorage::accelerations[stepper] * EEPROMStorage::steps[stepper]));
     *
     *      for each stepper.
     */

    static float *const deceleration_constants;


    /*
     * The array containing the delta speed constants : in the deceleration distance formula :
     *      max_delta_speed = EEPROMStorage::accelerations[stepper] * EEPROMStorage::steps[stepper] * time;
     *
     *  the product of the two first terms is constant. This array wil contain the float value
     *      EEPROMStorage::accelerations[stepper] * EEPROMStorage::steps[stepper]
     *
     *      for each stepper.
     */

    static float *const delta_speed_constants;

    /*
     * The array containing the maximum speed constants : in the deceleration distance formula :
     *      max_speed = EEPROMStorage::speeds[stepper] * EEPROMStorage::steps[stepper] * time;
     *
     *  the product is constant. This array wil contain the float value
     *      EEPROMStorage::speeds[stepper] * EEPROMStorage::steps[stepper]
     *
     *      for each stepper.
     */

    static float *const max_speed_constants;


};


#endif //TRACER_REAL_TIME_PROCESS_H

#endif