/*
  Taskprogrammer.h - Part of TRACER

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

/*
 * The TaskProgrammer class : it is in charge of programming tasks.
 *
 *  It can program any task to be executed in a certain amount of time, a certain number of time.
 *
 *  The delay is not rigorous, it can exceed the programmed time, in case of task overflow, so
 *
 *      please do not use this class to program real-time tasks;
 */

#ifndef TRACER_TASKPROGRAMMER_H
#define TRACER_TASKPROGRAMMER_H

#include <DataStructures/Containers/PointerContainer.h>
#include "stdint.h"
#include "ProgrammedTask.h"

class TaskProgrammer {


    //----------------------------------- Initialisation -----------------------------------

public:

    //Initialise the hardware in a safe state;
    static void intialise_hardware();

    //Initialise task queues in a safe state;
    static void initialise_data();


    //----------------------------------- Seconds counting -----------------------------------

private:

    //The function to count seconds;
    static void count_seconds();

    //The number of seconds ellapsed since last reset;
    static volatile uint32_t seconds;


    //----------------------------------- Builders -----------------------------------

public:

    //Program a one shot task;
    static void program_punctual_task(task_pointer_t task, uint32_t offset);

    //Program a repetitive task;
    static void program_repetitive_task(task_pointer_t task, uint32_t offset, uint32_t period, uint32_t nb_schedulings);

    //Program a repetitive task with offset equal to the period;
    static void program_repetitive_task(task_pointer_t task, uint32_t period, uint32_t nb_schedulings);


    //Program an infinite task;
    static void program_infinite_task(task_pointer_t task, uint32_t offset, uint32_t period);

    //Program an infinite task with offset equal to the period;
    static void program_infinite_task(task_pointer_t task, uint32_t period);


    //Un-program a task;
    static void un_program_task(task_pointer_t task);

private:

    //Program a task;
    static void _program_task(task_pointer_t task, uint32_t offset, uint32_t period, uint32_t nb_schedulings);


    //----------------------------------- Execution -----------------------------------

public:

    //Schedule tasks according to the current time (provided by kernel)
    static void process_tasks();


private:

    //Enable all tasks that can be scheduled;
    static void check_schedulability(uint32_t kernel_time);

    //Schedule as many schedulable tasks as possible;
    static void _schedule_tasks(uint32_t current_time);


    //----------------------------------- Fields -----------------------------------

    //Programmed tasks;
    static PointerVector<ProgrammedTask> *programmed_tasks;

    //Next time that a task can be scheduled;
    static uint32_t next_check_time;

    //A flag, set if some tasks can currently be executed;
    static bool schedulable_tasks;

    static void get_next_check_time();

};


#endif //TRACER_TASKPROGRAMMER_H
