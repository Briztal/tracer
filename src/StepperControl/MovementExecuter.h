/*
  MovementExecuter.h - Part of TRACER

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


#include "../config.h"

#ifdef ENABLE_STEPPER_CONTROL

#ifndef TRACER_MOVEMENTEXECUTER_H
#define TRACER_MOVEMENTEXECUTER_H

#include "../hardware_language_abstraction.h"
#include "motion_data.h"
#include "../DataStructures/Queue.h"

class MovementExecuter {


    static Queue<unsigned char> movement_queue;

    /*
     * The methods to start and stop the Movement Executer
     */

public :

    static void start();

    static void stop();

    /*
    * The three interrupt functions :
    *  - wait_for_movement waits till it can setup a movement, then sets trace as interrupt routine;
    *  - trace plans the current sub_movement, while periodically stepping and waiting,
    *      and then set finish_sub_movement as interrupt routine;
    *  - finish_sub_movement makes all remaining tics and then sets prepare_next_sub_movement as interrupt routine,
    *      or process_next_move is the current movement is done.
    */



    static void process_next_move();

    static void enqueue_trajectory_movement();

    static void enqueue_homing_movement();

private :

        static bool trajectory_move;
};


#endif //TRACER_MOTIONEXECUTER_H


#endif