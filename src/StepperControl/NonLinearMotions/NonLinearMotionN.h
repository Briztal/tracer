/*
  NonLinearMotionN.h - Part of TRACER

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


#include "../../config.h"

#ifdef ENABLE_STEPPER_CONTROL

#ifndef CODE_NONLINEARMOTIONN_H
#define CODE_NONLINEARMOTIONN_H


/**
 * A more evolved Class for non trivial plannar motions, ie :
 * - indexed on one float variable;
 * - comprised in a plan containing two axis;
 */

class NonLinearMotionN {


private :

    static unsigned char *const axis_t;

    //Number of axis in the motion
    static unsigned char movement_size;

    //StepperControl Functions : direction setting, motion, and endstop reading
    //static void (**dir_set_functions)(bool dir);

protected:
    /*
     * Method to verify your implementation of process_position : it will calculate position iteratively and send_packet the position.
     * Requires min, max and increment to be correctly set up, and takes steps/mm for 2 axis in argument
     */
    static void checkPositionByStep(float *offsets, unsigned int d);

    /*
     * Method to verify your implementation of getPositions  : it will calculate position iteratively and send_packet the position.
     * takes steps/mm for 2 axis in argument
     */
    static  void checkPosition(float *offsets, unsigned int d);



private:

    //Speed coefficients //TODO PRE PROCESS
    static float *const speed_coeffs;//TODO MOTION_RESERVED

private :


    //distance computing for N size vectors
    static unsigned char get_distance(long *dest, long *pos);

    //Speed coefficients setting
    static void set_speed_coefficients();


private :



    //method to compute the increment if it is not provided.
    static float extract_increment(float point, const float end, float increment, const unsigned char processing_steps,
                            const void (*get_position)(float, long *));

    //method to adjust the increment for step in the increment extraction process
    static float adjust_increment(const float point, float increment, const unsigned char processing_steps,
                                  const void (*get_position)(float, long *));


private :


    //Set the last elementary distances, for the last elementary move during motion

    //Speed processing method for N Axis non linear motions.
    static void process_speed();

    //Method to step and delay the correct time
    static void step(unsigned char sig);

    static void set_final_positions(const long *final_positions);


    static void fill_processors(void (*init_f)(), unsigned char (*position_f)(unsigned char *));

    static void set_speed_parameters_enqueue();

public:

    static void initialise(unsigned char *move_axis_t, const unsigned char size);

//Position process fields
    static float *const steps;

    static void set_initial_positions(const float *initial_positions);

    unsigned char
    get_movement_distances(float point_0, float point_1, void (*get_relative_position)(float, long *),
                           unsigned char *distances);

    void set_motion_data(const float min, const float max, const float step, void (*get_relative_position)(float, long *));

    void set_speed_parameters_enqueue(unsigned char processing_steps);
};

#endif //CODE_NONTRIVIALMOTION_H


#endif