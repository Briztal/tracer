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

#include "service.h"

#include <kernel/systick.h>

#include <data_structures/containers/container.h>
#include <kernel/kernel.h>
#include <data_structures/containers/llist.h>


//TODO MAKE THE SCHEDULER LIBRARY
//TODO MAKE THE SCHEDULER LIBRARY
//TODO MAKE THE SCHEDULER LIBRARY
//TODO MAKE THE SCHEDULER LIBRARY
//TODO MAKE THE SCHEDULER LIBRARY
//TODO MAKE THE SCHEDULER LIBRARY
//TODO MAKE THE SCHEDULER LIBRARY
//TODO MAKE THE SCHEDULER LIBRARY
//TODO MAKE THE SCHEDULER LIBRARY
//TODO MAKE THE SCHEDULER LIBRARY
//TODO MAKE THE SCHEDULER LIBRARY
//TODO MAKE THE SCHEDULER LIBRARY
//TODO MAKE THE SCHEDULER LIBRARY
//TODO MAKE THE SCHEDULER LIBRARY
//TODO MAKE THE SCHEDULER LIBRARY
//TODO MAKE THE SCHEDULER LIBRARY
//TODO MAKE THE SCHEDULER LIBRARY
//TODO MAKE THE SCHEDULER LIBRARY
//TODO MAKE THE SCHEDULER LIBRARY
//TODO MAKE THE SCHEDULER LIBRARY
//TODO MAKE THE SCHEDULER LIBRARY
//TODO MAKE THE SCHEDULER LIBRARY
//TODO MAKE THE SCHEDULER LIBRARY
//TODO MAKE THE SCHEDULER LIBRARY


//----------------------------------- Builder -----------------------------------

//Add a service;
void _service_add(void (*service_f)(void *), uint32_t offset, uint32_t period, uint32_t remaining_execs);

//Insert a service in the queue;
void services_insert(service_t *service);


//----------------------------------- Fields -----------------------------------

/*
 * The service manager starts in the uninitialised sequences_initialised,
 * then becomes scheduler_initialised, and is finally started;
 */
bool services_initialised = false;

//Programmed tasks;
linked_list_t *pending_services;


//----------------------------------- Initialisation -----------------------------------

/*
 * services_initialise : initialises the service linked list;
 */

void services_initialise(size_t max_nb_services) {

    //If the service manager has already been scheduler_initialised :
    if (services_initialised) {

        //Error, must not be scheduler_initialised twice;
        return;//TODO ERROR

    }

}

//----------------------------------- Builders -----------------------------------


/*
 * program_repetitive_task : this function adds a temporary service task;
 */

void service_add_temporary(void (*task_function)(void *), uint32_t offset, uint32_t period, uint32_t nb_execs) {

    //If the service manager has not been scheduler_initialised :
    if (!services_initialised) {

        //Error, must be scheduler_initialised before usage;
        return;//TODO ERROR

    }

    //If the service mustn't be scheduled, nothing to do;
    if (!nb_execs) {

        //Ignore;
        return;

    }

    //Program the task
    _service_add(task_function, offset, period, nb_execs);

}


/*
 * service_add_permanent : adds a permanent service;
 */

void service_add_permanent(void (*task_function)(void *), uint32_t offset, uint32_t period) {

    //If the service manager has not been scheduler_initialised :
    if (!services_initialised) {

        //Error, must be scheduler_initialised before usage;
        return;//TODO ERROR

    }

    //Program the task;
    _service_add(task_function, offset, period, 0);

}


/*
 * _program_task : programs a task with the given set of parameters;
 */

void _service_add(void (*service_f)(void *), uint32_t offset, uint32_t period, uint32_t remaining_execs) {

    //If the task is null :
    if (service_f == 0) {

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

    //Create the service initializer;
    service_t init = {
            .task = {
                    .linked_element = EMPTY_LINKED_ELEMENT(),
                    .function = service_f,
                    .args = 0,
                    .cleanup = 0,
                    .task_type = SERVICE_TASK,
            },
            .next_exec_time = offset + systick_milliseconds(),
            .remaining_execs = remaining_execs,
            .period = period,
    };

    //Create and initialise the service in the heap;
    service_t *service = kernel_malloc_copy(sizeof(service_t), &init);

    //Insert the service in the pending services list;
    services_insert(service);

}


/*
 * service_insert : inserts a service in the pending ordered list;
 */

void services_insert(service_t *service) {

    //Cache the next execution time;
    uint32_t next_execution = service->next_exec_time;

    //The access to the list is critical;//TODO CRITICAL PATCH;
    kernel_enter_critical_section();

    //Cache the first element of the list;
    service_t *element = (service_t *) (pending_services->first);

    //While the element exists :
    while (element) {

        //If we have reached the first greater element :
        if (next_execution < element->next_exec_time) {

            //Insert the new service before the first greater element;
            llist_insert_before(pending_services, (linked_element_t *) element, (linked_element_t *) service);

            //Go to the end of the function;
            goto inserted;

        }

    }

    //If we reached the end of the list : the element must be inserted at the end;
    llist_insert_end(pending_services, (linked_element_t *) service);

    inserted:

    //Leave the critical section;
    kernel_leave_critical_section();

}


//----------------------------------- Scheduler interaction -----------------------------------

/*
 * services_available_task : this function asserts if services are available.
 */

bool services_available_task() {

    //If the service manager has not been scheduler_initialised :
    if (!services_initialised) {

        //Error, must be scheduler_initialised before usage;
        return false;//TODO ERROR

    }

    //The access to the list is critical;//TODO CRITICAL PATCH;
    kernel_enter_critical_section();

    //Cache the number of pending services;
    size_t nb_elements = pending_services->nb_elements;

    //Leave the critical section;
    kernel_leave_critical_section();

    //If the services list is empty, no services available;
    if (!nb_elements)
        return false;

    //Cache the timeline;
    uint32_t millis = systick_milliseconds();

    //Services are stored by increasing execution time;


    //The access to the list is critical;//TODO CRITICAL PATCH;
    kernel_enter_critical_section();

    //Services are available if and only if the first service has to be executed;
    service_t * service = (service_t *) (pending_services->first);

    //Leave the critical section;
    kernel_leave_critical_section();

    //Services are available if and only if the first service has to be executed;
    return service->next_exec_time < millis;

}


/*
 * services_get_task : get the first service of the list. Doesn't check if the service must be executed;
 */

task_t *services_get_task() {

    //If the service manager has not been scheduler_initialised :
    if (!services_initialised) {

        //Error, must be scheduler_initialised before usage;
        return 0;//TODO ERROR

    }
    //The access to the list is critical;//TODO CRITICAL PATCH;
    kernel_enter_critical_section();

    //Cache the number of pending services;
    size_t nb_elements = pending_services->nb_elements;

    //Leave the critical section;
    kernel_leave_critical_section();

    //If the services list is empty, no services available; This shouldn't have happened;
    if (!nb_elements)
        return 0;//TODO ERROR;


    //Cache the timeline;
    uint32_t millis = systick_milliseconds();

    //Services are stored by increasing execution time;


    //The access to the list is critical;//TODO CRITICAL PATCH;
    kernel_enter_critical_section();

    //Services are available if and only if the first service has to be executed;
    service_t * service = (service_t *) (pending_services->first);

    //Leave the critical section;
    kernel_leave_critical_section();

    if (service->next_exec_time < millis)
        return 0;//TODO ERROR, SERVICE MUSTN'T BE EXECUTED;

    //Return the first service of the list;
    return (task_t *)service;

}


/*
 * services_reprogram : reprograms an executed service if necessary, or delete it if its lifetime ended;
 */

void services_reprogram(service_t *service) {

    //If the service is not permanent, and has reached the end of its lifetime :
    if (service->remaining_execs == 1) {

        //Delete the service;
        task_delete((task_t *)service);

    } else {
        //If the service must be reprogrammed :

        //Cache the timeline;
        uint32_t millis = systick_milliseconds();

        //Cache the previous execution time, and the period;
        uint32_t period = service->period, next_exec = service->next_exec_time + period;


        //TODO CONDITION. SET A VARIABLE TO DETERMINE STRICTNESS;

        //If the execution is not strict, determine the next time the service must be executed;
        while(next_exec < millis) next_exec += period;

        //Update the next execution time;
        service->next_exec_time = next_exec;

        //Re-insert the service in the pending services queue;
        services_insert(service);

    }

}
