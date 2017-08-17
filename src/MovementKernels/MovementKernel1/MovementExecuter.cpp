/*
  MovementExecuter.cpp - Part of TRACER

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

#include <interface.h>
#include <DataStructures/Queue.h>

#include "MovementExecuter.h"
#include "TrajectoryExecuter.h"
#include "LinearMovement/HomingMovement.h"
#include "SpeedManager.h"
/*
 * TODOs for a correct motion setup :
 *      set end distances
 *      set first elementary_distances, calling MovementExecuter::fill_movement_data(true,  ... );
 *      set last  elementary_distances, calling MovementExecuter::push_first_sub_movement(false, ... );w
 *      set speed data, calling MovementScheduler::pre_set_speed_axis(...);
 *      set processing functions, calling MovementExecuter::fill_processors(...);
 *      call MovementExecuter::enqueue_movement_data
 *
 */


//------------------------------------------------movement_queue_management---------------------------------------------

/*
 * push_first_sub_movement : this function hashes the first sub_movement of the future motion, and adds the resulting data
 *      to the future movement data
 */

void MovementExecuter::start() {
    setup_stepper_interrupt(process_next_move, 30000);
}

void MovementExecuter::enqueue_trajectory_movement() {
    movement_queue.push_object(0);
}

void MovementExecuter::enqueue_homing_movement() {
    movement_queue.push_object(1);
}

/*
 * The movement initialisation function :
 *  - if possible, starts a movement;
 *  - if not, returns, and will re-check later (interrupt)
 */
void MovementExecuter::process_next_move() {

    disable_stepper_interrupt();

    if (movement_queue.available_elements()) {
        unsigned char c = movement_queue.pull();

        switch (c) {
            /*
             * First case : following a trajectory.
             *  All parameters have been enqueued in the trajectory executer queue.
             *  All we need to do is to start the tracing procedure
             */
            case 0 :
                trajectory_move = true;

                TrajectoryExecuter::start();
                return;
                /*
                 * Second case : a homing movement.
                 *  The movement does not require any pre-computed parameter.
                 *  All we need to do is to start the homing procedure
                 */
            case 1 :
                //HomingMovement::start();
            default:
                break;

        }
    } else {

        if (trajectory_move) {
            //Disable the jerk checking, if the movement queue is empty.
            SpeedPlanner::last_regulation_speed = 0;
        }

        trajectory_move = false;

    }

    enable_stepper_interrupt();

}


bool MovementExecuter::trajectory_move = false;
Queue<unsigned char> MovementExecuter::movement_queue(MOTION_DATA_QUEUE_SIZE);

#endif