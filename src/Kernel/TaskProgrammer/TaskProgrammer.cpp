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


#include "TaskProgrammer.h"

//----------------------------------- Initialisation -----------------------------------

/*
 * intialise_hardware : this function will initialise properly the seconds counting environment;
 */

void TaskProgrammer::intialise_hardware() {

    //Clean the hardware;
    clean_taskprog_interrupt();

    //Reset the seconds counter;
    seconds = 0;

    //Setup the timer interruption to occur at 1 hertz and to call count_seconds;
    setup_taskprog_interrupt(count_seconds, 1);

}


/*
 * initialise_data : this function will set all data in a safe state;
 */

void TaskProgrammer::initialise_data() {

    //Reset the seconds counter;
    seconds = 0;

    //Reset all programmed tasks;
    programmed_tasks->clear();

    //Program a check at runtime;
    next_check_time = 0;

    //Program a scheduling at runtime;
    schedulable_tasks = true;

}


//----------------------------------- Seconds counting -----------------------------------

/*
 * count_seconds : this function increments the number of seconds ellapsed since last reset;
 */

void TaskProgrammer::count_seconds() {

    seconds++;

}

//----------------------------------- Builders -----------------------------------

/*
 * Program a one shot task in [offset] seconds;
 */

void TaskProgrammer::program_punctual_task(task_pointer_t task, uint32_t offset) {

    //Program the single shot task;
    _program_task(task, offset, offset, 1);

}

/*
 * program_repetitive_task : this function programs a repetitive task;
 */

void TaskProgrammer::program_repetitive_task(task_pointer_t task, uint32_t offset, uint32_t period,
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

void TaskProgrammer::program_repetitive_task(task_pointer_t task, uint32_t period, uint32_t nb_schedulings) {

    //Program the task;
    program_repetitive_task(task, period, period, nb_schedulings);

}


/*
 * program_infinite_task : programs an infinite task;
 */

void TaskProgrammer::program_infinite_task(task_pointer_t task, uint32_t offset, uint32_t period) {

    //Program the task;
    _program_task(task, offset, period, 0);

}


/*
 * program_infinite_task : programs an infinite task that has its offset equal to its period;
 */

void TaskProgrammer::program_infinite_task(task_pointer_t task, uint32_t period) {

    //Program the task;
    program_infinite_task(task, period, period);

}


/*
 * _program_task : programs a task with the given set of parameters;
 */

void TaskProgrammer::_program_task(task_pointer_t task, uint32_t offset, uint32_t period, uint32_t nb_schedulings) {

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

    //The task is clear;

    uint32_t first_time = seconds + offset;

    //Create the task;
    ProgrammedTask *programmedTask = new ProgrammedTask(task, first_time, period, nb_schedulings);

    //Add the task to the programmed tasks;
    programmed_tasks->add(programmedTask);

    //Eventually modify the check time;
    if (first_time < next_check_time) next_check_time = first_time;

}


/*
 * un_program_task : remove any programmed taks that matches the given function;
 */

void TaskProgrammer::un_program_task(task_pointer_t function) {

    //A flag, set if a task has been effectively removed;
    bool removed = false;

    //For each task :
    for (uint8_t task_index = 0; task_index < programmed_tasks->getSize(); task_index++) {

        //Cache the task;
        ProgrammedTask *task = programmed_tasks->getElement(task_index);

        //If the task is null :
        if (task == nullptr) {

            //Log;
            std_out("Error in TaskProgrammer::check_schedulability : a task pointer points to null;");

            //Remove the task;
            programmed_tasks->remove(task_index);

            //Decrement the task, in order to re-iterate on the next element;
            task_index--;

            //Re-iterate
            continue;

        }

        //The task is not null;

        //If we found a match :
        if (task->task == function) {

            //Remove the task;
            programmed_tasks->remove(task_index);

            //Decrement the task, in order to re-iterate on the next element;
            task_index--;

            //Set the removed flag;
            removed = true;

        }

    }


    //If a task was removed, update the next check time;
    if (removed) get_next_check_time();

}


//----------------------------------- Execution -----------------------------------

/*
 * process_tasks : this function will mark all schedulable tasks, and try to schedule all schedulable tasks;
 */

void TaskProgrammer::process_tasks() {

    //Cache the current time;
    uint32_t current_time = seconds;

    //If tasks are to schedule now, and couldn't be scheduled at check time :
    if (schedulable_tasks) {

        //Schedule as many schedulable tasks as possible;
        _schedule_tasks(current_time);

    }

    //If we didn't reach the next check time yet :
    if (current_time < next_check_time) {

        //Reprogram;
        return;

    }

    check_schedulability(current_time);

    //If tasks have just been marked schedulable :
    if (schedulable_tasks) {

        //Schedule as many schedulable tasks as possible;
        _schedule_tasks(current_time);

    }

}


/*
 * check_schedulability : this function will compare each task's scheduling time to the current time, and mark
 *
 *  them as schedulable if the current time is greater;
 */

void TaskProgrammer::check_schedulability(uint32_t current_time) {

    //For each task :
    for (uint8_t task_index = 0; task_index < programmed_tasks->getSize(); task_index++) {

        //Cache the task;
        ProgrammedTask *task = programmed_tasks->getElement(task_index);

        //If the task is null :
        if (task == nullptr) {

            //Log;
            std_out("Error in TaskProgrammer::check_schedulability : a task pointer points to null;");

            //Remove the task;
            programmed_tasks->remove(task_index);

            //Decrement the task, in order to re-iterate on the next element;
            task_index--;

            //Re-iterate
            continue;

        }

        //The task is not null;

        //If the task can already be scheduled, no work needed;
        if (task->isSchedulable)
            continue;

        //If the current time is superior to the task's scheduling time :
        if (current_time > task->nextExecutionTime) {

            //Mask the task as schedulable;
            task->isSchedulable = true;

        }

    }
}


/*
 * _schedule_tasks : this function will :
 *
 *  - attempt to execute all schedulable tasks;
 *
 *  - determine the next execution time;
 */

void TaskProgrammer::_schedule_tasks(uint32_t current_time) {

    //A flag, set if a task has been scheduled;
    bool scheduled = false;

    //For each task :
    for (uint8_t task_index = 0; task_index < programmed_tasks->getSize(); task_index++) {

        //Cache the task;
        ProgrammedTask *task = programmed_tasks->getElement(task_index);

        //If the task is null :
        if (task == nullptr) {

            //Log;
            std_out("Error in TaskProgrammer::check_schedulability : a task pointer points to null;");

            //Remove the task;
            programmed_tasks->remove(task_index);

            //Decrement the task, in order to re-iterate on the next element;
            task_index--;

            //Re-iterate
            continue;

        }

        //The task is not null;

        //If the task is schedulable and if a space is available in the scheduler;
        if ((task->isSchedulable) && (TaskScheduler::available_spaces(255))) {

            //Schedule the task;
            TaskScheduler::schedule_task(255, task->task, nullptr, nullptr, nullptr);

            //Determine the new execution time;
            task->nextExecutionTime = current_time + task->period;

            //Mark the task as scheduled;
            task->isSchedulable = false;

            //Cache the remaining schedulings;
            uint32_t remaining_schedulings = task->remainingSchedulings;

            std_out(string("Remaining schedulings : ") + remaining_schedulings);

            //Set the scheduled flag;
            scheduled = true;

            //If the task is not infinite :
            if (remaining_schedulings) {

                //Decrement the number of schedulings;
                remaining_schedulings--;

                //If all schedulings have been done :
                if (!remaining_schedulings) {

                    //Remove the task;
                    programmed_tasks->remove(task_index);

                    //Decrement the task, in order to re-iterate on the next element;
                    task_index--;

                    //Re-iterate
                    continue;

                }

                //If not, update the remaining number of schedulings;
                task->remainingSchedulings = remaining_schedulings;

            }

        }

    }


    //If tasks have been scheduled :
    if (scheduled) {

        //Update the next check time;
        get_next_check_time();

    }

}

void TaskProgrammer::get_next_check_time() {


    //if no tasks are programmed :
    if (!programmed_tasks->getSize()) {

        //Disable the checking;
        next_check_time = 0;

        //Return;

    }

    //Declare the next time;
    uint32_t next_time = 0;

    //For each task :
    for (uint8_t task_index = 0; task_index < programmed_tasks->getSize(); task_index++) {

        //Cache the task;
        ProgrammedTask *task = programmed_tasks->getElement(task_index);

        //If the task is null :
        if (task == nullptr) {

            //Log;
            std_out("Error in TaskProgrammer::check_schedulability : a task pointer points to null;");

            //Remove the task;
            programmed_tasks->remove(task_index);

            //Decrement the task, in order to re-iterate on the next element;
            task_index--;

            //Re-iterate
            continue;

        }

        //The task is not null;

        //Cache the local scheduling time;
        uint32_t task_time = task->nextExecutionTime;

        //If the next time is null or greater than the task's scheduling time :
        if ((!next_time) || (task_time < next_time)) {

            //Update the next check time;
            next_time = task_time;

        }

    }

    //Update the next check time with the minimum of all execution times;
    next_check_time = next_time;

}



//----------------------------------------- Static declarations - definitions ------------------------------------------


//Programmed tasks;
PointerVector<ProgrammedTask> *TaskProgrammer::programmed_tasks = new PointerVector<ProgrammedTask>(
        MAX_PROGRAMMED_TASKS);


//Next time that a task can be scheduled;
uint32_t TaskProgrammer::next_check_time = 0;

//A flag, set if some tasks can currently be executed;
bool TaskProgrammer::schedulable_tasks = true;

//The seconds counter;
volatile uint32_t TaskProgrammer::seconds = 0;