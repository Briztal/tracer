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

#include "TaskScheduler.h"

#include <Kernel/Kernel.h>

#include <Kernel/Interaction/Interaction.h>

#include <Project/MachineController.h>//TODO REMOVE

#include <StepperControl/TrajectoryTracer.h>//TODO REMOVE

#include "Config/kernel_config.h"


namespace TaskScheduler {

    //------------------------------------------- Task scheduling - execution ------------------------------------------

    //Process a particular task;
    bool execute_task(task_t *task);


    //------------------------------------------ Pool and Sequences processing -----------------------------------------

    //Execute every possible task of the task pool;
    void process_task_pool();

    //Shift a task at the insertion index;
    uint8_t shift(bool shift_enabled, task_t *task, uint8_t insert_index);

    //The primary task pool;
    task_t task_pool[TASK_POOL_SIZE];

    //The number of tasks stored in the task pool;
    uint8_t pool_task_nb = 0;

    //The number of tasks stored in the task pool;
    uint8_t pool_task_spaces = TASK_POOL_SIZE;

    //Temp var for tests//TODO REMOVE
    bool flood_enabled;

}


//------------------------------------------------- Init ---------------------------------------------------

/*
 * reset : this function initialises the scheduler in a safe state;
 */

void TaskScheduler::initialise_data() {

    //Reset the task pool, by nullifying the number of tasks stored in it, and is number of spaces;
    pool_task_nb = 0;
    pool_task_spaces = TASK_POOL_SIZE;

    //Reset the flood flag; //TODO REMOVE;
    flood_enabled = false;

}

//---------------------------------------------------Task Management----------------------------------------------------

/*
 * schedule_task : this function adds the task, if it is possible, in :
 *  - the task pool if its type is 255;
 *  - a task sequence if its type corresponds to a task sequence.
 *
 *  WARNING :
 *
 *  I REPEAT, {\Color(red) \textit{\textbf{WARNING : }}}
 *      (If you got that one, you use latex, and you're a good person ! Thanks for reading my comments by the way ;-) )
 *
 *  If the task cannot be scheduled, (no space available, or bad sequence type), arguments_p are freed automatically.
 */

bool TaskScheduler::schedule_task(task_t *task) {

    if (pool_task_spaces) {

        //copy the task;
        task_pool[pool_task_nb] = *task;

        //Increase the number of task in the pool;
        pool_task_nb++;

        //Decrease the number of data_spaces in the pool;
        pool_task_spaces--;

        //Succeed;
        return true;

    }

    //If the scheduling failed, automatically free arguments_p, to avoid memory leak;
    free(task->dynamic_args);

    //TODO PROPER CONTAINERS

    //Fail;
    return false;
}


/*
 * schedule_task : this function creates a task and adds it to the task pool.
 *
 *  It takes in data all parameters required to build a task, namely :
 *      - function : the function to parse;
 *      - dynamic_args : a void *, addressing the first byte of the data. Those may be dynamic or static.
 *      - auto_free : set if dynamic_args must be freed automatically by the scheduler (if they are on the heap, for example).
 */

bool TaskScheduler::schedule_task(task_state_t (*f)(void *), void *args) {

    //Create a task to contain the provided data;
    task_t task = task_t();

    //Set the function to execute;
    task.task = f;

    //Set the argument_t pointer;
    task.dynamic_args = args;

    //Get the communication pipe that the task will use;
    task.log_pipe = Interaction::getCommunicationPipe();

    //Call the scheduling function and return whether the task was successfully scheduled;
    return schedule_task(&task);

}


/*
 * nb_spaces : this function returns the number of nb_spaces available in the task pool.
 */

const uint8_t TaskScheduler::available_spaces() {

    //return the number of nb_spaces of the pool;
    return pool_task_spaces;

}


/*
 * verify_schedulability : this function verifies than nb_tasks can be scheduled.
 *
 *  To do this, it verifies :
 *      - that the task sequence (or task pool for a 255-type task) is unlocked
 *      - that the concerned task container can effectively contain the required number of tasks.
 */

bool TaskScheduler::verify_schedulability(uint8_t nb_tasks) {


    //If the sequence (or the task pool, depending on the type) contains less than nb_tasks nb_spaces, fail;
    return pool_task_spaces >= nb_tasks;


    //Succeed!

}


//---------------------------------------------------Task Flood----------------------------------------------------


uint8_t temp_xxx = 0;

uint8_t temp_yyy = 0;

uint8_t temp_zzz = 0;

void TaskScheduler::flood() {

    //TODO REMOVE
    if (flood_enabled) {

        while (TrajectoryTracer::enqueue_authorised()) {

            MachineController::movement_state_t state = MachineController::movement_state_t();

            state.x_flag = true;
            state.y_flag = true;
            state.z_flag = true;

            state.x = temp_xxx;
            state.y = temp_yyy;
            state.z = temp_zzz;

            temp_xxx += 10;

            if (temp_xxx == 150) {
                temp_xxx = 0;
                temp_yyy += 10;
            }

            if (temp_yyy == 150) {
                temp_yyy = 0;
                temp_zzz += 10;
            }

            if (temp_zzz == 300) {
                temp_zzz = 0;
            }

            //Schedule a line to the specified coordinates
            MachineController::line(state);

        }

        flood_enabled = false;

    }


}

//---------------------------------------------------Task Execution----------------------------------------------------



/*
 * clear : this function deletes all programmed tasks.
 *
 */

void TaskScheduler::clear() {
    //TODO RMEOVE
    flood_enabled = false;

    //Empty the task pool : simply reset size indicators.

    //The number of tasks stored in the task pool;
    pool_task_nb = 0;

    //The number of tasks stored in the task pool;
    pool_task_spaces = TASK_POOL_SIZE;

}

/*
 * process_task_pool : this function executes every non-sequential task it can in the task pool,
 *  and tries to dispatch others.
 *
 *  As non-non-sequential tasks have a sequential constraint, it must tak several precautions.
 */

void TaskScheduler::process_task_pool() {

    //Initialise an insertion index;
    uint8_t insert_index = 0;

    //A boolean to enable the task shift : disabled at the beginning, as no task is processed.
    bool shift_enabled = false;

    //Run through every task in the pool
    for (uint8_t task_index = 0; task_index < pool_task_nb; task_index++) {

        //Fist, cache the task pointer and the task's type.
        task_t *task = task_pool + task_index;

        /* If the task fails to parse (must be called later), then shift it at the insertion position.
         * This removes empty data_spaces in the pool, and saved the order.*/
        if (!execute_task(task)) {

            //Shift the task to the insertion position
            insert_index = shift(shift_enabled, task, insert_index);

        } else {

            //If the task succeeds, it leaves an empty space in the pool. Enable the shift.
            shift_enabled = true;
        }

    }

    //Update the number of task, in the pool, that is now equal to the insertion index.
    pool_task_nb = insert_index;

    pool_task_spaces = (uint8_t) TASK_POOL_SIZE - pool_task_nb;

}


/*
 * shift : this function shifts a task to the less greater position possible in the task pool.
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
 * execute_task : this function processes a task, and returns the result (it's a simple alias, for comprehensibility.)
 */

bool TaskScheduler::execute_task(task_t *task) {

    //Initialise the state as complete, in case of null task.
    task_state_t state = complete;

    //Execute if the task is not null.
    if (task->task != nullptr) {

        //Set the task's log pipe;
        Interaction::setCommunicationPipe(*task->log_pipe);

        //call the function of the task by pointer, and provide the data of the task.
        state = (*(task_function_t) (task->task))(task->dynamic_args);

    }

    //If the task must be reprogrammed,
    if (state != reprogram) {

        /*
         * The line below deletes a void *, and so will generate the following warning :
         *
         *      warning: deleting 'void*' is undefined [-Wdelete-incomplete]
         *
         * As dynamic arguments_p are dynamically allocated, this operation is secure.
         */

#pragma GCC diagnostic ignored "-Wdelete-incomplete"

//Free the memory occupied by the task.
        delete (task->dynamic_args);

#pragma GCC diagnostic pop

        //Succeed.
        return true;

    }

    //Fail if the task must be reprogrammed.
    return false;

}
