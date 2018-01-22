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


#include <DataStructures/Vectors/ValueSet.h>


/*
 * The system event class : contains the two pieces of data that define an event :
 *  - a name (array of chars);
 *  - a set of tasks;
 */

typedef void (*task_pointer_t)();


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


    //-------------------------------- Fields --------------------------------

private:

    //The event's name;
    char name[11]{0};

    //The event's tasks;
    ValueSet<task_pointer_t> *tasks;

};


#endif //TRACER_EVENT_H
