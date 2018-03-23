/*
  TaskScheduler.h - Part of TRACER

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


#ifndef PROJECT_SYSTEM_H
#define PROJECT_SYSTEM_H

#include <Config/kernel_config.h>

#include <DataStructures/Containers/Queue.h>

#include <Kernel/Interaction/CommunicationPipe.h>

#include "_task_scheduler_data.h"

//All files that include this library may require the generation of a scheduler.
#include "scheduler_generation.h"

//The file scheduler_safe_use.txt contains documentation and help about the scheduler. I Hope it will help you.

class TaskScheduler {

    //-------------------------------------------- Parsing - execution cycle -------------------------------------------

public:

    //Initialise the scheduler in a safe state;
    static void initialise_data();


    //-------------------------------------------- Parsing - execution cycle -------------------------------------------

public:

    //Trigger a complete parsing - execution cycle;
    static void iterate();


    //-------------------------------------------------- State getters -------------------------------------------------

public:

    //Returns the number of type-[type] tasks that can be scheduled;
    static const uint8_t available_spaces(uint8_t type);

    //Lock the specified sequence;
    static void lock_sequence(uint8_t type);

    //get the state of a sequence;
    static bool is_sequence_locked(uint8_t type);

    //Verify that nb_tasks can be schedule;
    static bool verify_schedulability(uint8_t task_type, uint8_t nb_tasks);


private:

    //First tasks flags;
    static bool *const sequences_locked;


    //------------------------------------------- Task scheduling - execution ------------------------------------------

public:

    //Schedule a task;
    static bool schedule_task(task_t *task);

    //Build and schedule a task;
    static bool schedule_task(uint8_t type, task_state_t (*f)(void *), void *args);


private:

    //Process a particular task;
    static bool execute_task(task_t *task);

public:

    //Temp var for tests//TODO REMOVE
    static bool flood_enabled;


    //------------------------------------------ Pool and Sequences processing -----------------------------------------

public:

    //Clear the task pool and all task sequences;
    static void clear();


private:

    //Execute every possible task of the task pool;
    static void process_task_pool();

    //Execute every possible task on tasks sequences;
    static void process_task_sequences();

    //Shift a task at the insertion index;
    static uint8_t shift(bool shift_enabled, task_t *task, uint8_t insert_index);

    //Verify that a type exists
    static bool check_sequence_type(uint8_t type);


    //The primary task pool;
    static task_t *const task_pool;

    //The number of tasks stored in the task pool;
    static uint8_t pool_task_nb;

    //The number of tasks stored in the task pool;
    static uint8_t pool_task_spaces;

    //Task sequences;
    static Queue<task_t> **const task_sequences;

};


/*
 * LOCK_SEQUENCE : This macro shall be used in your tasks to lock a given task_sequence.
 */

#define LOCK_SEQUENCE(type) {TaskScheduler::lock_sequence(type);}


#endif //PROJECT_SYSTEM_H
