/*
  SystemEvent.cpp - Part of TRACER

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

#include "SystemEvent.h"

//-------------------------------- Initialisation --------------------------------


SystemEvent::SystemEvent(const char *name, uint8_t max_tasks) : tasks(new DynamicSet<task_pointer_t>(max_tasks)) {

    //First, let's set the name;
    for (uint8_t i = 0; i<10; i++) {

        //Cache the current char;
        char c = name[i];

        //If the char is null, stop copying;
        if (!c)
            break;

        //If the char is not null, copy it and re-iterate;
        this->name[i] = c;

    }

}


/*
 * Destructor : deletes the task vector;
 */

SystemEvent::~SystemEvent() {

    //Delete the task vector;
    delete tasks;

}


//-------------------------------- Builders --------------------------------

/*
 * addTask : add a task to the tasks set;
 */

void SystemEvent::addTask(task_pointer_t task) {

    //Simply add the task to the set;
    tasks->add(task);

    //Nothing to do about task_to_schedule, as the new task is inserted at the end of the vector;

}


/*
 * removeTask : this function removes the given task from the tasks set;
 */

void SystemEvent::removeTask(task_pointer_t t) {

    //We must know the index of an eventual deletion;
    uint8_t index = 0;

    //Simply remove the task from the tasks set; If no element is found :
    if (!tasks->remove(t, &index)) {

        //Return, no modification to make;
        return;

    }

    /*
     * If an element has been deleted (at index [index]) :
     *
     *  The task_to_schedule index must be moved only if the removed index was strictly before task_to_schedule;
     */

    //If the removed index is below task_to_schedule, decrease it of one;
    if (index < task_to_schedule)
        task_to_schedule--;

}


/*
 * removeAllTasks : this function removes all tasks from the set;
 */

void SystemEvent::removeAllTasks() {

    //Clear the task;
    tasks->clear();

    //No more task to schedule;
    task_to_schedule = 0;

}


//-------------------------------- Name getter --------------------------------


/*
 * getName : returns the name of the event;
 */

const char *SystemEvent::getName() {

    //Return the name of the event;
    return name;
}

//-------------------------------- Task Scheduling --------------------------------


/*
 * scheduleNextTask : schedule the next task on the list;
 *
 *  It will return true only if all tasks have been executed;
 */

bool SystemEvent::scheduleNextTask() {

    //If the next task to schedule is outside the task array (can happen if empty task list) :
    if (task_to_schedule >= tasks->getSize()) {

        //Reset the scheduling environment;
        task_to_schedule = 0;

        //Succeed;
        return true;

    }

    //If the task scheduler has no more spaces left :
    if (!TaskScheduler::available_spaces(255)) {

        //Log;
        std_out("Error in SystemEvent::scheduleNextTask : function called with no space left in the TaskScheduler;");

        //Fail, still tasks to execute;
        return false;

    }

    //Schedule a task of type 255, executing our function;
    TaskScheduler::schedule_task(255, tasks->getElement(task_to_schedule), nullptr);

    //Increment the task to execute;
    task_to_schedule++;

    //If the next task to schedule is outside the task array :
    if (task_to_schedule == tasks->getSize()) {

        //Reset the scheduling environment;
        task_to_schedule = 0;

        //Succeed, as all tasks have been scheduled;
        return true;

    }

    //If the next task to execute is in the tasks array, other tasks are to execute.
    return false;

}
