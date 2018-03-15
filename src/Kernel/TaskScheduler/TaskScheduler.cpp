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

#include <Interaction/Interaction.h>

#include <Project/MachineController.h>//TODO REMOVE

#include <StepperControl/TrajectoryTracer.h>//TODO REMOVE



//-------------------------------------------- Const variable init --------------------------------------------

/*
 * The number of task sequences is a constant, directly determinable by counting the number of times TASK_SEQUENCES
 *  is written in the configuration file.
 *
 *  Instead of asking the user to provide it, what could lead to errors, we will use a compiler constant
 *      determined directly using macro;
 *
 */
#define TASK_SEQUENCE(...) 1 +

const uint8_t NB_TASK_SEQUENCES =

#include "Config/kernel_config.h"

        0;

#undef TASK_SEQUENCE


//------------------------------------------------- Init ---------------------------------------------------

/*
 * initialise_data : this function initialises the scheduler in a safe state;
 */

void TaskScheduler::initialise_data() {

    //Reset the task pool, by nullifying the number of tasks stored in it, and is number of spaces;
    pool_task_nb = 0;
    pool_task_spaces = TASK_POOL_SIZE;

    //Reset sequences lock counters;
    memset(sequences_locked, 0, NB_TASK_SEQUENCES * sizeof(bool));

    //Clear tasks sequences;
    for (uint8_t sequence = 0; sequence < NB_TASK_SEQUENCES; sequence++) {
        task_sequences[sequence]->clear();
    }

    //Reset the flood flag; //TODO REMOVE;
    flood_enabled = false;


    //Initialise the default log environment;
    default_log_function = (void (*)(Protocol *,const char *)) Interaction::get_default_log_function();

    //Initialise the default protocol;
    default_log_protocol = Interaction::get_default_protocol();

    //Reset the log environment;
    log_function = default_log_function;
    log_protocol = default_log_protocol;

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


    //If the task's type corresponds to an existing sequence:
    if ((type < NB_TASK_SEQUENCES) && (task_sequences[type]->available_spaces())) {

        //First, cache the address of the current task sequence;
        Queue<task_t> *queue = task_sequences[type];

        //Initialise a flag;
        bool queue_flag = false;

        //Copy the task in the sequence;
        queue->insert_object(task, &queue_flag);


        //Copy the task in the sequence;
        *task_sequences[type]->get_insertion_address(&queue_flag) = *task;

        //Integriy check
        if (!queue_flag) {

            //Log
            std_out("ERROR in Kernel::schedule_task : failed to copy the task in the queue.");

            //Emergency stop
            Kernel::emergency_stop();

        } else {

            //Succeed;
            return true;

        }


    }

    //If the scheduling failed, automatically free args, to avoid memory leak;
    free(task->dynamic_args);


    //Fail;
    return false;
}


/*
 * schedule_task : this function creates a task and adds it to the task pool.
 *
 *  It takes in content all parameters required to build a task, namely :
 *      - function : the function to parse;
 *      - dynamic_args : a void *, addressing the first byte of the content. Those may be dynamic or static.
 *      - auto_free : set if dynamic_args must be freed automatically by the scheduler (if they are on the heap, for example).
 *      - type : the type of the task.
 */

bool TaskScheduler::schedule_task(uint8_t type, task_state_t (*f)(void *), void *args,
                                  void (*log_function)(Protocol *, const char *), Protocol *protocol) {

    //Create a task to contain the provided data;
    task_t task = task_t();

    //Set the function to execute;
    task.task = f;

    //Set the argument_t pointer;
    task.dynamic_args = args;

    //Set the type;
    task.type = type;


    if (log_function && log_protocol) {

        //Set the log function;
        task.log_function = log_function;

        //Set the log protocol;
        task.log_protocol = protocol;

    } else {

        //Set the log function;
        task.log_function = default_log_function;

        //Set the log protocol;
        task.log_protocol = default_log_protocol;

    }

    //Call the scheduling function and return whether the task was successfully scheduled;
    return schedule_task(&task);
}


/*
 * nb_spaces : this function returns the number of nb_spaces available in the task pool.
 */

const uint8_t TaskScheduler::available_spaces(uint8_t type) {

    if (type == 255) {

        //If the type corresponds to the task pool : return the number of nb_spaces of the pool;
        return pool_task_spaces;

    } else {

        //If the type is not allocated, return zero;
        if (!check_sequence_type(type))
            return 0;

        //If the type corresponds to a sequence, return the number of nb_spaces in the concerned sequence;
        return task_sequences[type]->available_spaces();

    }
}


/*
 * lock_sequence : this function locks a specified sequence.
 */

void TaskScheduler::lock_sequence(uint8_t type) {

    //If the type is not allocated, do nothing;
    if (!check_sequence_type(type))
        return;

    if (type != 255) {

        //Set the lock of the concerned sequence;
        sequences_locked[type] = true;

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

    return !sequences_locked[type];
}


/*
 * verify_schedulability : this function verifies than nb_tasks of type task_type can be scheduled.
 *
 *  To do this, it verifies :
 *      - that the task sequence (or task pool for a 255-type task) is unlocked
 *      - that the concerned task container can effectively contain the required number of tasks.
 */

bool TaskScheduler::verify_schedulability(uint8_t task_type, uint8_t nb_tasks) {

    std_out(string("NB TASK SEQUENCES ")+(uint8_t)NB_TASK_SEQUENCES);

    //If the sequence is locked, fail, no more tasks of this type are schedulable;
    if (TaskScheduler::is_sequence_locked(task_type)) {
        return false;
    } else {

        //If the sequence (or the task pool, depending on the type) contains less than nb_tasks nb_spaces, fail;
        if (TaskScheduler::available_spaces(task_type) < nb_tasks) {
            return false;
        }
    }

    //Succeed!
    return true;

}


//---------------------------------------------------Task Execution----------------------------------------------------


/*
 * iterate : this function executes all available tasks in the pool, while maintaining the order for non-non-sequential tasks;
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

void TaskScheduler::iterate() {

    //Process non-sequential tasks in priority
    process_task_pool();

    //Process tasks sequences after
    process_task_sequences();


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


    //Clear all tasks sequences.

    //For each task sequence :
    for (uint8_t i = 0; i < NB_TASK_SEQUENCES; i++) {

        //Clear the sequence;
        task_sequences[i]->clear();

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
        sequences_locked[i] = true;
    }

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

            //If the sequence can be processed :
            if (process[sequence_id]) {

                //If the sequence still has tasks to process
                if (sequence->available_objects()) {

                    //Declare a flag
                    bool queue_flag = false;

                    //Get the task to execute
                    task_t *task = sequence->get_reading_address(&queue_flag);

                    //Integrity check
                    if (!queue_flag) {

                        //Log
                        std_out("ERROR in Kernel::process_task_sequences : "
                                         "the reading element is not allocated.");

                        //Emergency stop
                        Kernel::emergency_stop();

                        return;

                    }

                    //If the executed task must be reprogrammed
                    if (!execute_task(task)) {

                        //Disable this sequence's processing
                        process[sequence_id] = false;


                    } else {
                        //If the task was processed :

                        //Reset the flag
                        queue_flag = false;

                        //Go to the other task
                        sequence->discard(&queue_flag);

                        //Integrity check
                        if (!queue_flag) {

                            //Log
                            std_out("ERROR in Kernel::process_task_sequences : "
                                             "the discarded element was not allocated.");

                            //Emergency stop
                            Kernel::emergency_stop();

                            return;

                        }

                        //Enable the next processing
                        keep_processing = true;

                    }

                } else {

                    //Disable this sequence's processing
                    process[sequence_id] = false;

                }

            }

        }

    }

}


/*
 * execute_task : this function processes a task, and returns the result (it's a simple alias, for comprehensibility.)
 */

bool TaskScheduler::execute_task(task_t *task) {

    //Initialise the state as complete, in case of null task.
    task_state_t state = complete;

    //Execute if the task is not null.
    if (task->task != nullptr) {

        //Save the log function if it is not null;
        if (task->log_function)
            log_function = task->log_function;

        //Save the protocol if it is not null;
        if (task->log_protocol)
            log_protocol = task->log_protocol;

        //call the function of the task by pointer, and provide the content of the task.
        state = (*(task->task))(task->dynamic_args);

    }

    //If the task must be reprogrammed,
    if (state != reprogram) {

        /*
         * The line below deletes a void *, and so will generate the following warning :
         *
         *      warning: deleting 'void*' is undefined [-Wdelete-incomplete]
         *
         * As dynamic args are dynamically allocated, this operation is secure.
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


//-------------------------------------------------------- Log ---------------------------------------------------------


void TaskScheduler::log(tstring &message) {

    //If both log function and log log_protocol have been assigned :
    if (log_function && log_protocol) {

        //Generate the final message pointer;
        const char *ptr = message.data();

        //Send the message with the correct log_protocol.
        (*log_function)(log_protocol, ptr);

    }

}

void TaskScheduler::log(tstring &&message) {

    //If both log function and log log_protocol have been assigned :
    if (log_function && log_protocol) {

        //Generate the final message pointer;
        const char *ptr = message.data();

        //Send the message with the correct log_protocol.
        (*log_function)(log_protocol, ptr);

    }

}

void TaskScheduler::log(const char *message) {

    //If both log function and log log_protocol have been assigned :
    if (log_function && log_protocol) {


        //Send the message with the correct log_protocol.
        (*log_function)(log_protocol, message);

    }

}

//----------------------------------------- Tasks queues creation ------------------------------------------

/*
 * instantiate_task_queues : this function will instantiate task queues in the provided array.
 */
Queue<task_t> **instantiate_task_queues(Queue<task_t> **ptr) {

//task sequences definition
#define TASK_SEQUENCE(i, size) ptr[i] = new Queue<task_t>(size);

#include <Config/kernel_config.h>

#undef TASK_SEQUENCE

    return ptr;
}


//-----------------------------------------Static declarations and definitions------------------------------------------

#define m TaskScheduler

//task pool definition
task_t t_tskpl[TASK_POOL_SIZE];
task_t *const m::task_pool = t_tskpl;

//The number of tasks stored in the task pool
uint8_t m::pool_task_nb = 0;

//The number of tasks stored in the task pool
uint8_t m::pool_task_spaces = TASK_POOL_SIZE;

//Sequences lock counters definition
bool t_ftflg[NB_TASK_SEQUENCES]{false};
bool *const m::sequences_locked = t_ftflg;

//task sequences declaration
Queue<task_t> *t_tsks[NB_TASK_SEQUENCES];
Queue<task_t> **const m::task_sequences = instantiate_task_queues(t_tsks);

bool m::flood_enabled = false;

//The encoding function;
void (*m::log_function)(Protocol *, const char * message) = nullptr;

//The communication log_protocol;
Protocol *m::log_protocol = nullptr;

//The default log function
void (*m::default_log_function)(Protocol *, const char * message);

//The default log_protocol;
Protocol *m::default_log_protocol;

