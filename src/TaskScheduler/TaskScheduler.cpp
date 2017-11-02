/*
  TaskScheduler.cpp - Part of TRACER

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
PONEY
*/

#include <Sensors/Thermistors/Thermistors.h>
#include "../config.h"

#ifdef ENABLE_STEPPER_CONTROL

#include "TaskScheduler.h"
#include "../interface.h"
#include "EEPROM/EEPROMStorage.h"
#include "../StepperControl/StepperController.h"



/*
 * init : this function initialises the code.
 *
 *  It starts by initialising the hardware_language_abstraction layer, and then initialises all enabled interfaces.
 *
 *  Finally, it initialises the stepper control module.
 *
 */

void TaskScheduler::init() {

    hl_init();

    initialise_interfaces();

    Thermistors::init();

#ifdef ENABLE_STEPPER_CONTROL
    EEPROMStorage::init();
    StepperController::init();

#endif

}


//---------------------------------------------------Task Management----------------------------------------------------


/*
 * spaces : this function returns the number of spaces available in the task pool.
 *
 */

const uint8_t TaskScheduler::spaces() {
    return pool_task_spaces;
}


/*
 * add_task : this function adds a task in the taks pool.
 *
 */

void TaskScheduler::add_task(task_t task) {

    //Insert the task only if spaces are available
    if (pool_task_spaces) {

        //copy the task
        task_pool[pool_task_nb] = task;

        //Increase the number of task in the queue
        pool_task_nb++;

        //Decrease the number of spaces in the queue
        pool_task_spaces--;

    }
}


/*
 * add_procedure : this function adds a task that takes no arguments.
 *
 */

uint8_t TaskScheduler::add_procedure(task_state_t (*f)(void *), uint8_t type) {

    //Insert the task only if spaces are available
    if (pool_task_spaces) {

        //copy the task
        task_pool[pool_task_nb].task = f;
        task_pool[pool_task_nb].args = 0;
        task_pool[pool_task_nb].type = type;

        //Increase the number of task in the queue
        pool_task_nb++;

        //Decrease the number of spaces in the queue
        pool_task_spaces--;

    }

    return pool_task_spaces;
}


/*
 * add_procedure : this function adds a task that takes no arguments, to process asap.
 *
 */

uint8_t TaskScheduler::add_prioritary_procedure(task_state_t (*f)(void *)) {

    //Insert the task only if spaces are available
    if (pool_task_spaces) {

        //copy the task
        task_pool[pool_task_nb].task = f;
        task_pool[pool_task_nb].args = 0;
        task_pool[pool_task_nb].type = 255;

        //Increase the number of task in the queue
        pool_task_nb++;

        //Decrease the number of spaces in the queue
        pool_task_spaces--;

    }

    return pool_task_spaces;
}

//---------------------------------------------------Task Execution----------------------------------------------------

/*
 * run : this function executes all available tasks in the queue, while maintaining the order for non-non-sequential tasks;
 *
 *  It starts by reading interfaces, and filling the task pool with incoming tasks;
 *
 *  Then it executes every task it can (while maintaining the sequential constraints in non-non-sequential tasks) in the
 *      task pool;
 *
 *  Finally, it executes every task it can in task sequences.
 *
 */

void TaskScheduler::run() {

    //Add as much tasks as possible in the pool;
    read_interfaces();//TODO WATCH FOR SPACES

    //Process non-sequential tasks in priority
    process_task_pool();

    //Process tasks sequences after
    process_task_sequences();

}


/*
 * process_task_pool : this function executes every non-sequential task it can in the task pool,
 *  and tries to dispatch others.
 *
 *  As non-non-sequential tasks have a sequential constraint, it must tak several precautions.
 *
 */

void TaskScheduler::process_task_pool() {

    //Reset the first-task flags
    for (uint8_t i = 0; i < NB_TASK_SEQUENCES; i++) {
        dispatch_enabled[i] = true;
    }

    //Initialise an insertion index;
    uint8_t insert_index = 0;

    //A boolean to enable the task shift : disabled at the beginning, as no task is processed.
    bool shift_enabled = false;

    //Run through every task in the pool
    for (uint8_t task_index = 0; task_index < pool_task_nb; task_index++) {

        //Fist, cache the task pointer and the task's type.
        task_t *task = task_pool + task_index;
        uint8_t type = task->type;

        //If it is a non-sequential task, then try to process it.
        if (type == 255) {

            /* If the task fails to execute (must be called later), then shift it at the insertion position.
             * This removes empty spaces in the pool, and saved the order.*/
            if (!process_task(task)) {

                //Shift the task to the insertion position
                insert_index = shift(shift_enabled, task, insert_index);

            } else {

                //If the task succeeds, it leaves an empty space in the pool. Enable the shift.
                shift_enabled = true;
            }

        } else {

            //If the task is not a non-sequential task, then a dispatch in the appropriate sequence will be attempted.
            Queue<task_t> *sequence = task_sequences[type];

            /* If the current task is the first task of this type encountered during this process and
                the sequence is not full anymore, re-insertion is allowed */
            if (dispatch_enabled[type]) {

                if (sequence->available_spaces()) {

                    //Dispatch
                    *(sequence->get_input_ptr()) = *task;

                    sequence->enqueue();

                    //Enable the shifting, as a space is now present in the pool.
                    shift_enabled = true;

                    //DO NOT MOVE THE INSERTION INDEX, AS THE TASK IS NOT IN THE POOL ANYMORE


                } else {

                    //If No spaces are still available, disable the dispatch
                    dispatch_enabled[type] = false;

                    //Shift the task to the insertion position
                    insert_index = shift(shift_enabled, task, insert_index);

                }

            } else {

                //If the dispatch is disabled, shift the task to the insertion position
                insert_index = shift(shift_enabled, task, insert_index);

            }

        }

    }

    //Update the number of task, in the queue, that is now equal to the insertion index.
    pool_task_nb = insert_index;

    pool_task_spaces = (uint8_t) TASK_POOL_SIZE - pool_task_nb;

}


/*
 * shift : this function shifts a task to the less greater position possible in the task pool.
 *
 */
uint8_t TaskScheduler::shift(boolean shift_enabled, task_t *task, uint8_t insert_index) {

    //Shift only if the shifting is enabled.
    if (shift_enabled) {

        //Shift the task to the insertion position.
        task_pool[insert_index] = *task;

    }

    //Return the modified insertion index.
    return ++insert_index;

}


/*
 * process_task_sequences : this function processes task sequences.
 *
 *  It executes one task for each sequence, until no tasks are executable anymore.
 *
 */

void TaskScheduler::process_task_sequences() {

    bool process[NB_TASK_SEQUENCES];
    for (uint8_t i = 0; i < NB_TASK_SEQUENCES; i++) {
        process[NB_TASK_SEQUENCES] = true;
    }

    bool keep_processing = true;

    while (keep_processing) {

        keep_processing = false;

        for (uint8_t sequence_id = 0; sequence_id < NB_TASK_SEQUENCES; sequence_id++) {

            //Cache the task sequence;
            Queue<task_t> *sequence = task_sequences[sequence_id];

            //If the sequence still has elements to process  the output task is correctly processed;
            if (process[sequence_id] && (sequence->available_elements()) && process_task(sequence->read_output())) {

                //Go to the other task
                sequence->discard();

                //Enable the next processing
                keep_processing = true;

            } else {

                //Disable this sequence's processing
                process[sequence_id] = false;

            }

        }

    }

}



/*
 * process_task_sequences_singular : this function processes task sequences.
 *
 *  It focuses on each sequence consecutively : it executes all possible tasks on one sequence, and then focuses
 *      on another.
 *
 */

void TaskScheduler::process_task_sequences_singular() {

    for (uint8_t sequence_id = 0; sequence_id < NB_TASK_SEQUENCES; sequence_id++) {

        //Cache the task sequence;
        Queue<task_t> *sequence = task_sequences[sequence_id];

        while (sequence->available_elements()) {

            //if the output task is correctly processed :
            if (process_task(sequence->read_output())) {

                //Go to the other task
                sequence->discard();

            } else {

                /* If the output task fails to execute (must be called later), then stop the process for the sequence,
                 * as the execution order must be respected */

                break;
            }
        }
    }
}


/*
 * process_task : this function processes a task, and returns the result (it's a simple alias, for comprehensivity.
 *
 */

bool TaskScheduler::process_task(task_t *task) {

    //call the function of the task by pointer, and provide the arguments of the task. return false if the task must be reprogrammed
    return ((*(task->task))(task->args) != reprogram) ;

}



//-----------------------------------------Static declarations and definitions------------------------------------------

Queue<task_t> **define_task_queue(Queue<task_t> **ptr) {

//task sequences definition
#define TASK_SEQUENCE(i, size) ptr[i] = new Queue<task_t>(size);

#include <config.h>

#undef TASK_SEQUENCE

    return ptr;
}

#define m TaskScheduler

//task pool definition
task_t t_tskpl[TASK_POOL_SIZE];
task_t *const m::task_pool = t_tskpl;

//The number of tasks stored in the task pool
uint8_t m::pool_task_nb = 0;

//The number of tasks stored in the task pool
uint8_t m::pool_task_spaces = TASK_POOL_SIZE;


//Sequences lock counters definition
bool t_ftflg[NB_TASK_SEQUENCES]{0};
bool *const m::dispatch_enabled = t_ftflg;

//task sequences declaration
Queue<task_t> *t_tsks[NB_TASK_SEQUENCES];
Queue<task_t> **const m::task_sequences = define_task_queue(t_tsks);

#endif