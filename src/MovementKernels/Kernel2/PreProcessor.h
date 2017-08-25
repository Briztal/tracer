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


#include <stdint.h>

#ifdef ENABLE_STEPPER_CONTROL

#ifndef TRACER_INCREMENT_COMPUTER_H
#define TRACER_INCREMENT_COMPUTER_H


class PreProcessor {

public:

    static float
    pre_process_increment_and_jerk(const float min, const float max, void (*trajectory_function)(float, float *),
                                   const uint8_t speed_group, bool jerk_checking, uint32_t *jerk_distances_offsets);

    static void
    verify_jerk_absorptions(uint32_t *enqueued_jerk_offsets, int32_t *enqueued_jerk_positions, uint32_t *new_jerk_offsets,
                            int32_t *new_jerk_positions);

private :


    static void
    get_jerk_offsets(float *initial_steps_distances, const uint8_t speed_group, uint32_t *jerk_distances_offsets);


    static float extract_increment(void (*get_position)(float, float *), float point, float incr);

    static float *const final_jerk_ratios;

    static uint32_t get_max_dist(float *p0, float *p1);

    static void get_stepper_position(void (*get_position)(float, float *), float point, float *positions);

    static void update_jerk_final_data(float *final_steps_distances, const uint8_t speed_group);

    static void get_stepper_distances(const float p0, const float p1, void (*trajectory_function)(float, float *),
                          float *distances);


};


#endif //TRACER_INCREMENT_COMPUTER_H

#endif