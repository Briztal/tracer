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
#include "event.h"

#include <kernel/kernel.h>


//-------------------------------- Initialisation --------------------------------

/*
 * event_create : this function will create an event in the heap with no tasks, and the required name;
 */

event_t *event_create(const char *name) {

    //Define the new event;
    event_t init =  {
            .tasks = EMPTY_LINKED_RING(),
            .nb_pending = 0,
    };

    //Allocate some memory in the heap;
    event_t *event = kernel_malloc_copy(sizeof(event_t), &init);


    //Everything has been initialised except the name;

    //Cache dest pointers;
    char *dst = event->name;

    //For each byte of the name, while termination is not encountered :
    for (uint8_t i = EVENT_NAME_LENGTH; i--;) {

        //Copy the byte and cache it;
        char c = *(dst++) = *(name++);

        //If we copied a null byte :
        if (!c) {

            //Return the created event;
            return event;
        }

    }

    //Nullify the end of the name;
    event->name[EVENT_NAME_LENGTH + 1] = 0;

    //If all letters were copied but no termination has been encountered, the name was too big. Error;
    return event;//TODO kernel PANIC;


}


//-------------------------------- Builders --------------------------------

/*
 * event_append_task : add a task to the event's task array;
 */

void event_append_task(event_t *event, task_t *task) {

    //Append the task to the event's tasks list;
    lring_insert_after(&event->tasks, (linked_element_t *) task);

    //If the event is pending, increment the number of pending tasks;
    if (event->nb_pending)
        event->nb_pending++;

}


//-------------------------------- Task Scheduler --------------------------------

/*
 * event_set_pending : tries to set the event pending, and returns true if it succeeds;
 */

bool event_set_pending(event_t *event) {

    //Cache the number of tasks in the event;
    const size_t nb_tasks = event->tasks.nb_elements;

    //Mark the event pending;
    event->nb_pending = nb_tasks;

    //If the event doesn't contain any tasks, nothing to do;
    if (!nb_tasks)
        return false;

    //Complete;
    return true;

}


/*
 * Get the current task to execute of the provided event;
 */

task_t *event_get_task(event_t *event) {

    //If the event is not pending :
    if (!event->nb_pending) {

        //System error;
        return 0;//TODO ERROR;

    }

    //Decrement the number of pending tasks;
    event->nb_pending--;

    //Get the next task;
    task_t *task = (task_t *) &event->tasks.current;

    //Go to the next task;
    lring_incr((linked_ring_t *) event);

    //Finally, return the cached task;
    return task;

}
