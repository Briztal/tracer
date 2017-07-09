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


#ifndef CODE_NONLINEARMOTIONN_H
#define CODE_NONLINEARMOTIONN_H

#include "../MotionScheduler.h"

/**
 * A more evolved Class for non trivial plannar motions, ie :
 * - indexed on one float variable;
 * - comprised in a plan containing two axis;
 */

class NonLinearMotionN {

    //-----------------------------------------virtual_methods----------------------------------------------------------

protected:

    //Method for motion parameters init :
    virtual void init_position_parameters() = 0;

    //Step by step processing for position
    virtual bool process_position() = 0;

    //One time processing for position
    virtual void get_position(float a, long *c) = 0;


    //-----------------------------------------protected_fields---------------------------------------------------------

protected:

    static unsigned char *const axis_copy_t;

    //Number of axis in the motion
    static unsigned char motion_size;

    //Curve variable parameters : beginning, last
    static float min, max;

    //Curve variable modified during motion
    static float alpha;

    //Relative position array
    static long *const pos_t;

    //First and last relative positions
    static long *const first_pos_t, *const last_pos_t;

    //First ans last absolutes positions
    static long *const first_positions, *const last_positions;

    //Position process fields
    static float *const steps;

    //StepperControl Functions : direction setting, motion, and endstop reading
    static void (**dir_set_functions)(bool dir);

    //boolean indicating if the increment variable is provided
    static bool increment_provided;


    //----------------------------------------protected_methods---------------------------------------------------------

protected:

    //Method to provide the increment
    static void provide_increment(float i);

    //Method to setup the linear move, going to the first position of the curve
    void setup_engagement_move();

    //Method to set the curve ending position, fur speed regulation
    void set_last_position();

    //Methods to update axis parameters (steps, accelerations etc...)
    void initialise_motion();

    //Method to begin the motion
    void move();

    /*
     * Method to verify your implementation of process_position : it will calculate position iteratively and send_packet the position.
     * Requires min, max and increment to be correctly set up, and takes steps/mm for 2 axis in argument
     */
    void checkPositionByStep(float *offsets, unsigned int d);

    /*
     * Method to verify your implementation of getPositions  : it will calculate position iteratively and send_packet the position.
     * takes steps/mm for 2 axis in argument
     */
    void checkPosition(float *offsets, unsigned int d);


    //------------------------------------------private_fields----------------------------------------------------------

private:

    //The curve variable increment across motion
    static float increment;

    //Speed coefficients //TODO PRE PROCESS
    static float *const speed_coeffs;

    //------------------------------------------private_methods---------------------------------------------------------

private :

    //------------------------------------------static

    //Signatures setting
    static void set_signatures();

    //distance computing for N size vectors
    static unsigned char get_distance(long *dest, long *pos);

    //Speed coefficients setting
    static void set_speed_coefficients();


private :

    //------------------------------------------non_static

    //method to set increment, count, alpha, and pos_t
    void initialise_motion_vars();

    //method to compute the increment if it is not provided.
    float extract_increment(float point, float end, float increment, unsigned int processing_steps);

    //method to adjust the increment for step in the increment extraction process
    float adjust_increment(float step, float point);


private :

    //------------------------------------------implemented methods from StepperControl

    //Set the last elementary distances, for the last elementary move during motion
    void set_end_distances();


    //Speed processing method for N Axis non linear motions.
    void process_speed();

    //Method to step and delay the correct time
    void step_and_delay(unsigned char sig);

    void draw();
};

#endif //CODE_NONTRIVIALMOTION_H
