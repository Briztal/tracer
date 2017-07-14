/*
  NonLinearMotionN.cpp - Part of TRACER

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


#include "NonLinearMotionN.h"
#include "../../Interfaces/CommandInterface.h"
#include "../LinearMotionN/LinearMotionN.h"
#include "../../Core/EEPROMStorage.h"
#include "../SpeedManager.h"
#include "../mathProcess.hpp"
#include "../MotionExecuter.h"
#include "../StepperController.h"
#include "../MotionScheduler.h"

/*
 * Procedure :
 *
 * Appel de la fonction de mouvement d'une classe implémentée.
 * Appel N : Initialisation des variables (steps, acceleration, dirs...)
 * Calcul des positions de départ (steps)
 *
 */

/*
void NonLinearMotionN::initialise_motion() {
    unsigned char a;
    for (unsigned char axis = 0; axis < movement_size; axis++) {
        a = axis_copy_t[axis] = axis_t[axis];
        steps[axis] = EEPROMStorage::steps[a];
        assignFunction(a, dir_set_functions + axis);
    }
}
*/
/*
void NonLinearMotionN::set_last_position() {
    Motion::set_end_position(last_positions, axis_t, movement_size);
}
*/

/*
 * pre_process : this function enqueues the first linear move, to go to the first position.
 *
 *  After doing this, it selects the correct axis,
 */
void NonLinearMotionN::pre_process(unsigned char *move_axis_t, const float *dests, const unsigned char size) {
    float first_move_destinations[NB_STEPPERS];
    memcpy(first_move_destinations, MotionScheduler::positions, 4*NB_STEPPERS);

    for (unsigned char indice = 0; indice < size; indice++) {
        unsigned char axis = move_axis_t[indice];
        first_move_destinations[axis] = dests[move_axis_t[indice]];
    }

    LinearMotionN::prepare_motion(first_move_destinations);

    for (unsigned char indice = 0; indice < size; indice++) {
        unsigned char axis = axis_t[indice];
        axis_t[indice] = axis;
        steps[indice] = EEPROMStorage::steps[axis];
        assignFunction(axis, dir_set_functions + indice);
    }

    movement_size = size;

}

void NonLinearMotionN::move() {

    //Dir Setting

    for (unsigned char axis = 0; axis < movement_size; axis++) {
        (**(dir_set_functions + axis))(true);
    }

    memset(dirs, true, movement_size);

    set_signatures();

    set_speed_coefficients();


    increment_provided = false;

    //Speed axis setting
    set_speed_axis(*axis_t, 1);

    //Child class init
    init_position_parameters();

    initialise_motion_vars();

    draw();

    send_position();

}


void NonLinearMotionN::set_speed_coefficients() {
    float invsteps0 = 1.0 / *steps;
    for (unsigned char axis = 1; axis < movement_size; axis++) {
        speed_coeffs[axis] = steps[axis] * invsteps0;
    }
}

void NonLinearMotionN::set_end_distances() {
    bool * d = dirs;
    int i;
    for (int axis = movement_size-1; axis--;) {
        i = (int) (last_pos_t[axis] - pos_t[axis]);
        bool is_pos = (i>0);
        elementary_dists[axis] = is_pos ? (unsigned char) (i) : (unsigned char) (-i);
        if (*d^is_pos) {
            (*(dir_set_functions + axis))(is_pos);
            *d= is_pos;
        }
    }

    memcpy(positions, last_positions, movement_size<<2);
}


void NonLinearMotionN::initialise_motion_vars() {

    //Determine increment if necessary;
    if (!increment_provided) {
        increment = extract_increment(min, max, 0.0005, processing_steps);
    }

    //Initialise positions
    memcpy(pos_t, first_pos_t, movement_size<<2);

    //initialise alpha for the first move;
    alpha = min + increment;

    //Initialise the motion counter
    count = (unsigned int) ((max - min) / increment);

}


float NonLinearMotionN::extract_increment(float point, float end, float increment, unsigned int processing_steps) {
    const unsigned char ms = movement_size;

    long pos[ms], dest[ms];
    unsigned int dist;
    float tmp;

    increment = adjust_increment(point, increment);
    point += increment;
    get_position(point, pos);

    while (point + increment < end) {
        get_position(point + increment, dest);

        dist = get_distance(dest, pos);

        if (dist < processing_steps) {
            tmp = increment / 3.;
            while (dist < processing_steps) {
                increment += tmp;
                get_position(point + increment, dest);
                dist = get_distance(dest, pos);
            }
        }

        point += increment;

        memcpy(pos, dest, ms<<2);
    }

    return increment;
}


float NonLinearMotionN::adjust_increment(float point, float step) {


    unsigned char dist;

    long pos[movement_size], dest[movement_size];
    get_position(point, pos);
    get_position(point + step, dest);
    dist = get_distance(dest, pos);

    bool lastSign = ((dist) > processing_steps);
    float incr = step / 5.;

    //if dist>processing steps, incr must be opposed to step
    if (lastSign)
        incr *= -1;

    //Dichotomy
    while (dist != processing_steps) {
        //If the state changed :
        if (lastSign ^ (dist > processing_steps)) {
            incr *= -1. / 5.;
            lastSign = (dist > processing_steps);
        }

        step += incr;
        get_position(point + step, dest);
        dist = get_distance(dest, pos);
    }

    return step;

}

unsigned char NonLinearMotionN::get_distance(long *dest, long *pos) {
    int d;
    unsigned char dist;
    unsigned char maxi = 0;

#define maximum(a, b) ((a<b) ? b : a)

    for (unsigned char axis = 0; axis < movement_size; axis++) {
        d = (int) (dest[axis] - pos[axis]);
        dist = (unsigned char) ((d < 0) ? -d : d);
        if (maxi < dist) {
            maxi = dist;
        }
    }

    return maxi;
}


void NonLinearMotionN::step_and_delay(unsigned char sig) {
    //simple_delay(StepperController::fastStepDelay(sig));
}


void NonLinearMotionN::checkPositionByStep(float *offsets, unsigned int d) {
    //reset_data_pointers();
    pos_data *p = pos_data_pointer;
    for (int i = 0; i < count; i++, alpha += increment) {
        while (!process_position(p));
        *processors_state = false;

        CI::prepare_data_out();

        float t[movement_size];
        for (int axis = 0; axis < movement_size; axis++) {
            t[axis] = pos_t[axis] / steps[axis] + offsets[axis];
            CI::add_float_out(pos_t[axis] / steps[axis] + offsets[axis]);
        }

        CI::send_position();
        delayMicroseconds(d);
    }
}


void NonLinearMotionN::checkPosition(float *offsets, unsigned int d) {
    long c[NB_STEPPERS];

    for (alpha = min; alpha < max; alpha += increment) {
        get_position(alpha, c);
        for (int axis = 0; axis < movement_size; axis++) {
            CI::add_float_out(c[axis] / steps[axis] + offsets[axis]);
        }

        CI::send_position();
        delayMicroseconds(d);
    }
}

void *nt_spd_ptr;
float tmpdelay;
unsigned char axis;

void NonLinearMotionN::process_speed() {

    //speed_processing_1 :
    float inverse = invert(SpeedManager::distance_square_root);
    STEP_AND_WAIT;

    tmpdelay = SpeedManager::delay0 = (unsigned int) (SpeedManager::delay_numerator * inverse);
    SpeedManager::delay0 =
    StepperController::delays[*axis_t] = (unsigned int) tmpdelay;

    for (unsigned char i = movement_size; i--;) {
        STEP_AND_WAIT;
        StepperController::delays[axis] = (unsigned int) (speed_coeffs[axis] * tmpdelay);
    }

    STEP_AND_WAIT;
    return ;

}

void NonLinearMotionN::provide_increment(float i) {
    increment = i;
    increment_provided = true;
}


#define m NonLinearMotionN

unsigned char m::movement_size;
float m::alpha, m::increment;
bool m::increment_provided = false;
float m::min, m::max;


float tstps[NB_STEPPERS];
float *const m::steps = tstps;

float tsc[NB_STEPPERS];
float *const m::speed_coeffs = tsc;

long tpt[NB_STEPPERS];
long *const m::pos_t = tpt;

long tlpt[NB_STEPPERS];
long *const m::last_pos_t = tlpt;

long tfpt[NB_STEPPERS];
long *const m::first_pos_t = tfpt;

long tfpos[NB_STEPPERS];
long *const m::first_positions = tfpos;

long tlpos[NB_STEPPERS];
long *const m::last_positions = tlpos;

void (*t_fdirs[NB_STEPPERS])(bool dir);

void (**m::dir_set_functions)(bool dir) = t_fdirs;

