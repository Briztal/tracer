/*
  SpeedManager.cpp - Part of TRACER

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

#include <math.h>
#include <WString.h>
#include "SpeedManager.h"
#include "MotionExecuter.h"
#include "../Core/EEPROMStorage.h"
#include "../Interfaces/CommandInterface.h"

/*
 * Heuristic distance : this function calculates the heuristic distance between the actual position and the end position
 *
 * The current heuristic is the sum of all distances.
 */
void SpeedManager::heuristic_distance() {

    long td;
    unsigned long distance = 0;
    unsigned long dist = 0;

#define STEPPER(i, ...) \
    td = MotionExecuter::end_distances[i];\
    distance=((unsigned long) (td<0) ? -td : td );\
    if (distance>dist) dist = distance;

#include "../config.h"

#undef STEPPER

    distance_to_end = dist;
}


/*
 * regulate_speed : this function uses the distance to the end provided by the heuristic, to modify the speed_distance.
 *
 * It the distance is less than the minimal deceleration distance to respect the acceleration limit,
 *      it decrements the speed distance, and so enables the speed modification.
 *
 * If not, if the regulation speed has not been reached, it increments (or decrements) the speed distance,
 *      to go forward to it.
 */

void SpeedManager::regulate_speed() {
    if (distance_to_end < speed_distance) {
        //If we entered in the deceleration range :
        go_to_speed_distance(distance_to_end);
        regulation_unreached = true;
        regulation_stop_enabled = false;
        return;
    } else if (regulation_unreached) {
        //If speed still has to be modified :

        if (delay0 > regulation_delay) {
            //if speed is too low :

            if (!speed_incr && regulation_stop_enabled) {
                //If last call reduced the speed : theorical_regulation_speed is reached
                delay0 = regulation_delay;
                regulation_unreached = false;
                return;
            }
            //if not, acceleration
            set_speed_distance_fast(true, processing_steps);
            speed_incr = true;
            regulation_stop_enabled = true;

            return;
        } else if (delay0 < regulation_delay) {
            //if speed is too high :

            if (speed_incr && regulation_stop_enabled) {
                //If last call increased the speed : theorical_regulation_speed is reached

                delay0 = regulation_delay;
                regulation_unreached = false;
                return;
            }
            //if not, deceleration
            set_speed_distance_fast(false, processing_steps);
            speed_incr = false;
            regulation_stop_enabled = true;

            return;
        }
    }
}


/*
 * set_speed_distance : this function updates the speed_distances, and its square root, with a call to sqrt.
 *
 * It also sets correct variables for the fast algorithm.
 */
void SpeedManager::set_speed_distance(unsigned long distance_to_end) {
    speed_distance = distance_to_end;
    distance_square_root = (unsigned int) sqrt(distance_to_end);
    square_sup = square_increments = ((unsigned int) 2 * distance_square_root + 1);
    square_inf = 1;
    speed_processing_required = true;
}


/*
 * set_speed_distance_fast : this function updates the speed_distance and its square root,
 *      with a more efficient algorithm than calling sqrt
 *
 * If positive_incr is true, it increments speed_distance with incr value. If not, it decrements it with incr value.
 *
 * This algorithm is based on the formula n*n = 1 + 3 + 5 + ... + (2.n-1)
 *
 * When the square root is modfied, it sets speed_processing_required, so that delay (speed) will be modified aferwards.
 *
 * The delay is set according to  : delay = delay_numerator/sqrt(speed_distance)
 *
 */
void SpeedManager::set_speed_distance_fast(bool positive_incr, unsigned int incr) {

#define switch_up square_inf = 1, square_increments+=2, square_sup = square_increments, distance_square_root++, speed_processing_required = true;
#define switch_down if(distance_square_root) { square_increments-=2, square_inf=square_increments, square_sup=1, distance_square_root--;speed_processing_required = true;}

    if (positive_incr) {
        //An increment is required :

        //incrementing the distance
        speed_distance += incr;

        //Updating bounds and square root
        while (square_sup < incr) {
            incr -= square_sup;
            switch_up
        }

        //last update, with the reduced increment
        square_sup -= incr, square_inf += incr;
    } else {
        //An decrement is required :

        if (speed_distance > incr) {
            //If we need to decrement less than the speed distance

            //decrementing the distance
            speed_distance -= incr;

            //Updating bounds and square root
            while (square_inf < incr) {
                incr -= square_inf;
                switch_down
            }

            //last update, with the reduced increment
            square_sup += incr, square_inf -= incr;
            return;
        } else {
            //If we need to decrement more than the speed distance : speed distance goes to zero (necessarily >= 0);

            speed_distance = 0;
            square_inf = square_sup = square_increments = 1;
            distance_square_root = 0;

        }
    }

#undef switch_down
#undef switch_up
}

/*
 * go_to_speed_distance : this function sets speed distance, in absolute mode.
 *
 * It calls set_speed_distance_fast in relative mode.
 */

void SpeedManager::go_to_speed_distance(unsigned long distance_to_end) {

    long d = distance_to_end - speed_distance;

    bool positive_incr;
    if (!(positive_incr = d > 0)) d = -d;

    set_speed_distance_fast(positive_incr, (unsigned int) d);

}


float last_ratio;
/*
 * set_delay_parameters : this function is called before a movement is actually executed.
 *
 *  It uses the 3 parameters pre-computed during the motion scheduling by MotionScheduler::pre_set_speed_axis :
 *      - ratio : the distance ratio, used to adapt the speed management to one axis, in linear moves (1 for others);
 *      - tmp_delay_numerator : the new delay numerator;
 *      - tmp_regulation_delay : the new regulation delay;
 */
void SpeedManager::set_delay_parameters(unsigned int tmp_regulation_delay, unsigned int tmp_delay_numerator, float ratio, unsigned char procesing_steps) {
    //Set speed_distance and delay0

    unsigned int tmp_delay_0;
    if (speed_distance != 0) {
        tmp_delay_0 = (unsigned int) ((float) delay0 * ratio / last_ratio);
        unsigned long sqrt_new_speed_distance = (unsigned long) ((float) tmp_delay_numerator / (float) tmp_delay_0);//TODO FLOAT NECESSAIRE
        //TODO DIRECTLY SET SPEED_DISTANCE, SQRT AND OTHER_PARAMETERS
        go_to_speed_distance(sqrt_new_speed_distance*sqrt_new_speed_distance);
    } else {
        tmp_delay_0 = tmp_delay_numerator;//TODO DN -> uint16_t
    }

    last_ratio = ratio;
    delay0 = tmp_delay_0;
    set_stepper_int_period(tmp_delay_0);
    delay_numerator = tmp_delay_numerator;
    regulation_delay = tmp_regulation_delay;
    regulation_unreached = true;
    SpeedManager::processing_steps = procesing_steps;

    CommandInterface::echo(String(delay0)+" "+String(OCR5A));

}


#define m SpeedManager

//ACCELERATION DELAY

unsigned long m::speed_distance = 0, m::distance_to_end;
unsigned int m::square_inf = 1, m::square_sup = 1, m::distance_square_root = 0, m::square_increments = 1;

unsigned int m::regulation_delay;
unsigned int m::delay0;

bool m::regulation_unreached = false;

bool m::speed_incr = true;
bool m::regulation_stop_enabled = false;

bool m::speed_processing_required;

unsigned char m::processing_steps;

float m::delay_numerator;
