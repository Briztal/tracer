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
  along with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/


#include "../config.h"

#ifdef ENABLE_STEPPER_CONTROL

#ifndef TRACER_MOTIONEXECUTER_H
#define TRACER_MOTIONEXECUTER_H

#include "../hardware_language_abstraction.h"
#include "motion_data.h"
#include "../DataStructures/Queue.h"

#define WAIT\
    while(!TIFR5) {}\
        TIFR5 = 255;

#define ME MotionExecuter

#define STEP_AND_WAIT \
    {\
        unsigned char s_w_signature;\
        if (!(s_w_signature = ME::saved_elementary_signatures[ME::trajectory_array[ME::saved_trajectory_indice--]]))\
        s_w_signature = ME::saved_elementary_signatures[ME::trajectory_array[ME::saved_trajectory_indice--]];\
        (*ME::step)(s_w_signature);\
    }\
    WAIT

class MotionExecuter {


    //--------------------------------------------movement_queue_management---------------------------------------------

private:
    static Queue<motion_data> motion_data_queue;
    static motion_data motion_data_to_fill;
    static motion_data popped_data;


    //---------------------------------------------movement_pre_processing----------------------------------------------

    /*
     * The three functions above are in charge of adding data to motion_data_to_fill.
     */
public:

    static void fill_movement_data(bool first, unsigned char *elementary_dists, unsigned int count, unsigned char negative_signatures);

    static void fill_speed_data(unsigned int delay_numerator, unsigned int regulation_delay, float ratio, unsigned char processing_steps);

    static void fill_processors(void (*init_f)(), void (*step_f)(unsigned char), unsigned char (*position_f)(unsigned char *), void (*speed_f)());

    //The function to copy the current motion_data to the queue.
    static void enqueue_movement_data();

    //---------------------------------------------Real_Time_Movement_data----------------------------------------------



public :

    static volatile bool distances_lock;
    static long *const end_distances;
    static unsigned char *saved_elementary_signatures;
    static unsigned char saved_trajectory_indice;
    static void (*step)(unsigned char);
    static unsigned char *const trajectory_array;

private :

    static unsigned int count;
    static bool ultimate_movement, penultimate_movement;
    static unsigned char *const es0, *const es1;
    static bool is_es_0;
    static unsigned char trajectory_indice;
    static const unsigned char *const trajectory_indices;


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

    static bool *const axis_directions_negative;

    /*  Axis direction conventions :
     *      true    : negative direction (- <-)
     *      false   : positive direction (-> +)
     */

    //-------------------------------------------Real_Time_Movement_Processors-------------------------------------------

private:
    static unsigned char (*position_processor)(unsigned char *);

    static void (*speed_processor)();


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
    *  - finish_sub_movement makes all remaining tics and then sets prepare_next_sub_motion as interrupt routine,
    *      or wait_for_movement is the current movement is done.
    */



private:

    static void prepare_next_sub_motion();

    static void wait_for_movement();

    static void finish_sub_movement();

    static void set_last_sub_motion();

};


#endif //TRACER_MOTIONEXECUTER_H


#endif