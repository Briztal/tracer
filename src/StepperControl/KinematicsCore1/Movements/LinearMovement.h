/*
  LinearMovement.h - Part of TRACER

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

#if defined(ENABLE_STEPPER_CONTROL) && (CORE_VERSION == 0)

#ifndef CODE_MOTIONND_H
#define CODE_MOTIONND_H

#include <stdint.h>
#include <DataStructures/Queue.h>


class LinearMovement {

public :

    static void prepare_motion(const float *destinations_t);

private :

    //Fields required during the movement :
    static uint32_t *const MR_positions;
    static float *const MR_slopes;
    static uint8_t MR_max_axis;
    static sig_t MR_negative_signatures;

    //Other fields
    static Queue<linear_data> data_queue;
    static linear_data data_to_fill;


    //Preparators

    static void micro_move(uint32_t *dists);

    static void set_motion_data(uint32_t *final_dists);

    static void step_and_delay(uint8_t sig);

    static void process_speed();

    static void enqueue_movement(uint32_t *dists);

    static sig_t process_position(uint8_t *elementary_dists);

    static uint8_t setup_movement_data(const float *destinations_t, uint32_t *absolute_distances);

    static void set_position_data(uint32_t *dists);

    static void initialise_motion();

};

#endif //CODE_MOTION3D_H

#endif