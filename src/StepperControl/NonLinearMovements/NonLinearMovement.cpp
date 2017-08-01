/*
  NonLinearMovement.cpp - Part of TRACER

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

#include "../../config.h"

#ifdef ENABLE_STEPPER_CONTROL

#include "NonLinearMovement.h"
#include "../../Interfaces/TreeInterface/TreeInterface.h"
#include "../../Core/EEPROMStorage.h"
#include "../SpeedManager.h"
#include "../mathProcess.hpp"
#include "../MovementExecuter.h"
#include "../StepperController.h"
#include "../SpeedPlanner.h"
#include "../LinearMovement/LinearMovement.h"
#include "../TrajectoryExecuter.h"

/*
 * TODOs for a correct motion setup :
 *      set end distances
 *      set first elementary_distances, calling MotionExecuter::fill_movement_data(true,  ... );
 *      set last  elementary_distances, calling MotionExecuter::fill_movement_data(false, ... );w
 *      set speed data, calling SpeedPlanner::pre_set_speed_axis(...);
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
void NonLinearMovement::initialise_motion() {
    uint8_t a;
    for (uint8_t axis = 0; axis < movement_size; axis++) {
        a = axis_copy_t[axis] = axis_t[axis];
        steps[axis] = EEPROMStorage::steps[a];
        assignFunction(a, dir_set_functions + axis);
    }
}
*/
/*
void NonLinearMovement::set_last_position() {
    Motion::set_end_position(last_positions, axis_t, movement_size);
}
*/

/*
 * initialise : this function enqueues the first linear enqueue_movement, to go to the first position.
 *
 *  After doing this, it selects the correct axis,
 */
void NonLinearMovement::initialise(uint8_t *move_axis_t, const uint8_t size) {
    for (uint8_t indice = 0; indice < size; indice++) {
        uint8_t axis = axis_t[indice];
        axis_t[indice] = axis;
        steps[indice] = EEPROMStorage::steps[axis];
        //assignFunction(axis, dir_set_functions + indice);
    }

    movement_size = size;

}

void NonLinearMovement::set_initial_positions(const float *initial_positions) {

    float first_move_destinations[NB_STEPPERS];
    memcpy(first_move_destinations, SpeedPlanner::positions, 4 * NB_STEPPERS);

    for (uint8_t indice = 0; indice < movement_size; indice++) {
        uint8_t axis = axis_t[indice];
        first_move_destinations[axis] = initial_positions[axis];
    }

    LinearMovement::prepare_motion(first_move_destinations);

}

void NonLinearMovement::set_final_positions(const int32_t *final_positions) {
    for (uint8_t indice = 0; indice < movement_size; indice++) {
        uint8_t axis = axis_t[indice];
        int32_t pos = SpeedPlanner::positions[axis], npos = final_positions[axis];
        SpeedPlanner::positions[axis] = npos;
        SpeedManager::end_distances[axis] += npos - pos;
    }
}

void NonLinearMovement::fill_processors(void (*init_f)(), sig_t (*position_f)(uint8_t *)) {
    TrajectoryExecuter::fill_processors(init_f, step, position_f, process_speed);
}

void NonLinearMovement::set_speed_parameters_enqueue(uint8_t processing_steps) {
    set_speed_coefficients();
    float dists_array[NB_STEPPERS]{0};
    for (uint8_t indice = 0; indice<movement_size; indice++) {
        dists_array[axis_t[indice]] = 1;
    }
    float regulation_speed = SpeedPlanner::get_adjusted_regulation_speed_linear(dists_array, 1);
    //TODO SpeedPlanner::pre_set_speed_axis(*axis_t, 1, regulation_speed, processing_steps);
    TrajectoryExecuter::enqueue_movement_data();

}

void NonLinearMovement::set_speed_coefficients() {
    float invsteps0 = (float) (1.0 / *steps);
    for (uint8_t indice = 1; indice < movement_size; indice++) {
        speed_coeffs[indice] = steps[indice] * invsteps0;
    }
}

/*
 * set_motion_data : extracts and fills the movement data required to enqueue the data :
 *      (initial/final) (movement/dir) signatures, and number of sub movements.
 */
void NonLinearMovement::set_motion_data(const float min, const float max, const float step,
                                       void (*get_relative_position)(float, int32_t *)) {

    uint8_t elementary_dists[movement_size];

    //Number of sub_movements
    uint16_t count = (uint16_t) ((max - min) / step);

    //Get initial data (dists and dir signature)
    uint8_t nsig = get_movement_distances(min, min + step, get_relative_position, elementary_dists);

    //Fill initial signatures (movement and dir)
    TrajectoryExecuter::fill_movement_data(true, elementary_dists, count, nsig);

    //Get final data (dists and dir signature)
    nsig = get_movement_distances(count * step, max, get_relative_position, elementary_dists);

    //Fill final signatures (movement and dir)
    TrajectoryExecuter::fill_movement_data(false, elementary_dists, count, nsig);

}

/*
 * get_movement_distance : gets the distance for a movement from point_0 to point_1,
 *      for a curve given by the get_relative_position function.
 *      Returns the direction signature for this movement.
 */
uint8_t NonLinearMovement::get_movement_distances(float point_0, float point_1,
                                                       void (*get_relative_position)(float, int32_t *),
                                                       uint8_t *distances) {

    //Arrays to contain position;
    int32_t td0[movement_size], td1[movement_size];

    //Get relative positions for both points
    get_relative_position(point_0, td0), get_relative_position(point_1, td1);

    //Direction signature
    uint8_t nsig = 0;

    //Pointers to position arrays;
    const int32_t *dptr0 = td0, *dptr1 = td1;

    //Get effective distances and signature.
    for (uint8_t indice = 0; indice < movement_size; indice++) {

        //Get relative distance and increment pointers
        int d = (int) (*(dptr1++) - *(dptr0++));

        //Convert to absolute distance, and set bit in signature if dist is negative
        if (d < 0) {
            nsig |= SpeedPlanner::axis_signatures[axis_t[indice]];
            d = -d;
        }

        //Save distance
        *(distances++) = (uint8_t) (d);
    }

    return nsig;
}

//--------------------------------------------Increment_extraction------------------------------------------------------


float
NonLinearMovement::extract_increment(float point, const float end, float increment, const uint8_t processing_steps,
                                    const void(*get_position)(float, int32_t *)) {
    const uint8_t ms = movement_size;

    int32_t pos[ms], dest[ms];
    uint16_t dist;
    float tmp;

    increment = adjust_increment(point, increment, processing_steps, get_position);
    point += increment;
    get_position(point, pos);

    while (point + increment < end) {
        get_position(point + increment, dest);

        dist = get_distance(dest, pos);

        if (dist < processing_steps) {
            tmp = (float) (increment / 3.);
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


float NonLinearMovement::adjust_increment(const float point, float increment, const uint8_t processing_steps, const void(*get_position)(float, int32_t *)) {

    uint8_t dist;

    int32_t pos[movement_size], dest[movement_size];
    get_position(point, pos);
    get_position(point + increment, dest);
    dist = get_distance(dest, pos);

    bool lastSign = ((dist) > processing_steps);
    float incr = (float) (increment / 5.);

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

uint8_t NonLinearMovement::get_distance(int32_t *dest, int32_t *pos) {
    int d;
    uint8_t dist;
    uint8_t maxi = 0;

    for (uint8_t axis = 0; axis < movement_size; axis++) {
        d = (int) (dest[axis] - pos[axis]);
        dist = (uint8_t) ((d < 0) ? -d : d);
        if (maxi < dist) {
            maxi = dist;
        }
    }

    return maxi;
}


void NonLinearMovement::step(sig_t sig) {
    uint16_t delay = StepperController::fastStepDelay(sig);
    set_stepper_int_period(delay);
}


void NonLinearMovement::process_speed() {

    //speed_processing_1 :
    float inverse = invert(SpeedManager::distance_square_root);
    STEP_AND_WAIT;

    //TODO uint16_t tmpdelay = SpeedManager::delay0 = (uint16_t) (SpeedManager::delay_numerator * inverse);

    /*SpeedManager::delay0 = StepperController::delays[*axis_t] = tmpdelay;

    for (uint8_t i = movement_size; i--;) {
        STEP_AND_WAIT;
        StepperController::delays[axis_t[i]] = (uint16_t) (speed_coeffs[i] * (float) tmpdelay);
    }

     */

    STEP_AND_WAIT;
    return;

}


#define m NonLinearMovement

uint8_t m::movement_size;

float tstps[NB_STEPPERS];
float *const m::steps = tstps;

float tsc[NB_STEPPERS];
float *const m::speed_coeffs = tsc;

void (*t_fdirs[NB_STEPPERS])(bool dir);

//void (**m::dir_set_functions)(bool dir) = t_fdirs;


/*
void NonLinearMovement::checkPositionByStep(float *offsets, uint16_t delay, uint16_t count,
                                           void (*process_position)()) {
    //reset_data_pointers();
    for (uint16_t i = 0; i < count; i++, alpha += increment) {
        process_position();

        float t[movement_size];
        for (int axis = 0; axis < movement_size; axis++) {
            t[axis] = pos_t[axis] / steps[axis] + offsets[axis];
        }

        CI::send_position(t);
        delayMicroseconds(delay);
    }
}


void NonLinearMovement::checkPosition(float *offsets, uint16_t d) {
    int32_t c[NB_STEPPERS];

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

#endif