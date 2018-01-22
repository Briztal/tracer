/*
  EventManager.h - Part of TRACER

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

#ifndef TRACER_EVENTMANAGER_H
#define TRACER_EVENTMANAGER_H

#include <DataStructures/Vectors/ValueSet.h>
#include <DataStructures/Vectors/PointerVector.h>

#include "stdint.h"

#include "SystemEvent.h"


class EventManager {

    //------------------------------------- Initialisation -------------------------------------

private:

    //Initialise data in a safe state;
    static void initialise_data();


    //------------------------------------- Builders -------------------------------------

public:

    //Add an event;
    static void add_event(const char *name);

    //Register a function to a particular event;
    static void register_to_event(const char *name, void (*function)());

    //Un-register a function to a particular event;
    static void un_register_to_event(const char *name, void (*function)());

    //Un-register a function to all events;
    static void un_register_to_all(void (*function)());


    //------------------------------------- Name search -------------------------------------

private:

    //Get the index of an event by its name;
    static bool search_event(const char *event, uint8_t *found_index);


    //------------------------------------- Fields -------------------------------------

private:

    //Registered events;
    static PointerVector<SystemEvent> *system_events;


};


#endif //TRACER_EVENTMANAGER_H
