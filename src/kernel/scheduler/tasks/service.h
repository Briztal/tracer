/*
  service.h - Part of TRACER

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

#include "stask.h"


/*
 * A service is composed of a task, and time parameters;
 */

typedef struct {

    //The task itself;
    stask_t task;

    //The time at which the next execution is supposed to happen;
    uint32_t next_exec_time;

    //The remaining number of times the service must be executed;
    uint32_t remaining_execs;

    //The service's period;
    const uint32_t period;

} service_t;


//----------------------------------- Initialisation -----------------------------------

//Initialise the service linked list;
void services_initialise(size_t max_nb_services);


//----------------------------------- Builders -----------------------------------


//TODO REMOVE PATCH : NOT CLOSED
//Program a repetitive task;
void service_add_temporary(void (*task_function)(void *), uint32_t offset, uint32_t period, uint32_t nb_execs,
                           uint16_t activity_time);


//TODO REMOVE PATCH : CLOSED;
//Program an infinite task;
void service_add_permanent(void (*task_function)(void *), uint32_t offset, uint32_t period, uint16_t activity_time);


//----------------------------------- Scheduler interaction -----------------------------------

//Are there services available ?
bool services_available_task();

//Get a service to execute;
stask_t *services_get_task();

//Reprogram an executed service;
void services_reprogram(service_t *service);



#endif //TRACER_TASKPROGRAMMER_H
