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

//TODO TESTER L'ENVOI DES PARAMETRES
//TODO MONITOR : AFFICHER LES VALEURS DANS LES WIDGETS
//TODO MONITOR : FAIRE L'AFFICHAGE DES PID ET DES ACTIONS

//TODO : ENVOI DES ACTIONS (METTRE UN OCTET POUR LE TYPE)

//TODO REINTEGRATE GCODEINTERFACE
//TODO ENABLE-DISABLE GCODEINTERFACE
//TODO PIN_INTERRUPT
//TODO DIGITALWRITE DIGITALREAD ANALOGWRITE ANALOGREAD IN HARDWARE_LANGUAGE_ABSTRACTION



/*
 * TaskScheduler Structure :
 *
 * Main thread: Task manager. Runs basic tasks, like
 * - Communication Interface
 * - Basic interaction
 * - StepperControl Plannification for TRACER
 *
 * A task can be added, in the queue. Priorities may be defined in a future commit.
 *
 *
 * Level 0 Thread : Running permanent tasks, on non prioritary interrupts
 * - Control loops;
 * - Step process function
 *
 */


#ifndef PROJECT_SYSTEM_H
#define PROJECT_SYSTEM_H


#include <sanity_check.h>

#include <DataStructures/Queue.h>
#include "_task_scheduler_data.h"

//All files that include this library may requrie the generation of a scheduler.
#include "scheduler_generation.h"

class TaskScheduler {

public:

    //Initialise TRACER's enabled modules.
    static void init();


private:

    //Verify that a type exists
    static bool check_sequence_type(uint8_t type);


public:

    //Start the scheduling - execution.
    static void run();

    //Schedule a task.
    static bool schedule_task(task_t *task);

    //Build and schedule a task.
    static bool schedule_task(uint8_t type, task_state_t (*f)(void *), void *args);

    //Returns the number of type-[type] tasks that can be scheduled.
    static const uint8_t available_spaces(uint8_t type);

    //Verify that nb_tasks can be enqueued
    //Lock the specified sequence.
    static void lock_sequence(uint8_t type);

    //get the state of a sequence.
    static bool is_sequence_locked(uint8_t type);

    static bool verify_schedulability(uint8_t task_type, uint8_t nb_tasks);

    static bool flood_enabled;


private:


    //The primary task pool
    static task_t *const task_pool;

    //The number of tasks stored in the task pool
    static uint8_t pool_task_nb;

    //The number of tasks stored in the task pool
    static uint8_t pool_task_spaces;

    //Task sequences
    static Queue<task_t> **const task_sequences;

    //First tasks flags
    static bool *const queues_locked;

    static void process_task_pool();

    static void process_task_sequences();

    static bool process_task(task_t *task);

    static uint8_t shift(boolean shift_enabled, task_t *task, uint8_t insert_index);

    static void process_task_sequences_singular();


};


/*
 * LOCK_SEQUENCE : This macro shall be used in your tasks to lock a given task_sequence.
 */
#define LOCK_SEQUENCE(type) {TaskScheduler::lock_sequence(type);}


#endif //PROJECT_SYSTEM_H
