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


#include <DataStructures/Containers/ObjectContainer.h>
#include <Kernel/Scheduler/TaskData.h>


/*
 * The system event class : contains the two pieces of data that define an event :
 *  - a name (array of chars);
 *  - a set of tasks;
 */


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
    void addTask(TaskData);

    //Un-register all tasks;
    void removeAllTasks();


    //-------------------------------- Name getter --------------------------------

public:

    //Get the name of the event;
    const char *getName();


    //-------------------------------- Task Scheduler --------------------------------

public:

    //Is the event isPending (un-executed tasks remain);
    bool isPending();

    //Set the event isPending;
    void setPending();

    //Get the next task on the list;
    uint8_t getTask();


    //-------------------------------- Fields --------------------------------

private:

    //Is the event isPending
    bool eventPending;

    //The index of the next task to schedule, in case of a isPending event;
    uint8_t nextTask;

    //The event's name;
    char name[11]{0};

    //The event's tasks;
    Container<TaskData> *tasks;

};


#endif //TRACER_EVENT_H
