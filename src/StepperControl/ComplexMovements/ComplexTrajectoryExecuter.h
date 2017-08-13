/*
  ComplexTrajectoryExecuter.h - Part of TRACER

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


#include "../../config.h"

#ifdef ENABLE_STEPPER_CONTROL

#ifndef TRACER_TRAJECTORYEXECUTER_H
#define TRACER_TRAJECTORYEXECUTER_H

#include "../../hardware_language_abstraction.h"
#include "../motion_data.h"
#include "../../DataStructures/Queue.h"

#define WAIT\
    while(!stepper_int_flag) {}\
        stepper_int_flag_clear();

#define ME ComplexTrajectoryExecuter

#define STEP_AND_WAIT \
    {\
        sig_t s_w_signature;\
        if (!(s_w_signature = ME::saved_elementary_signatures[ME::trajectory_array[ME::saved_trajectory_indice--]]))\
        s_w_signature = ME::saved_elementary_signatures[ME::trajectory_array[ME::saved_trajectory_indice--]];\
        ME::step(s_w_signature);\
    }\
    WAIT

class ComplexTrajectoryExecuter {

    //--------------------------------------------current_stepper_positions---------------------------------------------

private:
    static int32_t *const current_stepper_positions;

    //--------------------------------------------movement_queue_management---------------------------------------------

private:
    static Queue<motion_data> motion_data_queue;
    static motion_data motion_data_to_fill;
    static motion_data popped_data;


    //-----------------------------------------------sub_movement_queue-------------------------------------------------

private:
    static Queue<sig_t> sub_movement_queue;
    static uint8_t * sub_movement_distances;

    //-----------------------------------------------sub_movement_queue-------------------------------------------------

public :

    static bool in_motion;

    static motion_data *peak_last_motion_data();

    //---------------------------------------------movement_pre_processing----------------------------------------------

    /*
     * The three functions above are in charge of adding data to motion_data_to_fill.
     */
public:

    static void fill_movement_data(bool first, uint8_t *elementary_dists, uint32_t count, sig_t negative_signatures);

    static void fill_speed_data(delay_t delay_numerator, delay_t regulation_delay, float speed_factor, float ratio,
                                uint8_t processing_steps);

    static void
    fill_processors(void (*init_f)(), void (*step_f)(sig_t), sig_t (*position_f)(uint8_t *), void (*speed_f)());

    //The function to copy the current motion_data to the queue.
    static void enqueue_movement_data();

    //---------------------------------------------Real_Time_Movement_data----------------------------------------------

public :

    static sig_t *saved_elementary_signatures;

    static uint8_t saved_trajectory_indice, trajectory_indice, *const trajectory_indices;

    static uint8_t *const trajectory_array;

    static void step(sig_t);

    //---------------------------------Intermediary_Positions_Pre_Computation-------------------------------------------

private :

    //The movement index and its bounds
    static float index_min, index_max, index;

    static float increment;

#define MAX_DISTANCE_TARGET 15

#define MINIMUM_DISTANCE_LIMIT 10

#define MAXIMUM_DISTANCE_LIMIT 256

    //--------------------------------------Sub_Movement_Pre_Computation------------------------------------------------

private :

    static sig_t *const es0, *const es1;
    static bool is_es_0;

    /*
    static bool ultimate_movement, penultimate_movement;
     */


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
     *      true    : negative direction (- <-)
     *      false   : positive direction (-> +)
     */

    //-------------------------------------------Real_Time_Movement_Processors-------------------------------------------

private:

    static sig_t (*get_new_position)(float index, float *positions);

    static void (*update_speed)();


    //------------------------------------------------Movement_Procedure------------------------------------------------

    /*
     * The methods to start and stop the Stepper Motor movements
     */

public :

    static void start();

    static void stop();




    /*
    * The three interrupt functions :
    *  - wait_for_movement waits till it can setup a movement, then sets trace as interrupt routine;
    *  - trace plans the current sub_movement, while periodically stepping and waiting,
    *      and then set finish_sub_movement as interrupt routine;
    *  - finish_sub_movement makes all remaining tics and then sets prepare_next_sub_movement as interrupt routine,
    *      or process_next_move is the current movement is done.
    */

private:

    static void prepare_next_sub_movement();

    static void process_next_move();

    static void finish_sub_movement();

    static void set_last_sub_motion();




    static void update_end_distances(const uint8_t *elementary_dists);

    static void process_signatures(uint8_t *const elementary_dists, sig_t *const elementary_signatures);

    static void update_speed_and_actions();

    static sig_t *get_signatures_array();

    static void push_new_position();

    static uint32_t pop_next_position(uint8_t *elementary_dists);

    static bool get_distances(const int32_t *const pos, const int32_t *const dest, const uint8_t *const dists,
                              sig_t *dir_dignature_p, uint8_t *max_axis_p, uint8_t *max_distance_p);
};


#endif //TRACER_MOTIONEXECUTER_H


#endif