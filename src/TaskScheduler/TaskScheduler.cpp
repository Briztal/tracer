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

#include <EEPROM/EEPROMStorage.h>
#include <StepperControl/StepperController.h>
#include <ControlLoops/ControlLoops.h>


/*
 * init : this function initialises the code.
 *
 *  It starts by initialising the hardware_language_abstraction layer, and then initialises all enabled interfaces.
 *
 *  Finally, it initialises the stepper control module.
 */

void TaskScheduler::init() {

    //Initialise the hardware and the framework
    hl_init();

    //Initialise all enabled interfaces
    initialise_interfaces();

    //Initialise Thermistors
    Thermistors::init();

    //Initialise the EEPROM data
    EEPROMStorage::init();


    //Initialise the StepperController module only if it is enabled
#ifdef ENABLE_STEPPER_CONTROL

    StepperController::init();

#endif

    //TODO SEPARATE THIS FROM THE SCHEDULER

    //Enable the
    ControlLoops::enable_temperature();


}




//--------------------------------------------------Type Verification---------------------------------------------------

bool TaskScheduler::check_sequence_type(uint8_t type) {

    if (type == 255) {
        return true;
    } else {
        return (type < NB_TASK_SEQUENCES);
    }
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
 *  If the task cannot be scheduled, (no space available, or bad sequence type), args are freed automatically.
 */

bool TaskScheduler::schedule_task(task_t *task) {

    uint8_t type = task->type;

    if ((type == 255) && (pool_task_spaces)) {

        //copy the task;
        task_pool[pool_task_nb] = *task;

        //Increase the number of task in the pool;
        pool_task_nb++;

        //Decrease the number of data_spaces in the pool;
        pool_task_spaces--;

        //Succeed;
        return true;

    }

    CI::echo("available : " + String(task_sequences[type]->available_spaces()));

    //If the task's type corresponds to an existing sequence:
    if ((type < NB_TASK_SEQUENCES) && (task_sequences[type]->available_spaces())) {

        //Copy the task in the sequence;
        *task_sequences[type]->get_input_ptr() = *task;

        //Validate the copy.
        task_sequences[type]->enqueue();

        //Succeed;
        return true;

    }

    //If the scheduling failed, automatically free args, to avoid memory leak;
    free(task->dynamic_args);


    //Fail;
    return false;
}


/*
 * schedule_task : this function creates a task and adds it to the task pool.
 *
 *  It takes in arguments all parameters required to build a task, namely :
 *      - f : the function to schedule_command;
 *      - dynamic_args : a void *, addressing the first byte of the arguments. Those may be dynamic or static.
 *      - auto_free : set if dynamic_args must be freed automatically by the scheduler (if they are on the heap, for example).
 *      - type : the type of the task.
 */

bool TaskScheduler::schedule_task(uint8_t type, task_state_t (*f)(void *), void *args) {

    //Create a task to contain the provided data;
    task_t task = task_t();

    //Set the function to execute;
    task.task = f;

    //Set the argument_t pointer;
    task.dynamic_args = args;

    //Set the type;
    task.type = type;

    //Call the scheduling function and return whether the task was successfully scheduled;
    return schedule_task(&task);
}


/*
 * spaces : this function returns the number of spaces available in the task pool.
 */

const uint8_t TaskScheduler::available_spaces(uint8_t type) {

    //If the type is not allocated, return zero
    if (!check_sequence_type(type))
        return 0;

    if (type == 255) {

        //If the type corresponds to the task pool : return the number of spaces of the pool
        return pool_task_spaces;

    } else {

        //If the type corresponds to a sequence, return the number of spaces in the concerned sequence.
        return task_sequences[type]->available_spaces();

    }
}


/*
 * lock_sequence : this function locks a specified sequence.
 */

void TaskScheduler::lock_sequence(uint8_t type) {

    //If the type is not allocated, do nothing
    if (!check_sequence_type(type))
        return;


    if (type != 255) {

        //Set the lock of the concerned sequence.
        queues_locked[type] = true;

    }

}


/*
 * is_sequence_locked : this function returns true if the specified sequence is locked, and false if not.
 */

bool TaskScheduler::is_sequence_locked(uint8_t type) {

    //If the type is not allocated, the queue is locked by default.
    if (!check_sequence_type(type))
        return true;

    if (type == 255) {

        return false;
    }

    return !queues_locked[type];
}


/*
 * verify_schedulability : this function verifies than nb_tasks of type task_type can be scheduled.
 *
 *  To do this, it verifies :
 *      - that the task sequence (or task pool for a 255-type task) is unlocked
 *      - that the concerned task container can effectively contain the required number of tasks.
 */

bool TaskScheduler::verify_schedulability(uint8_t task_type, uint8_t nb_tasks) {

    //If the sequence is locked, fail, no more tasks of this type are schedulable;
    if (TaskScheduler::is_sequence_locked(task_type)) {
        return false;
    } else {

        //If the sequence (or the task pool, depending on the type) contains less than nb_tasks spaces, fail;
        if (TaskScheduler::available_spaces(task_type) < nb_tasks) {
            return false;
        }
    }

    //Succeed!
    return true;

}


//---------------------------------------------------Task Execution----------------------------------------------------


/*
 * run : this function executes all available tasks in the pool, while maintaining the order for non-non-sequential tasks;
 *
 *  It starts by reading interfaces, and filling the task pool with incoming tasks;
 *
 *  Then it executes every task it can (while maintaining the sequential constraints in non-non-sequential tasks) in the
 *      task pool;
 *
 *  Finally, it executes every task it can in task sequences.
 */

uint8_t temp_xxx = 0;

uint8_t temp_yyy = 0;

uint8_t temp_zzz = 0;

void TaskScheduler::run() {

    while (true) {


        //Add as much tasks as possible in the pool;
        read_interfaces();//TODO WATCH FOR SPACES

        //Process non-sequential tasks in priority
        process_task_pool();

        //Process tasks sequences after
        process_task_sequences();

        /*
        if (data_spaces(0)) {

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
            MachineController::line_scheduled_0(state);

        }

        if (data_spaces(0)) {

            //Schedule an enable / disable of steppers.
            //MachineController::enable_steppers_scheduled_0(true);

        }

        CI::echo("SUUS");

        delay(300);


         */

    }
}


/*
 * process_task_pool : this function executes every non-sequential task it can in the task pool,
 *  and tries to dispatch others.
 *
 *  As non-non-sequential tasks have a sequential constraint, it must tak several precautions.
 */

void TaskScheduler::process_task_pool() {

    //Reset the first-task flags
    for (uint8_t i = 0; i < NB_TASK_SEQUENCES; i++) {
        queues_locked[i] = true;
    }

    //Initialise an insertion index;
    uint8_t insert_index = 0;

    //A boolean to enable the task shift : disabled at the beginning, as no task is processed.
    bool shift_enabled = false;

    //Run through every task in the pool
    for (uint8_t task_index = 0; task_index < pool_task_nb; task_index++) {

        //Fist, cache the task pointer and the task's type.
        task_t *task = task_pool + task_index;

        /* If the task fails to analyse_command (must be called later), then shift it at the insertion position.
         * This removes empty data_spaces in the pool, and saved the order.*/
        if (!process_task(task)) {

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
 * process_task_sequences : this function processes task sequences.
 *
 *  It executes one task for each sequence, until no tasks are executable anymore.
 */

void TaskScheduler::process_task_sequences() {

    //Declare an array of bool, that will enable the processing of the task queue they are indexed with.
    bool process[NB_TASK_SEQUENCES];

    //Fill array with true.
    for (uint8_t i = 0; i < NB_TASK_SEQUENCES; i++) {
        process[i] = true;
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

                /* If the output task fails to analyse_command (must be called later), then stop the process for the sequence,
                 * as the execution order must be respected */

                break;
            }
        }
    }
}


/*
 * process_task : this function processes a task, and returns the result (it's a simple alias, for comprehensibility.)
 */

bool TaskScheduler::process_task(task_t *task) {

    //Initialise the state as complete, in case of null task.
    task_state_t state = complete;

    //Execute if the task is not null.
    if (task->task != nullptr) {

        //call the function of the task by pointer, and provide the arguments of the task.
        state = (*(task->task))(task->dynamic_args);

    }

    //If the task must be reprogrammed,
    if (state != reprogram) {

        //Free the memory occupied by the task.
        delete (task->dynamic_args);

        //Succeed.
        return true;
    }

    //Fail if the task must be reprogrammed.
    return false;

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
bool *const m::queues_locked = t_ftflg;

//task sequences declaration
Queue<task_t> *t_tsks[NB_TASK_SEQUENCES];
Queue<task_t> **const m::task_sequences = define_task_queue(t_tsks);


#endif