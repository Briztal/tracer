/*
  EventManager.cpp - Part of TRACER

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

#include <data_structures/containers/container.h>
#include <data_structures/string/string.h>
#include <string.h>
#include <data_structures/containers/llist.h>

#include "events.h"

#include "event.h"


/*
 * TODO EVENT REMOVAL PATCH :
 *
 *  There are three things to add to the model :
 *      - Modify the sequences_initialised variable in the event structure, to share its sequences_initialised : IDLE, PENDING, CLOSED
 *
 *      - The task recuperation function must check that the event is deletable after caching the task;
 *          if it is deletable, it will delete all its tasks, except the last one that is currently in execution;
 *
 *      - A function that will search by name for an event to delete, and then check for its sequences_initialised.
 *          If it is idle, delete it, and if it is pending, mark it as deletable;
 *
 */


//------------------------------------- Fields -------------------------------------

//Non pending events;
linked_list_t events = EMPTY_LINKED_LIST();

//Pending events;
linked_list_t pending_events = EMPTY_LINKED_LIST();

//Events can only be created by the scheduler;
bool locked = false;

//------------------------------------- Private functions -------------------------------------

//Get the index of an event by its name;
event_t *event_search(linked_list_t list, const char *name);



//------------------------------------- Implementation -------------------------------------

/*
 * events_add : creates an event and store it in the event container;
 */

void events_add(const char *name) {

    //If events are locked :
    if (locked)
        return;//TODO ERROR;

    //If the event name already exists (unlocked -> pending empty):
    if (event_search(events, name)) {

        //Nothing to do;
        return;

    }

    //Create an event in the heap with the required name;
    event_t *event = event_create(name);

    //Insert the newly created event at the end of the pending events list;
    llist_insert_end(&events, (linked_element_t *) event);

}


/*
 * events_register : search for an event matching the given name, and eventually add the given function
 *  to its registered tasks;
 */

void events_register(const char *name, task_t *task) {

    //If a null pointer was provided :
    if (task == 0) {

        //TODO ERROR NULL PTR;

        //Ignore;
        return;

    }

    //Search for an event matching the given name;
    //event_t *matching_event = event_search(name);

    /*
    if (!matching_event)
        matching_event = event_sear

    //If the given name doesn't match any event :
    if (!matching_event) {

        //Error
        //TODO KERNEL PANIC std_out("Error in EventManager::registerToEvent : no events matches the given name");

        //Fail;
        return;

    }


    //Register the provided function to the event's tasks;
    event_append_task(matching_event, task);

     */

}


//---------------------------------------------------- Event search -----------------------------------------------------


/*
 * search_event : this function will search the event matching the given name, eventually save its index,
 * 
 *  and return true if it was found;
 */

event_t *event_search(linked_list_t list, const char *name) {

    //Cache the number of events;
    size_t nb_events = events.nb_elements;

    //Cache the first event;
    event_t *event = (event_t *) events.first;

    //While the event exists :
    while (nb_events--) {

        //If the event name matches the searched name :
        if (strcmp(name, event->name)) {

            //Return the matching event;
            return event;

        }

    }

    //Fail, as no event matches the given name;
    return 0;

}


//---------------------------------------- Event trigger and execution ----------------------------------------

/*
 * triggerEvent : this function will search for an event matching the given name and trigger it if found;
 */

void events_trigger(const char *name) {

    /*
    //Declare the search index;
    event_t *matching_event = event_search(name);

    //If the event name is not found :
    if (!matching_event) {

        //Log :
        //TODO ERROR std_out("Error in EventManager::triggerEvent : no event named " + string(event_name));

        //Fail;
        return;

    }

    //Now, let's try to mark the event pending;
    bool success = event_set_pending(matching_event);

    //If the event could be set pending :
    if (success) {

        //Add the event to the pending container;
        append_pending(matching_event);

    }

     */
}


/*
 * taskAvailable : this function returns true if a task can be executed;
 */

bool events_task_executable() {

    //A task is available if an event is pending and the storage is not full;
    return ((bool) pending_events.nb_elements);

}


/*
 * getTask : return the id pf the next task of the first pending event;
 */

task_t *events_get_task() {

    /*
    if (!events_task_executable()) //TODO KERNEL PANIC;
        return 0;

    //Get the first element of the array;
    event_t *pending_event = get_pending(0);

    //If the event is nullptr, fail;
    if (!pending_event) {

        return 0;//TODO KERNEL PANIC

    }

    //Get the next task of the event. Will be marked as not pending if no more tasks are to execute;
    task_t *task = event_get_task(pending_event);

    //If the queried task was the last to execute (event marked as not pending);
    if (!pending_event->pending) {

        //Remove the event of the pending list;
        container_remove_element(&pending_events, 0);

    }

    //Return the task to execute;
    return task;

     */

}

