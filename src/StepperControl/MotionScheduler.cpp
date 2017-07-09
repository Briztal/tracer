/*
  MotionScheduler.cpp - Part of TRACER

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
#include "MotionScheduler.h"
#include "StepperController.h"
#include "MotionExecuter.h"

#include "../Interfaces/CommandInterface.h"
#include "../Actions/ContinuousActions.h"



void MotionScheduler::begin() {

    theorical_regulation_speed = 400;
    for (unsigned char axis = 0; axis<NB_STEPPERS; axis++) {
        axis_signatures[axis] = (unsigned char) (1 << axis);
    }

}


void MotionScheduler::send_position() {

#ifdef position_log
    StepperController::send_position();

#else
    CI::echo("POSITION");
    float t[NB_STEPPERS];
    for (int axis = 0; axis < NB_STEPPERS; axis++) {
        t[axis] = (MotionScheduler::positions[axis] / EEPROMStorage::steps[axis]);
    }
    CI::send_position(t);
#endif

}

//TODO CHECK MAX_SPEED
void MotionScheduler::setSpeed(float speed) {
    theorical_regulation_speed = speed;
}


/*
 * get_regulation_speed : this function calculates the real regulation speed, from the theoretical one.
 *
 *  It verifies that the speed on each axis does not overpasses the maximum speed on this axis.
 *
 *  If it does, it determines the ratio to reduce the speed, for each axis where the speed is too high.
 *
 *  It applies the maximum ratio.
 */
float MotionScheduler::get_regulation_speed(float *const distsmm, const float inv_sqrt_square_dist_sum) {

    float scoeff = theorical_regulation_speed * inv_sqrt_square_dist_sum;

    float r = 0;
    bool init = true;
    float speed, tms;

    for (unsigned char axis = 0; axis < NB_STEPPERS; axis++) {
        //Maximum steps step
        if ((speed = (*(distsmm + axis)) * scoeff) > (tms = EEPROMStorage::maximum_speeds[axis])) {//TODO RESTORE AXIS_t
            r = (init) ? tms / speed : min(r, tms / speed);
            init = false;
        }
    }

    float regulation_speed = theorical_regulation_speed;

    CI::echo("theorical : "+String(theorical_regulation_speed));

    if (r != 0) {
        regulation_speed *= r;
    }

    CI::echo("real : "+String(regulation_speed));


    return regulation_speed;

}


/*
 * pre_set_speed_axis : this function is called during the scheduling of a movement.
 *
 *  It prepares the speed change, before the currently planned movement is really executes.
 *
 *  It sets three parameters :
 *      - ratio : the distance ratio, used to adapt the speed management to one axis, in linear moves (1 for others);
 *      - tmp_delay_numerator : the delay numerator that will be used during the movement we now plan;
 *      - tmp_regulation_delay : the regulation delay that will be used during the movement we now plan.
 */
void MotionScheduler::pre_set_speed_axis(unsigned char new_axis, float distance_coefficient, float regulation_speed) {


    //the acceleration considered is the new axis acceleration
    float acceleration = EEPROMStorage::accelerations[new_axis];
    float steps = EEPROMStorage::steps[new_axis];

    CI::echo("s-a-c-r : "+String(steps)+" "+String(acceleration)+" "+String(distance_coefficient)+" "+String(regulation_speed));

    float ratio = steps / distance_coefficient;
    unsigned int tmp_delay_numerator = (unsigned int) (1000000 / sqrt(2 * steps * acceleration));
    unsigned int tmp_regulation_delay = (unsigned int) (1000000 / (steps * distance_coefficient * regulation_speed));

    CI::echo("REGULATION_DELAY : "+String(tmp_regulation_delay));

    MotionExecuter::fill_speed_data(tmp_delay_numerator, tmp_regulation_delay, ratio);

}

//TODO ACCELERATION CHECKING ON OTHER AXIS
//TODO CALCULER (ts *ta ) AU PREALABLE MATRCIELLEMENT

#undef LOCAL_STEP_AND_WAIT






















#define m MotionScheduler


//POWER AND SPEED
float m::theorical_regulation_speed; //mm per second



long pos[NB_STEPPERS];
long *const m::positions = pos;


#define STEPPER(i, j, si, st, sp, ac, dp, ps, pd, pinPower, pmi, vi, pma, va) 1<<i,

unsigned char tmp_signature[NB_STEPPERS];
unsigned char *const m::axis_signatures = tmp_signature;

//Fields for theorical_regulation_speed processing :

unsigned char m::linear_tools_nb;

void (*function_array[NB_CONTINUOUS])(float);

void (**m::linear_set_functions)(float) = function_array;




/*

long d0[NB_STEPPERS], d1[NB_STEPPERS];
long *m::d_0 = d0, *m::d_1 = d1;
long *m::end_distances = d0, *m::next_end_distances = d1;

unsigned char do0[NB_STEPPERS], do1[NB_STEPPERS];
unsigned char *m::d_o_0 = do0, *m::d_o_1 = do1;
unsigned char *m::distances_order = do0, *m::next_distances_order = do1;

bool m::is_d_0 = true;
 */



/*
 //Acceleration checking

float *accelerations = EEPROMStorage::accelerations;
float maxValue = 0;
unsigned char m_axis = 0;
float temp_time;
for (unsigned char axis = 0; axis < dimension; axis++) {
    //Maximum acceleration step
    //Dists are in correct order, not accelerations so axis_t must be used
    if ((temp_time = distsmm[axis] / accelerations[axis_t[axis]]) > maxValue) {
        m_axis = axis;
        maxValue = temp_time;
    }
}

if (m_axis != 0) {
    acceleration0 = *distsmm / maxValue;
} else {
    acceleration0 = accelerations[axis_t[0]];
}
 */