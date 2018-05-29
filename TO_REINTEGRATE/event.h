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

#include "stdint.h"

#include "stdbool.h"

#include <kernel/scheduler/tasks/task.h>


//TODO CONFIG FILE
#define EVENT_NAME_LENGTH 10


/*
 * An event is composed of a linked list, a name and a status;
 */

typedef struct {

    //The linked list base;
    linked_ring_t tasks;

    //The number of pending tasks;
    size_t nb_pending;

    //The event's name;
    char name[EVENT_NAME_LENGTH + 1];

} event_t;


//Create an event in the heap;
event_t *event_create(const char *name);

//Register a task to the event;
void event_append_task(event_t *, task_t *);

//Un-register all tasks of an event;
//TODO REMOVE PATCH void event_clear(event_t *);

//Try to set the given event pending;
bool event_set_pending(event_t *event);

//Get the current task to execute of the provided event;
task_t *event_get_task(event_t *);

//-------------------------------- Fields --------------------------------

#endif //TRACER_EVENT_H
