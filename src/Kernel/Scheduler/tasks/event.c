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

#include <malloc.h>
#include <DataStructures/Containers/container.h>
#include "event.h"


//-------------------------------- Initialisation --------------------------------

/*
 * event_create : this function will create an event in the heap with no tasks, and the required name;
 */

event_t *event_create(const char *name) {

    //First, let's allocate some space in the heap;
    void *ptr = malloc(sizeof(event_t));

    //If malloc failed :
    if (!ptr) {

        //Fail //TODO KERNEL PANIC;
        return 0;

    }

    /*
     * Now that the malloc has completed for sure, let's initialise it;
     */

    //Cache a casted copy of pointer to ease readability. Compiler optimised;
    event_t *event = (event_t *)ptr;

    //Initialise the event;
    *event = {
            .pending = false,
            .tasks = EMPTY_CONTAINER(task_t *),
            .next_task = 0,
    };

    //Everything has been initialised except the name;

    //Cache dest pointers;
    char *dst = event->name;

    //For each byte of the name, while termination is not encountered :
    for (uint8_t i = EVENT_NAME_SIZE; i--;) {

        //Copy the byte and cache it;
        char c = *(dst++) = *(name++);

        //If we copied a null byte :
        if (!c) {

            //Return the created event;
            return event;
        }

    }

    //If all letters were copied but no termination has been encountered, the name was too big. Error;
    return event;//TODO KERNEL PANIC;


}


//-------------------------------- Builders --------------------------------

/*
 * event_append_task : add a task to the event's task array;
 */

void event_append_task(event_t *event, task_t *task) {

    //elements (task_t *) are passer by pointer for insertion;
    container_append_element(&event->tasks, &task);

}


/*
 * event_clear : this function removes all tasks of the given event
 */

void event_clear(event_t *event) {

    //Clear the event's tasks container;
    container_clear(&event->tasks);

    //No more task to schedule;
    event->next_task = 0;

    //Event is no more pending;
    event->pending = false;

}


//-------------------------------- Task Scheduler --------------------------------

/*
 * event_set_pending : tries to set the event pending, and returns true if it succeeds;
 */

bool event_set_pending(event_t *event) {

    //If the event is already pending, nothing to do;
    if (event->pending)
        return false;

    //Cache the number of tasks in the event;
    const container_index_t nb_tasks = event->tasks.nb_elements;

    //If the event doesn't contain any tasks, nothing to do;
    if (!nb_tasks)
        return false;

    //Mark the event pending;
    event->pending = true;

    //Update the next task's index to the last task;
    event->next_task = 0;

}


/*
 * Get the current task to execute of the provided event;
 */

task_t *event_get_task(event_t *event) {

    //If the event is not pending :
    if (!event->pending) {

        //System error;
        return 0;//TODO ERROR;

    }

    //To avoid redundant access, let's cache the current task's id;
    container_index_t next_task_id = event->next_task;

    //First, we must get the next task, and increase the next task id; elements (task_t *) are returned by pointer;
    task_t *task = *(task_t **)container_get_element(&event->tasks, next_task_id++);

    //Then, we must update the next task pointer.

    //If the next task's id is outside the task's array :
    if (next_task_id >= event->tasks.nb_elements) {

        //All tasks have been executed, the event is not pending anymore;
        event->pending = false;
        event->next_task = 0;

    } else {

        //Almost another task has to be executed. Save the next task's id;
        event->next_task = next_task_id;
    }

    //Finally, return the cached task;
    return task;

}
