/*
  LinearMotionN.h - Part of TRACER

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

#include "../../config.h"

#ifdef ENABLE_STEPPER_CONTROL

#ifndef CODE_MOTIONND_H
#define CODE_MOTIONND_H


#include "../motion_data.h"
#include "../../DataStructures/Queue.h"

class LinearMotionN {

public :

    static void prepare_motion(float *destinations_t);

private :

    //Fields required during the movement :
    static unsigned int *const MR_positions;
    static unsigned int *const MR_slopes;
    static int MR_shift_nb;
    static unsigned char MR_max_axis;
    static unsigned char MR_negative_signatures;

    //Other fields
    static Queue<linear_data> data_queue;
    static linear_data data_to_fill;


    //Preparators

    static void micro_move(unsigned long *dists);

    static void set_motion_data(unsigned int *final_dists);

    static void step_and_delay(unsigned char sig);

    static void process_speed();

    static void simple_move(unsigned long *dists);

    static unsigned char process_position(unsigned char *elementary_dists);

    static unsigned char setup_movement_data(float *destinations_t, unsigned long *absolute_distances);

    static void set_position_data(unsigned int *dists);

    static void initialise_motion();

};

#endif //CODE_MOTION3D_H

#endif