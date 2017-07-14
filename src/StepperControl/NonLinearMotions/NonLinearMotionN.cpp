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
 * TODOs for a correct motion setup :
 *      set end distances
 *      set first elementary_distances, calling MotionExecuter::fill_movement_data(true,  ... );
 *      set last  elementary_distances, calling MotionExecuter::fill_movement_data(false, ... );w
 *      set speed data, calling MotionScheduler::pre_set_speed_axis(...);
 *      set processing functions, calling MotionExecuter::fill_processors(...);
 *      call MotionExecuter::enqueue_movement_data
 *
 */

/*
 * Procedure :
 *
 * Appel de la fonction de mouvement d'une classe implémentée.
 *
 * /Appel N : Initialisation des variables (steps, acceleration, dirs...)
 * Calcul des positions de départ (steps)
 * /Appel N : ENQUEUE DU MOUVEMENT DE DEPART
 *
 * Calcul des positions d'arrivee (steps)
 * Push des positions d'arrivée.
 * /APPEL N : MAJ des distances d'arrivée
 *
 * /APPEL N : extraction increment
 * calcul premier mouvement
 *
 * APPEL N : Calcul des signatures de départ
 * Appel N : set des processors
 *
 * Appel N : speed - enqueue
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
void NonLinearMotionN::pre_process(unsigned char *move_axis_t, const unsigned char size) {
    for (unsigned char indice = 0; indice < size; indice++) {
        unsigned char axis = axis_t[indice];
        axis_t[indice] = axis;
        steps[indice] = EEPROMStorage::steps[axis];
        assignFunction(axis, dir_set_functions + indice);
    }

    movement_size = size;

}

void NonLinearMotionN::set_initial_positions(const float *initial_positions) {

    float first_move_destinations[NB_STEPPERS];
    memcpy(first_move_destinations, MotionScheduler::positions, 4 * NB_STEPPERS);

    for (unsigned char indice = 0; indice < movement_size; indice++) {
        unsigned char axis = axis_t[indice];
        first_move_destinations[axis] = initial_positions[axis];
    }

    LinearMotionN::prepare_motion(first_move_destinations);

}

void NonLinearMotionN::set_final_positions(const long *final_positions) {
    for (unsigned char indice = 0; indice<movement_size; indice++) {
        unsigned char axis = axis_t[indice];
        long pos = MotionScheduler::positions[axis], npos = final_positions[axis];
        MotionScheduler::positions[axis] = npos;
        MotionExecuter::end_distances[axis] += npos-pos;
    }
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

    set_motion_data();

    draw();

    send_position();

}


void NonLinearMotionN::set_speed_coefficients() {
    float invsteps0 = 1.0 / *steps;
    for (unsigned char axis = 1; axis < movement_size; axis++) {
        speed_coeffs[axis] = steps[axis] * invsteps0;
    }
}

/*
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
 */


//TODO DIRECTION SIGNATURE
void NonLinearMotionN::set_motion_data(const unsigned int count, const int *const first_dists) {

    //Step 1 : direction signature and elementary dists extraction.

    unsigned char nsig = 0;

    unsigned char elementary_dists[movement_size];
    const int *dptr = first_dists;
    unsigned char * cptr = elementary_dists;

    for (unsigned char indice = 0; indice < movement_size; indice++) {
        int d = *(dptr++);
        if (d<0) {
            nsig |= MotionScheduler::axis_signatures[axis_t[indice]];
            d = -d;
        }
        *(cptr++) = (unsigned char) (d);
    }

    //TODO FIRST_MOVEMENT
    //Fill beginning signatures
    MotionExecuter::fill_movement_data(true, elementary_dists, count, nsig);


}


float NonLinearMotionN::extract_increment(float point, const float end, float increment, const unsigned int processing_steps, const void(*get_position)(float, long*)) {
    const unsigned char ms = movement_size;

    long pos[ms], dest[ms];
    unsigned int dist;
    float tmp;

    increment = adjust_increment(point, increment, get_position);
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


float NonLinearMotionN::adjust_increment(const float point, float increment, const void(*get_position)(float, long*)) {

    unsigned char dist;

    long pos[movement_size], dest[movement_size];
    get_position(point, pos);
    get_position(point + increment, dest);
    dist = get_distance(dest, pos);

    bool lastSign = ((dist) > processing_steps);
    float incr = increment / 5.;

    //if dist>processing steps, incr must be opposed to increment
    if (lastSign)
        incr *= -1;

    //Dichotomy
    while (dist != processing_steps) {
        //If the state changed :
        if (lastSign ^ (dist > processing_steps)) {
            incr *= -1. / 5.;
            lastSign = (dist > processing_steps);
        }

        increment += incr;
        get_position(point + increment, dest);
        dist = get_distance(dest, pos);
    }

    return increment;

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


void NonLinearMotionN::checkPositionByStep(float *offsets, unsigned int delay, unsigned int count, void (*process_position)()) {
    //reset_data_pointers();
    for (unsigned int i = 0; i < count; i++, alpha += increment) {
        process_position();

        float t[movement_size];
        for (int axis = 0; axis < movement_size; axis++) {
            t[axis] = pos_t[axis] / steps[axis] + offsets[axis];
        }

        CI::send_position(t);
        delayMicroseconds(delay);
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

/*
long tlpt[NB_STEPPERS];
long *const m::last_pos_t = tlpt;

long tfpt[NB_STEPPERS];
long *const m::first_pos_t = tfpt;

long tfpos[NB_STEPPERS];
long *const m::first_positions = tfpos;

long tlpos[NB_STEPPERS];
long *const m::last_positions = tlpos;

 */

void (*t_fdirs[NB_STEPPERS])(bool dir);

void (**m::dir_set_functions)(bool dir) = t_fdirs;

