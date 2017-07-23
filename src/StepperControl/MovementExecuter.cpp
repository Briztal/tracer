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

#include "../config.h"

#ifdef ENABLE_STEPPER_CONTROL

#include "MovementExecuter.h"
#include "SpeedManager.h"
#include "../Interfaces/CommandInterface.h"
#include "motion_data.h"
#include "StepperController.h"
#include "LinearMovement/LinearMovement.h"
#include "MovementScheduler.h"
#include "../Actions/ContinuousActions.h"
#include "TrajectoryExecuter.h"

/*
 * TODOs for a correct motion setup :
 *      set end distances
 *      set first elementary_distances, calling MovementExecuter::fill_movement_data(true,  ... );
 *      set last  elementary_distances, calling MovementExecuter::fill_movement_data(false, ... );w
 *      set speed data, calling MovementScheduler::pre_set_speed_axis(...);
 *      set processing functions, calling MovementExecuter::fill_processors(...);
 *      call MovementExecuter::enqueue_movement_data
 *
 */


//------------------------------------------------movement_queue_management---------------------------------------------

/*
 * fill_movement_data : this function hashes the first sub_movement of the future motion, and adds the resulting data
 *      to the future movement data
 */

void MovementExecuter::start() {
    init_stepper_interrupt();
    setup_stepper_interrupt(process_next_move, 30000);
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

        switch(c) {
            /*
             * First case : a trajectory.
             *  All parameters have been enqueued in the trajectory executer queue.
             *  All we need to do is to start the tracing procedure
             */
            case 0 :
            TrajectoryExecuter::start();
                break;
        }


//TODO NECESSARY ?
    enable_stepper_interrupt();

}


Queue<unsigned char> MovementExecuter::movement_queue(MOTION_DATA_QUEUE_SIZE);