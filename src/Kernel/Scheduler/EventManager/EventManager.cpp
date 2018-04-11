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

#include <malloc.h>
#include "EventManager.h"


/*
 * reset : initialise the event array;
 */

void EventManager::initialise_data() {

    //Delete all events;
    system_events->clear();

    //Delete all triggered events;
    triggered_events->clear();

}


//------------------------------------- Builders -------------------------------------

/*
 * add_event : adds an event to the supported events;
 */

void EventManager::add_event(const char *name) {

    //First, we must check that the event is not already registered;

    //Declare an unused index;
    uint8_t unused = 0;

    //If the event name already exists :
    if (search_event(name, &unused)) {

        //Log;
        std_out("Error in EventManager::add_event : an event is already has this name;");

        //Fail;
        return;

    }

    //First, we will create the new system event;
    SystemEvent *new_event = new SystemEvent(name, MAX_TASKS_PER_EVENT);

    //Then, add the event to the system event;
    system_events->add(new_event);

}


/*
 * register_to_event : search for an event matching the given name, and eventually add the given function to its 
 * 
 *  registered tasks;
 */

void EventManager::register_to_event(const char *name, task_state_t (*function)(void *)) {

    //Declare a search index;
    uint8_t index = 0;

    //If the given name doesn't match any event :
    if (!search_event(name, &index)) {
        
        //Log;
        std_out("Error in EventManager::register_to_event : no events matches the given name");
        
        //Fail;
        return;
        
    }
    
    //Register the provided function to the event's tasks;
    system_events->getElement(index)->addTask(function);

}


/*
 * un_register_to_event : this function will un-register the given function to the event matching the given name;
 */

void EventManager::un_register_to_event(const char *name, task_pointer_t function) {

    //Declare a search index;
    uint8_t index = 0;

    //If the given name doesn't match any event :
    if (!search_event(name, &index)) {

        //Log;
        std_out("Error in EventManager::un_register_to_event : no events matches the given name");

        //Fail;
        return;

    }

    //Un-register the provided function to the event's tasks;
    system_events->getElement(index)->removeTask(function);
    
}


/*
 * un_register_to_all : this function will un-register the given function to all events;
 */

void EventManager::un_register_to_all(task_pointer_t function) {

    //Cache the number of events;
    const uint8_t nb_events = system_events->getSize();
    
    //For each event : 
    for (uint8_t event_index = 0; event_index < nb_events; event_index++) {

        //Un-register the provided function to the event's tasks;
        system_events->getElement(event_index)->removeTask(function);
        
    }
    
}


//---------------------------------------------------- Event search -----------------------------------------------------


/*
 * search_event : this function will search the event matching the given name, eventually save its index,
 * 
 *  and return true if it was found;
 */

bool EventManager::search_event(const char *name, uint8_t *found_index) {

    //First, cache the current number of events;
    const uint8_t nb_events = system_events->getSize();

    //For every system event :
    for (uint8_t event_index = 0; event_index < nb_events; event_index++) {

        //If the event name matches the searched name :
        if (cstring::strcmp(name, system_events->getElement(event_index)->getName())) {

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
 * trigger_event : this function will
 */
void EventManager::trigger_event(const char *event_name) {

    //Declare the search index;
    uint8_t index = 0;

    //If the event name is not found :
    if (!search_event(event_name, &index)) {

        //Log :
        std_out("Error in EventManager::trigger_event : no event named "+string(event_name));

        //Fail;
        return;

    }

    //Add the event index to the set of triggered events;
    triggered_events->add(index);

}


/*
 * process_events : attempt to schedule all tasks registered to all events;
 *
 * One task is scheduled at time for every event, to avoid execution of only one event that could re-trigger himself;
 */

void EventManager::process_events() {

    //While spaces are available in the scheduler and events are triggered :
    while(TaskScheduler::available_spaces() && triggered_events->getSize()) {

        //For every triggered event index (vector is modified during iteration, so size not const) :
        for (uint8_t triggered_event_index = 0;
             triggered_event_index < triggered_events->getSize(); triggered_event_index++) {

            //If no more spaces are available in the scheduler, complete;
            if (!TaskScheduler::available_spaces())
                return;

            //Get the system event;
            uint8_t event_index = triggered_events->getElement(triggered_event_index);

            //Cache the event, to check for nullity;
            SystemEvent *event = system_events->getElement(event_index);

            //If the event is nullptr :
            if (event == nullptr) {

                //Log;
                std_out("Error in EventManager::process_events : an event index points to nullptr event;");

                //Remove the index;
                triggered_events->remove(event_index);

                //Decrease the counter, to re-iterate on the same one.
                triggered_event_index--;

            }

            //Execute the task;
            bool event_processed = event->scheduleNextTask();

            //If all event's tasks have been processed :
            if (event_processed) {

                //Remove the index;
                triggered_events->remove(event_index);

                //Decrease the counter, to re-iterate on the same one.
                triggered_event_index--;

            }

        }
    }

}


//------------------------------------------ Static declaration - Definition -------------------------------------------


//Registered events;
DynamicPointerBuffer<SystemEvent> *EventManager::system_events = new DynamicPointerBuffer<SystemEvent>(MAX_NB_EVENTS);

//Triggered events;
DynamicSet<uint8_t> *EventManager::triggered_events = new DynamicSet<uint8_t>(MAX_NB_EVENTS);