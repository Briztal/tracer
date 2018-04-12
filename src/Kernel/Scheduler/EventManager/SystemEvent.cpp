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

#include <Kernel/Scheduler/TaskStorage.h>
#include "SystemEvent.h"


//-------------------------------- Initialisation --------------------------------

/*
 * Constructor : initialises the tasks array;
 */

SystemEvent::SystemEvent(const char *name, uint8_t max_tasks) :
        eventPending(false), nextTask(0), tasks(new Container<TaskData>(max_tasks)) {

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

void SystemEvent::addTask(TaskData task) {

    //Simply add the task to the set;
    tasks->add(task);

    //Nothing to do about task_to_schedule, as the new task is inserted at the end of the vector;

}


/*
 * removeAllTasks : this function removes all tasks from the set;
 */

void SystemEvent::removeAllTasks() {

    //Clear the task;
    tasks->clear();

    //No more task to schedule;
    nextTask = 0;

}


//-------------------------------- Name getter --------------------------------

/*
 * getName : returns the name of the event;
 */

const char *SystemEvent::getName() {

    //Return the name of the event;
    return name;

}


//-------------------------------- Task Scheduler --------------------------------

/*
 * setPending : sets the event isPending;
 */

void SystemEvent::setPending() {

    //Cache the number of tasks in the event;
    const uint8_t nbTasks = tasks->getSize();

    //If the event doesn't contain any tasks, nothing to do;
    if (!nbTasks)
        return;

    //Mark the event pending;
    eventPending = true;

    //Update the index to the last task;
    nextTask = nbTasks - (uint8_t) 1;

}


/*
 * taskAvailable : returns true if a task can be created;
 */

bool SystemEvent::isPending() {

    //A task can be created if the event is isPending and the task storage is not full;
    return eventPending;

}


/*
 * getTask : schedule the next task on the list;
 *
 *  It will return true only if all tasks have been executed;
 */

uint8_t SystemEvent::getTask() {

    if (!eventPending)
        return 0;//TODO Kernel Panic

    //Create the new task and get its index:
    uint8_t index = TaskStorage::addTask(*tasks->getPointer(nextTask));

    //If we just scheduled the last task :
    if (!nextTask) {

        //Mark the event not isPending;
        eventPending = false;

    } else {

        //If there are more tasks to execute, decrease the task index;
        nextTask--;

    }

    //Return the task's index;
    return index;

}
