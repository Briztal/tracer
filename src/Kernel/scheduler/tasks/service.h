/*
  Taskprogrammer.h - Part of TRACER

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


#ifndef TRACER_TASKPROGRAMMER_H
#define TRACER_TASKPROGRAMMER_H

#include "stdint.h"
#include "task.h"

//----------------------------------- Initialisation -----------------------------------

//Initialise task queues in a safe state;
void services_initialise();


//----------------------------------- Builders -----------------------------------
//TODO REMOVE PATCH : NOT CLOSED
//Program a repetitive task;
void service_add_temporary(void (*task)(void *), uint32_t offset, uint32_t period, uint32_t nb_execs);


//TODO REMOVE PATCH : CLOSED;
//Program an infinite task;
void service_add_permanent(void (*task)(void *), uint32_t offset, uint32_t period);


//----------------------------------- Execution -----------------------------------

//Schedule tasks according to the current time (provided by kernel)
void process_services();


#endif //TRACER_TASKPROGRAMMER_H
