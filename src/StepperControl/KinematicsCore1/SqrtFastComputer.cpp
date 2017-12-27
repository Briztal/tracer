/*
  SqrtFastComputer.h - Part of TRACER

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

#include "SqrtFastComputer.h"
#include <hardware_language_abstraction.h>
#include <Interaction/Interaction.h>


/*
 * sqrt_slow : this function updates the speed_distances, and its square root, with a call to sqrt.
 *
 * It also sets correct variables for the fast algorithm.
 */

uint16_t SqrtFastComputer::sqrt_slow(uint32_t distance_to_end) {
    primary = distance_to_end;
    square_root = (uint16_t) sqrt_long(distance_to_end);
    primary_sup = primary_increment = (uint16_t) (2 * square_root + 1);
    primary_inf = 1;

    return square_root;
}


/*
 * sqrt_fast : this function sets regulation_speed distance, in absolute mode.
 *
 * It calls _sqrt_fast in relative mode.
 */

uint16_t SqrtFastComputer::sqrt_fast(uint32_t distance_to_end) {

    int32_t d = distance_to_end - primary;

    bool positive_incr;
    if (!(positive_incr = d > 0)) d = -d;

    _sqrt_fast(positive_incr, (uint16_t) d);

    return square_root;

}


/*
 * _sqrt_fast : this function updates the primary and its square root,
 *      with a more efficient algorithm than calling sqrt
 *
 * If positive_incr is true, it increments primary with incr value. If not, it decrements it with incr value.
 *
 * This algorithm is based on the formula n*n = 1 + 3 + 5 + ... + (2.n-1)
 *
 * When the square root is modfied, it sets sub_movement_time_update_flag, so that step_period_us (regulation_speed) will be modified aferwards.
 *
 * The step_period_us is set according to  : step_period_us = delay_numerator/sqrt(primary)
 *
 */
uint16_t SqrtFastComputer::_sqrt_fast(bool positive_incr, uint16_t incr) {

#define switch_up primary_inf = 1, primary_increment+=2, primary_sup = primary_increment, square_root++;
#define switch_down if(square_root) { primary_increment-=2, primary_inf=primary_increment, primary_sup=1, square_root--;}

    if (positive_incr) {
        //An increment is required :

        //incrementing the distance
        primary += incr;

        //Updating bounds and square root
        while (primary_sup < incr) {
            incr -= primary_sup;
            switch_up
        }

        //last update, with the reduced increment
        primary_sup -= incr, primary_inf += incr;

    } else {
        //An decrement is required :

        if (primary > incr) {
            //If we need to decrement less than the regulation_speed distance

            //decrementing the distance
            primary -= incr;

            //Updating bounds and square root
            while (primary_inf < incr) {
                incr -= primary_inf;
                switch_down
            }

            //last update, with the reduced increment
            primary_sup += incr, primary_inf -= incr;
        } else {
            //If we need to decrement more than the regulation_speed distance : regulation_speed distance goes to zero (necessarily >= 0);

            primary = 0;
            primary_inf = primary_sup = primary_increment = 1;
            square_root = 0;

        }
    }

    return square_root;

#undef switch_down
#undef switch_up

}


void SqrtFastComputer::print_data() {
    std_out("sd : " + str(primary));
    std_out("de : " + str(square_root));
}
