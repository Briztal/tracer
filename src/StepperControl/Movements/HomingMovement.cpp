/*
  HomingMotion.cpp - Part of TRACER

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

#include <config.h>
#include <StepperControl/TrajectoryTracer.h>
#include <StepperControl/MovementCoordinator.h>
#include <StepperControl/SteppersData.h>

#ifdef ENABLE_STEPPER_CONTROL

#include "HomingMovement.h"
#include "StepperControl/Steppers.h"

#define step 10


/*
 * prepare_movement : this function will compute all data required to home the machine,
 *  and start the movement interrupt routine;
 */

void HomingMovement::prepare_movement(sig_t reset, uint8_t *endstops, sig_t directions) {

    //If movements are enqueued, execute them before homing;
    TrajectoryTracer::start();

    //Wait for movement to be finished;
    while (MovementCoordinator::started());

    //Reserve the usage of the stepper routine
    MovementCoordinator::reserve();


    /*
     * Delays update :
     *
     * The delay for a stepper simply corresponds to its max jerk;
     */

    //For each stepper :
    for (int axis = 0; axis < NB_STEPPERS; axis++) {

        //Cache the appropriate stepper data;
        stepper_data_t *data = SteppersData::steppers_data + axis;

        //Set the delay for the i-th stepper
        delays[axis] = (uint32_t) (1000000 / (data->jerk * data->steps_per_unit));
    }


    /*
     * Fields udpate :
     */

    //First step is to generate the signature for all axis;
    movement_signature = reset;

    //Update the number of axis to reset, with the cardinal of the signature;
    nb_axis = cardinal(reset);

    //Copy the entire content of the endstops indices array;
    memcpy(endstops_indices, endstops, nb_axis * sizeof(uint8_t));

    //Initialise the step period;
    step_period_us = get_movement_delay(reset, delays);


    /*
     * Steppers config :
     */

    //Set the steppers directions;
    Steppers::set_directions(directions);

    //Enable all steppers
    Steppers::enable(255);

}


/*
 * cardinal : this function counts the number of set bits in the signature;
 */

uint8_t HomingMovement::cardinal(sig_t signature) {

    //Initialise the counter;
    uint8_t counter = 0;

    //While all 1 have not all been counted :
    while (signature) {

        //If the last bit is a 1 :
        if (signature & 1) {

            //Increment the counter;
            counter++;

        }

        //Shift the signature;
        signature >>= 1;
    }

    //Return the nb of 1 in the signature;
    return counter;

}


/*
 * phase_1 : this function steps_per_unit all remaining axis, and eventually updates the signature and delay.
 *
 *  Then, it re-sechedules itself;
 */

void HomingMovement::phase_1() {

    //Disable the stepper interrupt for safety;
    disable_stepper_interrupt();

    //Step;
    Steppers::fastStep(movement_signature);

    //If we made [step] steps_per_unit since last update :
    if (!--step_index) {

        //Reset the step index;
        step_index = step;

        //Update the signature;
        movement_signature = read_endstops();

        //update the delay;
        step_period_us = get_movement_delay(movement_signature, delays);

        //Update the interrupt period;
        set_stepper_int_period(step_period_us);

    }

    //If steppers still need to move;
    if (movement_signature) {

        //Re-schedule the phase 1;
        enable_stepper_interrupt();

    } else {

        return;
        /*
        //If steppers still need to move;

        //Schedule the phase 1;
        set_stepper_int_function(phase_2);
        enable_stepper_interrupt();

         TODO phase 2 and 3;
         */

    }

}


/*
 * get_movement_delay : this function will compute the delay for the movement corresponding to the provided
 *      signature, with the provided delay array;
 */

float HomingMovement::get_movement_delay(sig_t signature, const float *const delays) {

    //Initialise the final delay to zero.
    float delay = 0;

    //For each axis :
    for (uint8_t axis = 0; axis < NB_STEPPERS; axis++) {

        //If the stepper must move :
        if (signature & (uint32_t) 1) {

            //Cache the delay for the current axis;
            float new_delay = delays[axis];

            //The final delay is the maximum of the max delay and the current axis delay;
            delay = max(delay, new_delay);

            break;
        }

        //Pass to the next axis;
        signature >>= 1;

    }

    //Return the maximum delay;
    return delay;
}


/*
 * read_endstops : this function will compute the new movement signature, according to endstops data;
 */

sig_t HomingMovement::read_endstops() {

    //Get the endstops_signature;
    sig_t endstops_signature = 0;

    //Initialise the movement signature;
    sig_t movement_signature = 0;

    //For every axis to reset :
    for (uint8_t i = 0; i < nb_axis; i++) {

        //Get the index of the stepper assigned to the current stepper;
        uint8_t index = endstops_indices[i];

        //If the bit at positon [index] is reset (if the [i-th] stepper must still move) :
        if (!(endstops_signature & (1 << index))) {

            //Set the [i]-th bit in the movement signature, so that stepper [i] keeps moving;
            movement_signature |= (1 << i);

        }
    }

    //return the movement signature;
    return movement_signature;

}


//------------------------------------ Static declarations - definitions ------------------------------------

#define m HomingMovement

//The initial number of axis to reset;
uint8_t m::nb_axis = 0;

//The interrupt period;
float m::step_period_us = 0;

//The delays array
float t_hm_dels[NB_STEPPERS]{0};
float *const m::delays = t_hm_dels;

//The array of endstops indices;
uint8_t t_hm_ed[NB_STEPPERS]{0};
uint8_t *const m::endstops_indices = t_hm_ed;

//The movement signature;
sig_t m::movement_signature = 0;

//The step index;
uint8_t m::step_index = step;


#endif