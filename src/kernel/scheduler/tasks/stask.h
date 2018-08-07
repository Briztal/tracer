/*
  task.h Part of TRACER

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


#ifndef TRACER_TASKDATA_H
#define TRACER_TASKDATA_H

#include "stdint.h"

#include <data_structures/containers/non_concurrent/llist.h>


typedef struct {
    //TODO INTERGRATE THE PRIORITY DATA;
} prio_data_t;


/*
 * The type of task;
 */
typedef enum {

    //The task is the loop executed by the scheduler when in inactive state;
            EMPTY_STASK,

    //The task is persistent (event or synchronous) Must not be terminated after completion;
            SERVICE_STASK,

    //The task comes from a sequence; Must be terminated after completion;
            SEQUENCE_STASK,

} stask_type_t;


/*
 * The service task (stask) struct;
 *
 *  Tasks are are not executed and removed in a particular order. They must be stored as linked elements.
 *
 *  A task is composed of a linked element, and of all data related to a task;
 */

typedef struct {

    //The linked element part;
    linked_element_t linked_element;

    //The task's function;
    void (*const function)(void *args);

    //The function's arguments;
    const void *const args;

    //The cleanup function to execute after the task's function;
    void (*const cleanup)();

    //The origin of the task
    const stask_type_t task_type;

    //The period between two preemptions for this task;
    uint16_t activity_time;

} stask_t;


//---------------------- Public Functions ----------------------

//Delete entirely a heap stored task composed struct;
void stask_delete(stask_t *task_p);

#endif //TRACER_TASKDATA_H
