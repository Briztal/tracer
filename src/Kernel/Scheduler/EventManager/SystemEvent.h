/*
  SystemEvent.h - Part of TRACER

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

#ifndef TRACER_EVENT_H
#define TRACER_EVENT_H


#include <DataStructures/Containers/DynamicSet.h>


/*
 * The system event class : contains the two pieces of data that define an event :
 *  - a name (array of chars);
 *  - a set of tasks;
 */

typedef task_state_t (*task_pointer_t)(void *);


class SystemEvent {


    //-------------------------------- Initialisation --------------------------------

public:

    //Constructor;
    SystemEvent(const char *name, uint8_t max_tasks);

    //Destructor;
    ~SystemEvent();


    //-------------------------------- Builders --------------------------------

public:

    //Register a task to the event;
    void addTask(task_pointer_t);

    //Un-register a task to the event;
    void removeTask(task_pointer_t t);

    //Un-register all tasks;
    void removeAllTasks();

    //-------------------------------- Name getter --------------------------------

public:

    //Get the name of the event;
    const char *getName();


    //-------------------------------- Task Scheduler --------------------------------

public:

    //Schedule the next task on the list;
    bool scheduleNextTask();


    //-------------------------------- Fields --------------------------------

private:

    //The event's name;
    char name[11]{0};

    //The event's tasks;
    DynamicSet<task_pointer_t> *tasks;

    //The index of the next task to schedule;
    uint8_t task_to_schedule;


};


#endif //TRACER_EVENT_H
