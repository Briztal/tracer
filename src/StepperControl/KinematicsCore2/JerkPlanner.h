/*
  JerkPlanner.h - Part of TRACER

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


#ifndef TRACER_JERKPLANNER_H
#define TRACER_JERKPLANNER_H


#include "StepperControl/_kinematics_data.h"


class JerkPlanner {

public :

    static bool determine_jerk(const k2_movement_data *current_movement, k2_movement_data *previous_movement);

    static void propagate_jerk(const k2_movement_data *current_movement, k2_movement_data *previous_movement);


private :

    static float *const final_jerk_ratios;

    static void get_jerk_offsets(float *initial_steps_distances, const uint8_t speed_group, uint32_t *jerk_distances_offsets);

    static void update_jerk_final_data(float *final_steps_distances, const uint8_t speed_group);

    static void get_stepper_distances(const float p0, const float p1, void (*trajectory_function)(float, float *),
                                      float *distances);
};



#endif //TRACER_JERKPLANNER_H
