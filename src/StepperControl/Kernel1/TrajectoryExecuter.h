/*
  MotionExecuter.h - Part of TRACER

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

#ifndef ENABLE_STEPPER_CONTROL

#ifndef TRACER_TRAJECTORYEXECUTER_H
#define TRACER_TRAJECTORYEXECUTER_H

#include <hardware_language_abstraction.h>
//#include "motion_data.h"
#include <DataStructures/Queue.h>

#define WAIT\
    while(!stepper_int_flag) {}\
        stepper_int_flag_clear();

#define ME TrajectoryExecuter

#define STEP_AND_WAIT \
    {\
        sig_t s_w_signature;\
        if (!(s_w_signature = ME::saved_elementary_signatures[ME::trajectory_array[ME::saved_trajectory_indice--]]))\
        s_w_signature = ME::saved_elementary_signatures[ME::trajectory_array[ME::saved_trajectory_indice--]];\
        (*ME::step)(s_w_signature);\
    }\
    WAIT

class TrajectoryExecuter {


    //--------------------------------------------movement_queue_management---------------------------------------------

private:
    static Queue<motion_data> motion_data_queue;
    static motion_data motion_data_to_fill;
    static motion_data popped_data;

public :

    static bool in_motion;
    static motion_data * peak_last_motion_data();


    //---------------------------------------------movement_pre_processing----------------------------------------------

    /*
     * The three functions above are in charge of adding data to motion_data_to_fill.
     */
public:

    static void fill_movement_data(bool first, uint8_t *elementary_dists, uint32_t count, sig_t negative_signatures);

    static void fill_speed_data(delay_t delay_numerator, delay_t regulation_delay, float speed_factor, float ratio, uint8_t processing_steps);

    static void fill_processors(void (*init_f)(), void (*step_f)(sig_t), sig_t (*position_f)(uint8_t *), void (*speed_f)());

    //The function to copy the current motion_data to the queue.
    static void enqueue_movement_data();

    //---------------------------------------------Real_Time_Movement_data----------------------------------------------

public :

    static sig_t *saved_elementary_signatures;
    static uint8_t saved_trajectory_indice;
    static void (*step)(sig_t);
    static uint8_t *const trajectory_array;

private :

    static uint32_t count;
    static bool ultimate_movement, penultimate_movement;
    static sig_t *const es0, *const es1;
    static bool is_es_0;
    static uint8_t trajectory_indice;
    static const uint8_t *const trajectory_indices;


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

    static sig_t (*position_processor)(uint8_t *);

    static void (*speed_processor)();

    //------------------------------------------------Movement_Procedure------------------------------------------------

    /*
     * The methods to start_movement and stop the Stepper Motor movements
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

    static void prepare_next_sub_motion();

    static void process_next_move();

    static void finish_sub_movement();

    static void set_last_sub_motion();

};


#endif //TRACER_MOTIONEXECUTER_H


#endif