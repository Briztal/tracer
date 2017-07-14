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
 * /APPEL N : Calcul des signatures de départ
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
 * initialise : this function enqueues the first linear move, to go to the first position.
 *
 *  After doing this, it selects the correct axis,
 */
void NonLinearMotionN::initialise(unsigned char *move_axis_t, const unsigned char size) {
    for (unsigned char indice = 0; indice < size; indice++) {
        unsigned char axis = axis_t[indice];
        axis_t[indice] = axis;
        steps[indice] = EEPROMStorage::steps[axis];
        //assignFunction(axis, dir_set_functions + indice);
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
    for (unsigned char indice = 0; indice < movement_size; indice++) {
        unsigned char axis = axis_t[indice];
        long pos = MotionScheduler::positions[axis], npos = final_positions[axis];
        MotionScheduler::positions[axis] = npos;
        MotionExecuter::end_distances[axis] += npos - pos;
    }
}

void NonLinearMotionN::fill_processors(void (*init_f)(), unsigned char (*position_f)(unsigned char *)) {
    MotionExecuter::fill_processors(init_f, step, position_f, process_speed);
}

void NonLinearMotionN::set_speed_parameters_enqueue(unsigned char processing_steps) {
    set_speed_coefficients();
    float dists_array[NB_STEPPERS]{0};
    for (unsigned char indice = 0; indice<movement_size; indice++) {
        dists_array[axis_t[indice]] = 1;
    }
    float regulation_speed = MotionScheduler::get_regulation_speed_linear(dists_array, 1);
    MotionScheduler::pre_set_speed_axis(*axis_t, 1, regulation_speed, processing_steps);
    MotionExecuter::enqueue_movement_data();

}

void NonLinearMotionN::set_speed_coefficients() {
    float invsteps0 = 1.0 / *steps;
    for (unsigned char indice = 1; indice < movement_size; indice++) {
        speed_coeffs[indice] = steps[indice] * invsteps0;
    }
}

/*
 * set_motion_data : extracts and fills the movement data required to enqueue the data :
 *      (initial/final) (movement/dir) signatures, and number of sub movements.
 */
void NonLinearMotionN::set_motion_data(const float min, const float max, const float step,
                                       void (*get_relative_position)(float, long *)) {

    unsigned char elementary_dists[movement_size];

    //Number of sub_movements
    unsigned int count = (unsigned int) ((max - min) / step);

    //Get initial data (dists and dir signature)
    unsigned char nsig = get_movement_distances(min, min + step, get_relative_position, elementary_dists);

    //Fill initial signatures (movement and dir)
    MotionExecuter::fill_movement_data(true, elementary_dists, count, nsig);

    //Get final data (dists and dir signature)
    nsig = get_movement_distances(count * step, max, get_relative_position, elementary_dists);

    //Fill final signatures (movement and dir)
    MotionExecuter::fill_movement_data(false, elementary_dists, count, nsig);

}

/*
 * get_movement_distance : gets the distance for a movement from point_0 to point_1,
 *      for a curve given by the get_relative_position function.
 *      Returns the direction signature for this movement.
 */
unsigned char NonLinearMotionN::get_movement_distances(float point_0, float point_1,
                                                       void (*get_relative_position)(float, long *),
                                                       unsigned char *distances) {

    //Arrays to contain position;
    long td0[movement_size], td1[movement_size];

    //Get relative positions for both points
    get_relative_position(point_0, td0), get_relative_position(point_1, td1);

    //Direction signature
    unsigned char nsig = 0;

    //Pointers to position arrays;
    const long *dptr0 = td0, *dptr1 = td1;

    //Get effective distances and signature.
    for (unsigned char indice = 0; indice < movement_size; indice++) {

        //Get relative distance and increment pointers
        int d = (int) (*(dptr1++) - *(dptr0++));

        //Convert to absolute distance, and set bit in signature if dist is negative
        if (d < 0) {
            nsig |= MotionScheduler::axis_signatures[axis_t[indice]];
            d = -d;
        }

        //Save distance
        *(distances++) = (unsigned char) (d);
    }

    return nsig;
}

//--------------------------------------------Increment_extraction------------------------------------------------------


float
NonLinearMotionN::extract_increment(float point, const float end, float increment, const unsigned char processing_steps,
                                    const void(*get_position)(float, long *)) {
    const unsigned char ms = movement_size;

    long pos[ms], dest[ms];
    unsigned int dist;
    float tmp;

    increment = adjust_increment(point, increment, processing_steps, get_position);
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

        memcpy(pos, dest, ms << 2);
    }

    return increment;
}


float NonLinearMotionN::adjust_increment(const float point, float increment, const unsigned char processing_steps, const void(*get_position)(float, long *)) {

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


void NonLinearMotionN::step(unsigned char sig) {
    unsigned int delay = StepperController::fastStepDelay(sig);
    set_stepper_int_period(delay);
}


void NonLinearMotionN::process_speed() {

    //speed_processing_1 :
    float inverse = invert(SpeedManager::distance_square_root);
    STEP_AND_WAIT;

    unsigned int tmpdelay = SpeedManager::delay0 = (unsigned int) (SpeedManager::delay_numerator * inverse);
    SpeedManager::delay0 = StepperController::delays[*axis_t] = tmpdelay;

    for (unsigned char i = movement_size; i--;) {
        STEP_AND_WAIT;
        StepperController::delays[axis_t[i]] = (unsigned int) (speed_coeffs[i] * (float) tmpdelay);
    }

    STEP_AND_WAIT;
    return;

}


#define m NonLinearMotionN

unsigned char m::movement_size;

float tstps[NB_STEPPERS];
float *const m::steps = tstps;

float tsc[NB_STEPPERS];
float *const m::speed_coeffs = tsc;

void (*t_fdirs[NB_STEPPERS])(bool dir);

//void (**m::dir_set_functions)(bool dir) = t_fdirs;


/*
void NonLinearMotionN::checkPositionByStep(float *offsets, unsigned int delay, unsigned int count,
                                           void (*process_position)()) {
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
 */