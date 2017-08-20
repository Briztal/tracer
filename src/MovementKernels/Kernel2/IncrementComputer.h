/*
  IncrementComputer.h - Part of TRACER

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
 * The Increment Computing class : this class computes the initial increment for a trajectory.
 *
 *  This step is absolutely necessary, as a position can only be enqueued in the sub_movement queue if the distance
 *      between this new position and the previous one is comprised in a certain interval.
 *
 *  This distance is determined by the index increment, the delta of the trajectory function's input variable.
 *
 *  This increment needs to be adjusted when a movement is planned. During the real-time position computation, it is
 *      again adjusted continuously, so that the distances between two consecutive positions continue to fit in the
 *      interval.
 *
 */


#include <stdint.h>
#include "../../config.h"
#include "../../DataStructures/Queue.h"
#include "_kernel_2_data.h"

#ifdef ENABLE_STEPPER_CONTROL

#ifndef TRACER_INCREMENT_COMPUTER_H
#define TRACER_INCREMENT_COMPUTER_H


class IncrementComputer {

public:

    static float extract_increment(void (*get_position)(float, float *), float point, float increment,
                                   const uint32_t distance_target);

private :

    static uint32_t get_max_dist(float *p0, float *p1);

    void static get_stepper_position(void (*get_position)(float, float *), float point, float *positions);

};


#endif //TRACER_INCREMENT_COMPUTER_H

#endif