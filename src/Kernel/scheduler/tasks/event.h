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

#include <Kernel/scheduler/tasks/task.h>

#include <data_structures/containers/container.h>

//TODO CONFIG FILE
#define EVENT_NAME_SIZE 11

typedef struct {

    //Is the event isPending
    bool pending;

    //The index of the next task to schedule, in case of a isPending event;
    container_index_t next_task;

    //The event's name;
    char name[EVENT_NAME_SIZE];

    //The event's tasks;
    container_t tasks;

} event_t;


/*
 * Due to their size, events are stored in the heap, and accessed by pointer; This function will create and initialise
 *  an event;
 */
event_t *event_create(const char *name);

//Register a task to the event;
void event_append_task(event_t *, task_t *);

//Un-register all tasks of an event;
void event_clear(event_t *);

//Try to set the given event pending;
bool event_set_pending(event_t *event);

//Get the current task to execute of the provided event;
task_t *event_get_task(event_t *);

//-------------------------------- Fields --------------------------------

#endif //TRACER_EVENT_H
