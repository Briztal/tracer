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

#include <Kernel/Scheduler/TaskStorage.h>
#include "EventManager.h"

namespace EventManager {

    //------------------------------------- Name search -------------------------------------

    //Get the index of an event by its name;
    bool search_event(const char *event, uint8_t *found_index);


    //------------------------------------- Fields -------------------------------------

    //Registered events;
    DynamicPointerBuffer<SystemEvent> systemEvents(MAX_NB_EVENTS);

    //Triggered events
    DynamicSet<uint8_t> pendingEvents(MAX_NB_EVENTS);

}

/*
 * reset : start the event array;
 */

void EventManager::reset() {

    //Delete all events;
    systemEvents.clear();

    //Delete all isPending events;
    pendingEvents.clear();

}


//------------------------------------- Builders -------------------------------------

/*
 * addEvent : adds an event to the supported events;
 */

void EventManager::addEvent(const char *name) {

    //First, we must check that the event is not already registered;

    //Declare an unused index;
    uint8_t unused = 0;

    //If the event name already exists :
    if (search_event(name, &unused)) {

        //Log;
        std_out("Error in EventManager::addEvent : an event is already has this name;");

        //Fail;
        return;

    }

    //First, we will create the new system event;
    SystemEvent *new_event = new SystemEvent(name, MAX_TASKS_PER_EVENT);

    //Then, add the event to the system event;
    systemEvents.add(new_event);

}


/*
 * registerToEvent : search for an event matching the given name, and eventually add the given function to its
 * 
 *  registered tasks;
 */

void EventManager::registerToEvent(const char *name, TaskData &task) {

    //Declare a search index;
    uint8_t index = 0;

    //If the given name doesn't match any event :
    if (!search_event(name, &index)) {

        //Log;
        std_out("Error in EventManager::registerToEvent : no events matches the given name");

        //Fail;
        return;

    }

    //Register the provided function to the event's tasks;
    systemEvents.getElement(index)->addTask(task);

}


//---------------------------------------------------- Event search -----------------------------------------------------


/*
 * search_event : this function will search the event matching the given name, eventually save its index,
 * 
 *  and return true if it was found;
 */

bool EventManager::search_event(const char *name, uint8_t *found_index) {

    //First, cache the current number of events;
    const uint8_t nb_events = systemEvents.getSize();

    //For every system event :
    for (uint8_t event_index = 0; event_index < nb_events; event_index++) {

        //If the event name matches the searched name :
        if (cstring::strcmp(name, systemEvents.getElement(event_index)->getName())) {

            //Save the found index;
            *found_index = event_index;

            //Complete;
            return true;

        }

    }

    //Fail, as no event matches the given name;
    return false;

}

//-------------------------------------------------- Event management --------------------------------------------------

/*
 * triggerEvent : this function will
 */

void EventManager::triggerEvent(const char *event_name) {

    //Declare the search index;
    uint8_t index = 0;

    //If the event name is not found :
    if (!search_event(event_name, &index)) {

        //Log :
        std_out("Error in EventManager::triggerEvent : no event named " + string(event_name));

        //Fail;
        return;

    }

}


/*
 * taskAvailable : this function returns true if a task can be executed;
 */

bool EventManager::taskAvailable() {

    //A task is available if an event is pending and the storage is not full;
    return ((bool) pendingEvents.getSize()) && (TaskStorage::availableSpaces());

}


/*
 * getTask : return the id pf the next task of the first pending event;
 */

uint8_t EventManager::getTask() {

    if (!taskAvailable()) //TODO KERNEL PANIC;
        return 0;

    //Get the first element of the array;
    uint8_t pendingEvent = pendingEvents.getElement(0);

    //Cache the event, to check for nullity;
    SystemEvent *event = systemEvents.getElement(pendingEvent);

    //If the event is nullptr, fail;
    if (event == nullptr) {

        return 0;//TODO KERNEL PANIC

    }

    //Execute the task;
    uint8_t task_index = event->getTask();

    //If all event's tasks have been processed :
    if (!event->isPending()) {

        //Remove the event of the pending list;
        pendingEvents.remove(0);

    }

    //Return the index of the task to execute;
    return task_index;

}

