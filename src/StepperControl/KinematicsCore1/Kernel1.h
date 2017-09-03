/*
  Kernel1.h - Part of TRACER

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

#if defined(ENABLE_STEPPER_CONTROL) && (KERNEL == 1)


#ifndef TRACER_KERNEL1_H
#define TRACER_KERNEL1_H

#include <stdint.h>
#include <StepperControl/_kernels_data.h>

class KinematicsCore1 {

public :

    static void initialise_tracing_procedure();

    //-----------------------------------------------Kernel status flags------------------------------------------------

public:

    //The function to call to verify that sub_movements are available in the queue.
    static uint8_t available_sub_movements();

    //The function to call to know if the current movement has been processed.
    static bool movement_processed_flag();


    //---------------------------------------------Current movement update----------------------------------------------

public:

    static uint8_t update_current_movement(k1_movement_data *movement_data);

    static void initialise_movement_data(k1_movement_data *movement_data);

    static void compute_jerk_data(const k1_movement_data *current_movement, k1_movement_data *previous_movement);

    //------------------------------------------------environment update------------------------------------------------

public :

    static void update_movement_environment(k1_movement_data *movement_data);

    static void update_jerk_environment(k1_movement_data *movement_data);


    //--------------------------------------------sub_movements preparation---------------------------------------------

public:

    //The first sub_movement preparation, called at the beginning of the movement routine.
    static void prepare_first_sub_movement(uint8_t *elementary_distances, sig_t *negative_signature, float *time);

    //The sub_movement preparation function, called on interrupts.
    static void prepare_next_sub_movement(uint8_t * elementary_distances, sig_t *negative_signatures, float *delay);


    //----------------------------------------------------positon log---------------------------------------------------

public:

    static void send_position();


};


#endif //TRACER_KERNEL1_H

#endif