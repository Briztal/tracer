/*
  Taskprogrammer.cpp - Part of TRACER

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


#include <Kernel/Scheduler/systick.h>

#include <DataStructures/Containers/container.h>
#include <malloc.h>


#include "service.h"
typedef struct {

    //The time at which the next execution is supposed to happen;
    uint32_t next_exec_time;

    //The remaining number of times the service must be executed;
    uint32_t remaining_execs;

    //The service's period;
    uint32_t period;

    //The task itself;
    task_t * task;

    //A flag, set if the task must be executed;
    bool executionFlag;

} service_t;


//----------------------------------- Builders -----------------------------------

//Add a service;
void _service_add(task_t *task, uint32_t offset, uint32_t period, uint32_t nb_execs);


//----------------------------------- Execution -----------------------------------

//Mark all services that can be executed;
void services_check_executability();


//----------------------------------- Timeline reset -----------------------------------

//Update all tasks durations according to the new timeline;
void services_reset_timeline(uint32_t diff);


//----------------------------------- Fields -----------------------------------

//Programmed tasks;
container_t services = EMPTY_CONTAINER(service_t *);

//Next time that we must check for a schedulable task; At init, a check must be made;
uint32_t next_check_time = 0;


//----------------------------------- Builders -----------------------------------


/*
 * program_repetitive_task : this function adds a temporary service task;
 */

void void service_add_temporary(task_t *task, uint32_t offset, uint32_t period, uint32_t nb_execs) {

    //If the service mustn't be scheduled, nothing to do;
    if (!nb_execs) {

        //Ignore;
        return;

    }

    //Program the task
    _service_add(task, offset, period, nb_execs);

}



/*
 * service_add_permanent : adds a permanent service;
 */

void service_add_permanent(task_t *task, uint32_t offset, uint32_t period) {

    //Program the task;
    _service_add(task, offset, period, 0);

}



/*
 * _program_task : programs a task with the given set of parameters;
 */

void _service_add(task_t * task, uint32_t offset, uint32_t period, uint32_t remaining_execs) {

    //If the task is null :
    if (task == 0) {
        
        //TODO ERROR NULL POINTER
        
        //Ignore;
        return;

    }

    //If the period is null :
    if (!period) {

        //TODO ERROR NULL PERIOD
        
        //Ignore;
        return;

    }
    
    //First, allocate some heap memory for the new service data;
    void *ptr = malloc(sizeof(service_t));
    
    //If the reallocation failed : 
    if (!ptr) {
        
        //TODO ERROR MEMORY EXCEPTION;
        return;
        
    }
    
    //Cache a casted copy for readability. Compiler optimised,
    service_t *service = (service_t *)ptr;

    //Initialise the service;
    *service = {
            .next_exec_time = offset + systick_milliseconds(),
            .remaining_execs = remaining_execs,
            .period = period,
            .task = task,
            .executionFlag = false,
    };

    //Add the service to the services array;
    container_append_element(&services, &service);

    //Cache the next execution time of the new task;
    const uint32_t next = service->next_exec_time;

    //Eventually modify the check time;
    if (next < next_check_time) next_check_time = next;

}


//----------------------------------- Execution -----------------------------------

/*
 * check_executability : this function will compare each task's scheduling time to the current time, and mark
 *
 *  them as executable if the current time is greater;
 */

void services_check_executability() {

    //Cache the current time;
    uint32_t current_time = systick_milliseconds();

    //If the next check time is not passed, nothing to do;
    if (current_time < next_check_time)
        return;

    //Declare the next time;
    uint32_t next_time = 0;

    //For each task :
    for (container_index_t service_index = services.nb_elements; service_index--;) {

        //Cache the service;
        service_t *service = *(service_t **)container_get_element(&services, service_index);

        //Cache the local scheduling time;
        uint32_t task_time = service->next_exec_time;

        //If the current time is superior to the task's scheduling time :
        if (current_time > task_time) {

            //Mask the task as executable;
            service->executionFlag = true;

            //update the task's period;
            service->next_exec_time = (task_time += service->period);

        }

        //If the next time is null or greater than the task's scheduling time :
        if ((!next_time) || (task_time < next_time)) {

            //Update the next check time;
            next_time = task_time;

        }

    }

    //Update the next check time with the minimum of all execution times;
    next_check_time = next_time;

}

//----------------------------------- Timeline reset -----------------------------------

/*
 * reset_timeline : update all tasks execution time according to the new timeline;
 */

void service_reset_timeline(uint32_t diff) {

    //For each task :
    for (container_index_t service_index = services.nb_elements; service_index--;) {

        //Cache the task;
        service_t *service = *(service_t **)container_get_element(&services, service_index);

        if (service->next_exec_time < diff) {

            //Minor the execution time;
            service->next_exec_time = 0;

        } else {

            //Subtract the difference;
            service->next_exec_time -= diff;

        }

    }

    //Plan an executability check;
    next_check_time = 0;

}
