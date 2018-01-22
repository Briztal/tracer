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
#include <DataStructures/StringUtils.h>

#include "EventManager.h"


/*
 * initialise_data : initialise the event array;
 */

void EventManager::initialise_data() {

    //Delete all events;
    system_events->clear();

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

void EventManager::register_to_event(const char *name, void (*function)()) {

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

void EventManager::un_register_to_event(const char *name, void (*function)()) {

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

void EventManager::un_register_to_all(void (*function)()) {

    //Cache the number of events;
    const uint8_t nb_events = system_events->getSize();
    
    //For each event : 
    for (uint8_t event_index = 0; event_index < nb_events; event_index++) {
        
        //Un Register the provided function to the event's tasks;
        system_events->getElement(event_index)->removeAllTasks();
        
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
        if (StringUtils::strcmp(name, system_events->getElement(event_index)->getName())) {

            //Save the found index;
            *found_index = event_index;

            //Complete;
            return true;

        }

    }

    //Fail, as no event matches the given name;
    return false;

}



//------------------------------------------ Static declaration - Definition -------------------------------------------


//Registered events;
PointerVector<SystemEvent> *EventManager::system_events = new PointerVector<SystemEvent>(MAX_NB_EVENTS);