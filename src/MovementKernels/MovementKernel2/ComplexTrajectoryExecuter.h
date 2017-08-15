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
#include "complex_motion_data.h"
#include "../../DataStructures/Queue.h"
#include "../StepperController.h"

#define WAIT\
    while(!stepper_int_flag) {}\
        stepper_int_flag_clear();

#define ME ComplexTrajectoryExecuter

#define STEP_AND_WAIT \
    {\
        sig_t s_w_signature;\
        while (!(s_w_signature = ME::saved_elementary_signatures[ME::trajectory_array[ME::saved_trajectory_indice--]])){}\
        StepperController::fastStep(s_w_signature);\
    }\
    WAIT

class ComplexTrajectoryExecuter {


    //--------------------------------------------movement_queue_management---------------------------------------------

private:
    static Queue<complex_motion_data> motion_data_queue;
    static complex_motion_data motion_data_to_fill;
    static complex_motion_data popped_data;



    //-----------------------------------------------sub_movement_queue-------------------------------------------------

public :

    static bool in_motion;

    static complex_motion_data *peak_last_motion_data();

    //---------------------------------------------movement_pre_processing----------------------------------------------

    /*
     * The three functions above are in charge of adding data to motion_data_to_fill.
     */
public:

    static void push_first_sub_movement(uint8_t *elementary_dists, sig_t negative_signatures);


    static void
    fill_processors(void (*init_f)(), sig_t (*position_f)(float, float *), void (*speed_f)());

    //The function to copy the current motion_data to the queue.
    static void enqueue_movement_data();

    //---------------------------------------------Real_Time_Movement_data----------------------------------------------

public :

    static sig_t *saved_elementary_signatures;

    static uint8_t saved_trajectory_indice, trajectory_indice, *const trajectory_indices;

    static uint32_t delay;

    static uint8_t *const trajectory_array;

    //--------------------------------------Sub_Movement_Pre_Computation------------------------------------------------

private :

    static sig_t *const es0, *const es1;
    static bool is_es_0;



    //------------------------------------------------Movement_Procedure------------------------------------------------


public :

    static void start_movement();

private:

    static void prepare_next_sub_movement();

    static void finish_sub_movement();

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
     *      true    : negative direction (- <-)
     *      false   : positive direction (-> +)
     */


    static void update_end_distances(const sig_t negative_signatures, const uint8_t *elementary_dists);

    //----------------------------------------------------Signatures----------------------------------------------------

    static void process_signatures(uint8_t *const elementary_dists, sig_t * elementary_signatures, uint8_t *trajectory_indice);

    static sig_t *initialise_sub_movement();
};


#endif //TRACER_MOTIONEXECUTER_H


#endif