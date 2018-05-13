/*
  SubMovementManager.h - Part of TRACER

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

/*
 * SubMovementManager : This class handles the entire processing of a movement.
 *
 *  The Trajectory Tracer feeds it with movements, that are progressively processed.
 *
 *  It maintains a sub_movement queue full, so that the steppers can keep moving.
 *
 *  It also handles the proximity with and jerk points.
 */

#ifndef TRACER_SUBMOVEMENTMANAGER_H
#define TRACER_SUBMOVEMENTMANAGER_H

#include <DataStructures/Containers/Queue.h>

#include "_core_include.h"

class SubMovementManager {

    //------------------------------------------- initialisation --------------------------------------------

public:

    //Initialise all fields to their safe value;
    static void initialise_data();


    //--------------------------------------------current_steppers_positions---------------------------------------------

private:

    //The current positions of stepper motors
    static float *const current_steppers_positions;

    //Remark : the current stepper position is the translation if the high level positions, by StepperAbstraction.


    //-----------------------------------------------Sub_movement_queue-------------------------------------------------

private:

    //The sub_movements queue
    static Queue<sub_movement_data_t> sub_movement_queue;


public :

    //A simple method to return the number of available_sub_movements currently in the sub_movement queue.
    static uint8_t available_sub_movements() {
        return sub_movement_queue.available_objects();
    }

    static bool is_movement_processed();


    //-----------------------------------------Current_Movement_Variables-----------------------------------------------

public :

    static void display_distances();

    //The function to initialise_hardware all movement variables, during the movement change.
    static uint8_t update_current_movement(movement_data_t *sub_movement_data);


private :

    //The movement index and its limit
    static float index, index_limit;

    //The index increment
    static float increment;

    //The index direction flag : true if the increment is positive_rotation.
    static bool positive_index_dir;

    //The trajectory function for the current movement
    static void (*get_new_position)(float index, float *positions);


    //----------------------------------------Pre_Computed_Positions_storage--------------------------------------------

private :

    //movement_last_position_processed_flag flag, true when the current movement's last position have been processed
    static bool movement_last_position_processed_flag;


public :

    /*
     * position pushing functions :
     *  As the pushing procedure is costly, it is divided in two procedures. They must be called one after the other
     *      for a new position to be effectively pushed.
     */

    //function to determine and process_t a candidate position.
    static void push_new_sub_movement();

    //function to determine a candidate position.
    static void compute_new_sub_movement(sub_movement_data_t *sub_movement_data);

    //function to process_t (if the step_distances bounds are respected) the previously computed position in the queue.
    static bool confirm_sub_movement(sub_movement_data_t *sub_movement_data);

    //function to update the increment and to verify the distance bounds validity
    static bool distance_bounds_error(float max_distance);

    //function to process_t the maximum number of positions in the queue.
    static void fill_sub_movement_queue();

    //function to pop the next position in the queue.
    static sub_movement_data_t *read_next_sub_movement();

    static void discard_sub_movement();


    //---------------------------------------High_level_and_Low_level_distances-----------------------------------------

private :

    //function to get low level step_distances.
    static float get_steppers_distances(float *const pos, sub_movement_data_t *sub_movement_data);

    //A constant array containing every axis signature
    static const sig_t *const axis_signatures;


    //---------------------------------------------------End_Distances--------------------------------------------------

public:

    /*  End Distances Conventions :
    *      pos              end_distance>0        destination   -> +
    *      destination      end_distance<0        pos           -> +
    *
    *  if movement direction is
    *      positive_rotation, (-> +), end distance decreases
    *      negative, (- <-), end distance increases
    *
    *  if new destination is
    *      greater than the position, (-> +), end distance increases
    *      lesser  than the position, (- <-), end distance decreases
    */

    /*  Axis direction conventions :
     *      1    : negative direction (- <-)
     *      0   : positive_rotation direction (-> +)
     */

    //End position update
    static void update_end_position(const float *const new_hl_position);

    //Jerk position update
    static void update_jerk_position(const int32_t *const new_hl_position);

    //End step_distances update
    static void update_end_jerk_distances(const sig_t negative_signatures, const uint8_t *elementary_dists);

    //the stepper end step_distances;
    static int32_t *const end_distances;

    //the stepper jerk step_distances;
    static int32_t *const jerk_distances;


private:

    //The stepper end positions;
    static int32_t *const end_position;

    //The stepper jerk positions;
    static int32_t *const jerk_position;


};


#endif //TRACER_SUBMOVEMENTMANAGER_H
