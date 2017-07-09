/*
  NonLinearMotionNTemplate.cpp - Part of TRACER

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
#include "NonLinearMotionNTemplate.h"


void NonLinearMotionNTemplate::move() {//TODO VIRER CETTE MERDE
    NonLinearMotionN::move();
}

/*
 * The motion preparation method : you must call it manually before calling motionN.move();
 * It is meant to setup the correct coordinates for the approaching motion.
 * In this method, you must initialise motion size, and absolute and relatives first position.
 */
bool NonLinearMotionNTemplate::prepare_motion() {
    /* All coordinates are expressed in the tic system. To manipulate real coordinates, you must use steps, initialised
     * 4 lines below
     */


    //TODO INITIALISE dimension

    //initialise machine parameters according to dimension and axis_t
    NonLinearMotionN::initialise_motion();
    //After this line, steps is correctly updated, you can use it as you wish

    //TODO INITIALISE first_pos_t

    //TODO INITIALISE first_positions

    //Modify destinations so that the engagement motion can happen
    setup_engagement_move();

    return true;

}

/*
 * Position parameters initialisation method : it is called in method move from NonLinearMotionN
 * In this function, you must initialise absolute and relative last positions, min and max, and eventually provide
 *  an increment.
 */
void NonLinearMotionNTemplate::init_position_parameters() {
    /* All coordinates are expressed in the tic system. To manipulate real coordinates, you must use steps, initialised
     * in prepare_motion
     */

    //TODO initialise min and max, the curve variable beginning and ending values.

    //TODO IF NECESSARY CALL provide_increment(your_increment)

    //TODO INITIALISE last_pos_t

    //TODO INITIALISE last_positions

    //set the motions last position, for speed regulation purposes
    set_last_position();

}

/*
 * One time position processing method : called in the increment extraction process
 * Used to compute the position corresponding to alpha. Position goes to *pos
 */
void NonLinearMotionNTemplate::get_position(float alpha, long *pos) {
    //TODO COMPUTE THE POSITION
}

/*
 * Step by step position processing function : called during motion to compute the next destination.
 *
 */
bool NonLinearMotionNTemplate::process_position() {
    /*
     * (*p).step contains the address of a label inside this function.
     * At each call, a goto redirects to the correct step, and (*p).step is set to the next step's address
     */

    /*Initialisation : if procedure not started (*p).step is initialised to the second step
     * if not, goto the correct step
     */

    /*
    if (p->started) {
        goto
        *(*p).step;
    }
    p->started = true;
*/
    //enqueue the first processing step

    /*
     * Each label below represents an intermediary step in position processing.
     * The dilemma is to divide the process sufficiently, for speed purposes, but no too much.
     * If the process is too long, the positions spacing will be large
     *  and the curve approximation will not be very good.
     */

    /*
    c1 :
    (*p).step = &&c2;
    return false;

    c2 :
    (*p).step = &&c3;
    //TODO Add a label, a step setting, and a return false for each processing step
    return false;
    c3:
    //The last processing step is followed by a return true.
    return true;

     */

}
