/*
  HomingMotion.h - Part of TRACER

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

#ifndef TRACER_HOMINGMOTION_H
#define TRACER_HOMINGMOTION_H

#include <config.h>

#ifdef ENABLE_STEPPER_CONTROL



class HomingMovement {

public:

    static void prepare_movement(uint32_t reset, uint8_t *endstops, uint32_t directions);

private:

    //The function that steps_per_unit the machine;
    static void phase_1();


private:

    //The initial number of axis to reset;
    static uint8_t nb_axis;

    //The interrupt period;
    static float step_period_us;

    //The delays array
    static float *const delays;

    //The array of endstops indices;
    static uint8_t *const endstops_indices;

    //The movement signature;
    static sig_t movement_signature;

    //The step index;
    static uint8_t step_index;



private:

    //Function to compute the sleep for a movement involving specified steppers;
    static float get_movement_delay(uint32_t signature, const float *const delays);

    //Get a new movement signature;
    static uint32_t read_endstops();

    //Get the number of set bits in a signature;
    static uint8_t cardinal(uint32_t signature);

};

#endif //TRACER_HOMINGMOTION_H

#endif