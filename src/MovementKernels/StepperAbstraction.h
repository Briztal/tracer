/*
  StepperAbstraction.h - Part of TRACER

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
 * StepperAbstraction : This class is in charge of the interface between high level and stepper coordinates
 *
 * Options :
 *      - Create Upper coordinate system
 *      - Translation to basement coordinate
 *      -
 *
 *
 * Process for tracing a line in the stepper coordinates from Ar to Br :
 *
 *      Trajectory tracing :
 *
 *      - get As and Bs, the equivalent of Ar and Br in the stepper referential
 *      - divide the segment [AsBs] in the stepper referential in consecutive positions
 *      - go to these positions, using an elementary movement algorithm.
 *      - The speed is the same for all elementary moves, as speed is constant in a line
 *
 *      Acceleration management :
 *
 *      Jerk points and stop points are defined across the trajectory. An heuristic distance
 *          (here the max distance of all axis) is calculated, and as the speed on each axis is known, the deceleration
 *          distance is known too. As a consequence, the speed is modified if the deceleration distance is greater than
 *          the distance to the jerk point.
 *
 *
 *
 * Process for tracing any type of of trajectory in the high level coordinates :
 *
 *      Trajectory tracing :
 *      - Divide the trajectory in consecutive positions, expressed in high level coordinates;
 *      - Translate each position into Stepper coordinates;
 *      - Go to These positions, using the elementary movement algorithm :
 *      - Before beginning each elementary movement, get the greater axis (the axis that moves the most)
 *      - get the delay time, projected on this axis.
 *      - Make the elementary movement with the delay.
 *
 *      Speed Management :
 *      - The speed is increased according to the acceleration bounds (see Acceleration Managemet below)
 *      - The delay is updated continuously, before each elementary movement
 *
 *      Acceleration Management :
 *      - The constraint is to have no axis accelerating more than its limit.
 *      - As the trajectory is not known, speeds on each axis either are not. So, we must consider that all axis are
 *          at their maximum speed. It is a very restrictive hypothesis.
 *      - Csq : the maximum acceleration is known.
 *      - At each delay axis change, the deceleration distance is upddated.
 *
 *
 *
 */


#ifndef TRACER_STEPPERABSTRACTION_H
#define TRACER_STEPPERABSTRACTION_H


class StepperAbstraction {



    //--------------------------------------------current_stepper_positions---------------------------------------------

public:

    static void translate(const float *const hl_coordinates, long *const steppers_coordinates);

    static void invert(const long *const steppers_coordinates, float *const hl_coordinates);


    //--------------------------------------------current_stepper_positions---------------------------------------------

private :

    static float *current_position;

public :

    static void get_current_position(float *const position);

    void update_position(const float *const new_position);

};


#endif //TRACER_STEPPERABSTRACTION_H
