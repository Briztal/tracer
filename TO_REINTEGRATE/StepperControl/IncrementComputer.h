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
 * The PreProcessing class : this class is in charge of the two tasks below :
 *      - Determining the correct increment for a particular point of a specific trajectory;
 *      - Determining the speed that matches the jerk limits for a couple of movements
 *
 */


#ifndef TRACER_INCREMENT_COMPUTER_H
#define TRACER_INCREMENT_COMPUTER_H

#include "config.h"

#ifdef ENABLE_STEPPER_CONTROL

#include "_kinematics_data.h"

class IncrementComputer {

    //-------------------------------------------- Compute Increments --------------------------------------------

public:

    //Compute beginning and ending increments for the given movement;
    static bool determine_increments(movement_data *movement);


    //-------------------------------------------- Two private functions --------------------------------------------

private :

    //Compute the increment for a point of a curve;
    static float extract_increment(void (*get_position)(float, float *), float point, float incr);

    //Get the maximal distance between the pair of given points;
    static uint32_t get_max_dist(float *p0, float *p1);

};


#endif //TRACER_INCREMENT_COMPUTER_H

#endif