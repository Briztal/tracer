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

#include "stdint.h"

#include "stdbool.h"

#include "task.h"

//------------------------------------- Initialisation -------------------------------------

//Initialise events data in a safe state;
void events_reset();


//------------------------------------- Builders -------------------------------------

//Add an event;
void events_add(const char *name);

//Register a function to a particular event;
void events_register(const char *name, task_t *task);


//------------------------------------- Event management -------------------------------------

//Trigger a particular event;
void events_trigger(const char *event_name);

//Process all triggered events;
void events_process();


//------------------------------------- Task Execution -------------------------------------

//Can a task be executed ?
bool taskAvailable();

//Get the next task to be executed;
uint8_t getTask();


#endif //TRACER_EVENTMANAGER_H
