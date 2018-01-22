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

SystemEvent::SystemEvent(const char *name, uint8_t max_tasks) : tasks(new ValueSet<task_pointer_t>(max_tasks)) {

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


/*
 * addTask : add a task to the tasks set;
 */

void SystemEvent::addTask(task_pointer_t task) {

    //Simply add the task to the set;
    tasks->add(task);

}


/*
 * removeTask : this function removes the given task from the tasks set;
 */

void SystemEvent::removeTask(task_pointer_t t) {

    //Simply remove the task from the tasks set;
    tasks->remove(t);


}


/*
 * removeAllTasks : this function removes all tasks from the set;
 */

void SystemEvent::removeAllTasks() {

    //Clear the task;
    tasks->clear();

}


/*
 * getName : returns the name of the event;
 */

const char *SystemEvent::getName() {

    //Return the name of the event;
    return name;
}
