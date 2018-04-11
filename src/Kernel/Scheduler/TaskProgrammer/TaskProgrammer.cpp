/*
  Taskprogrammer.cpp - Part of TRACER

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


#include <Kernel/Interaction/Interaction.h>
#include <Kernel/Scheduler/Systick.h>
#include <DataStructures/Containers/ObjectContainer.h>

#include "TaskProgrammer.h"


namespace TaskProgrammer {

    //----------------------------------- Builders -----------------------------------

    //Program a task;
    void _program_task(task_function_t task, uint32_t offset, uint32_t period, uint32_t nb_schedulings);


    //----------------------------------- Execution -----------------------------------

    //Enable all tasks that can be scheduled;
    void check_executability();


    //----------------------------------- Timeline reset -----------------------------------

    //Update all tasks durations according to the new timeline;
    void reset_timeline(uint32_t diff);


    //----------------------------------- Fields -----------------------------------

    //Programmed tasks;
    ObjectContainer<ProgrammedTask> programmedTasks(MAX_PROGRAMMED_TASKS);

    //Next time that we must check for a schedulable task; At init, a check must be made;
    uint32_t next_check_time = 0;

}


//----------------------------------- Initialisation -----------------------------------


/*
 * reset : this function will set all data in a safe state;
 */

void TaskProgrammer::reset() {

    //Reset all programmed tasks;
    programmedTasks.clear();

}


//----------------------------------- Builders -----------------------------------

/*
 * Program a one shot task in [offset] seconds;
 */

void TaskProgrammer::program_punctual_task(task_function_t task, uint32_t offset) {

    //Program the single shot task;
    _program_task(task, offset, offset, 1);

}

/*
 * program_repetitive_task : this function programs a repetitive task;
 */

void TaskProgrammer::program_repetitive_task(task_function_t task, uint32_t offset, uint32_t period,
                                             uint32_t nb_schedulings) {

    //If the number of schedulings is zero :
    if (!nb_schedulings) {

        //Log;
        std_out("Error in TaskProgrammer::program_repetitive_task : a repetitive task cannot be scheduled zero time;");

        //Ignore;
        return;

    }

    //Program the task
    _program_task(task, offset, period, nb_schedulings);

}


/*
 * program_repetitive_task : programs a repetitive task with an offset equal to its period;
 */

void TaskProgrammer::program_repetitive_task(task_function_t task, uint32_t period, uint32_t nb_schedulings) {

    //Program the task;
    program_repetitive_task(task, period, period, nb_schedulings);

}


/*
 * program_infinite_task : programs an infinite task;
 */

void TaskProgrammer::program_infinite_task(task_function_t task, uint32_t offset, uint32_t period) {

    //Program the task;
    _program_task(task, offset, period, 0);

}


/*
 * program_infinite_task : programs an infinite task that has its offset equal to its period;
 */

void TaskProgrammer::program_infinite_task(task_function_t task, uint32_t period) {

    //Program the task;
    program_infinite_task(task, period, period);

}


/*
 * _program_task : programs a task with the given set of parameters;
 */

void TaskProgrammer::_program_task(task_function_t task, uint32_t offset, uint32_t period, uint32_t nb_schedulings) {

    //If the task is null :
    if (task == nullptr) {

        //Log;
        std_out("Error in TaskProgrammer::_program_task : attempted to schedule a null task;");

        //Ignore;
        return;

    }

    //If the period is null :
    if (!period) {

        //Log;
        std_out("Error in TaskProgrammer::_program_task : attempted to schedule task with a null period;");

        //Ignore;
        return;

    }

    //Create and add the task to the programmed tasks;
    programmedTasks.add(ProgrammedTask(task, offset, period, nb_schedulings));

    //Get the pointer to the task in the container :
    ProgrammedTask *programmedTask = programmedTasks.get(programmedTasks.getSize() - (uint8_t) 1);

    //Cache the next execution time of the new task;
    const uint32_t next = programmedTask->nextExecutionTime;

    //Eventually modify the check time;
    if (next < next_check_time) next_check_time = next;

}


/*
 * deprogram_task : remove any programmed task that matches the given function;
 */

void TaskProgrammer::deprogram_task(task_function_t function) {

    //For each task :
    for (uint8_t task_index = 0; task_index < programmedTasks.getSize(); task_index++) {

        //Cache the task;
        ProgrammedTask *task = programmedTasks.get(task_index);

        //If the task is null :
        if (task == nullptr) {

            //Log;
            std_out("Error in TaskProgrammer::check_executability : a task pointer points to null;");

            //Remove the task;
            programmedTasks.remove(task_index);

            //Decrement the task, in order to re-iterate on the next element;
            task_index--;

            //Re-iterate
            continue;

        }

        //The task is not null;

        //If we found a match :
        if (task->function == function) {

            //Remove the task;
            programmedTasks.remove(task_index);

            //Decrement the task, in order to re-iterate on the next element;
            task_index--;


        }

    }

}


//----------------------------------- Execution -----------------------------------

/*
 * check_executability : this function will compare each task's scheduling time to the current time, and mark
 *
 *  them as executable if the current time is greater;
 */

void TaskProgrammer::check_executability() {

    //Cache the current time;
    uint32_t current_time = Systick::milliseconds();

    //If the next check time is not passed, nothing to do;
    if (current_time < next_check_time)
        return;

    //Declare the next time;
    uint32_t next_time = 0;

    //For each task :
    for (uint8_t task_index = programmedTasks.getSize(); task_index --;) {

        //Cache the task;
        ProgrammedTask *task = programmedTasks.get(task_index);

        //If the task is null :
        if (task == nullptr) {

            //Log;
            std_out("Error in TaskProgrammer::check_executability : a task pointer points to null;");

            //Remove the task;
            programmedTasks.remove(task_index);

            //Decrement the task, in order to re-iterate on the next element;
            task_index--;

            //Re-iterate
            continue;

        }


        //Cache the local scheduling time;
        uint32_t task_time = task->nextExecutionTime;

        //If the current time is superior to the task's scheduling time :
        if (current_time > task_time) {

            //Mask the task as executable;
            task->executionFlag = true;

            //update the task's period;
            task->nextExecutionTime = (task_time += task->period);

        }

        //If the next time is null or greater than the task's scheduling time :
        if ((!next_time) || (task_time < next_time)) {

            //Update the next check time;
            next_time = task_time;

        }

    }

    //Update the next check time with the minimum of all execution times;
    next_check_time = next_time;

}

//----------------------------------- Timeline reset -----------------------------------

/*
 * reset_timeline : update all tasks execution time according to the new timeline;
 */

void TaskProgrammer::reset_timeline(uint32_t diff) {

    //For each task :
    for (uint8_t task_index = programmedTasks.getSize(); task_index --;) {

        //Cache the task;
        ProgrammedTask *task = programmedTasks.get(task_index);

        //If the task is null :
        if (task == nullptr) {

            //Log;
            std_out("Error in TaskProgrammer::check_executability : a task pointer points to null;");

            //Remove the task;
            programmedTasks.remove(task_index);

            //Decrement the task, in order to re-iterate on the next element;
            task_index--;

            //Re-iterate
            continue;

        }

        if (task->nextExecutionTime < diff) {

            //Minor the execution time;
            task->nextExecutionTime = 0;

        } else {

            //Subtract the difference;
            task->nextExecutionTime -= diff;

        }

    }

    //Plan an executability check;
    next_check_time = 0;

}
