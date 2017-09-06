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



#include <config.h>

#ifdef ENABLE_STEPPER_CONTROL


#ifndef TRACER_JERKPLANNER_H
#define TRACER_JERKPLANNER_H


#include "StepperControl/_kinematics_data.h"


class JerkPlanner {

public :

    static bool control_and_initialise_jerk(const movement_data_t *current_movement, movement_data_t *previous_movement,
                                            bool jerk_control);

private :


    static void get_stepper_distances(const float p0, const float p1, void (*trajectory_function)(float, float *),
                                      float *distances);

    static void get_final_jerk_ratios(float *final_steps_distances, const uint8_t speed_group, float *final_jerk_ratios);

    static float get_jerk_point_speed(const float *initial_steps_distances, const uint8_t speed_group,
                                                   const float *final_jerk_ratios);

    static float *const saved_final_stepper_positions;



private:


};


#endif //TRACER_JERKPLANNER_H

#endif