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

#include <DataStructures/Containers/DynamicSet.h>
#include <DataStructures/Containers/DynamicPointerBuffer.h>

#include "stdint.h"

#include "SystemEvent.h"


namespace EventManager {

    //------------------------------------- Initialisation -------------------------------------

    //Initialise data in a safe state;
    void reset();


    //------------------------------------- Builders -------------------------------------

    //Add an event;
    void addEvent(const char *name);

    //Register a function to a particular event;
    void registerToEvent(const char *name, TaskData &data);


    //------------------------------------- Event management -------------------------------------

    //Trigger a particular event;
    void triggerEvent(const char *event_name);

    //Process all triggered events;
    void process_events();


    //------------------------------------- Task Execution -------------------------------------

    //Can a task be executed ?
    bool taskAvailable();

    //Get the next task to be executed;
    uint8_t getTask();


};


#endif //TRACER_EVENTMANAGER_H
