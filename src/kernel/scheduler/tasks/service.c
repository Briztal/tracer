/*
  service.c - Part of TRACER

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

#include <data_structures/containers/non_concurrent/vlarray.h>
#include <kernel/kernel.h>
#include <data_structures/containers/non_concurrent/llist.h>


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
void _service_add(void (*service_f)(void *), uint32_t offset, uint32_t period, uint32_t remaining_execs
        , uint16_t activity_time);

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
        kernel_error("service.c : services_initialise : the service manager is already initialised;");

        //Never reached;
        return;

    }

    //Create an initializer for the linked list;
    linked_list_t init = EMPTY_LINKED_LIST(max_nb_services);

    //Allocate some heap data for the list and copy the initializer;
    pending_services = kernel_malloc_copy(sizeof(linked_list_t), &init);

    //Mark the service maanager initialised;
    services_initialised = true;

}

//----------------------------------- Builders -----------------------------------


/*
 * program_repetitive_task : this function adds a temporary service task;
 */

void service_add_temporary(void (*task_function)(void *), uint32_t offset, uint32_t period, uint32_t nb_execs,
                           uint16_t activit_time) {


    //If the service manager has not been scheduler_initialised :
    if (!services_initialised) {

        //Error, service manager must be initialised before usage;
        kernel_error("service.c : service_add_temporary : the service manager was not initialised;");

        //Never reached;
        return;

    }

    //If the service mustn't be scheduled, nothing to do;
    if (!nb_execs) {

        //Ignore;
        return;

    }

    //Program the task
    _service_add(task_function, offset, period, nb_execs, activit_time);

}


/*
 * service_add_permanent : adds a permanent service;
 */

void service_add_permanent(void (*task_function)(void *), uint32_t offset, uint32_t period,
                           uint16_t activity_time) {

    //If the service manager has not been scheduler_initialised :
    if (!services_initialised) {

        //Error, service manager must be initialised before usage;
        kernel_error("service.c : service_add_permanent : the service manager was not initialised;");

        //Never reached;
        return;

    }

    //If the service manager has not been scheduler_initialised :
    if (!services_initialised) {

        kernel_halt(30);

        //Error, service manager must be initialised before usage;
        return;//TODO ERROR

    }

    //Program the task;
    _service_add(task_function, offset, period, 0, activity_time);

}


/*
 * _program_task : programs a task with the given set of parameters;
 */

void _service_add(void (*service_f)(void *), uint32_t offset, uint32_t period, uint32_t remaining_execs,
                  uint16_t activity_time) {


    //If the task is null :
    if (service_f == 0) {

        kernel_error("service.c : _service_add : provided a null function pointer;");

        //Ignore;
        return;

    }

    //If the period is null :
    if (!period) {

        kernel_error("service.c : _service_add : provided a null period;");

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
                    .task_type = SERVICE_STASK,
                    .activity_time = activity_time,
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

    //If the service manager has not been scheduler_initialised :
    if (!services_initialised) {

        //Error, service manager must be initialised before usage;
        kernel_error("service.c : services_insert : the service manager was not initialised;");

        //Never reached;
        return;

    }

    //Cache the next computation time;
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
            llist_insert_before(pending_services, (llist_t *) element, (llist_t *) service);

            //Go to the end of the function;
            goto inserted;

        }

        //Update the element;
        element = (service_t *) ((llist_t *)element)->next;

    }


    //If we reached the end of the list : the element must be inserted at the end;
    llist_insert_last(pending_services, (llist_t *) service);

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

        //Error, service manager must be initialised before usage;
        kernel_error("service.c : services_available_task : the service manager was not initialised;");

        //Never reached;
        return false;

    }

    //The access to the list is critical;//TODO CRITICAL PATCH;
    kernel_enter_critical_section();

    //Cache the number of pending services;
    size_t nb_elements = pending_services->nb_elements;

    //Leave the critical section;
    kernel_leave_critical_section();

    //If the services list is empty, no services available;
    if (!nb_elements) {

        return false;


    }

    //Cache the timeline;
    uint32_t millis = systick_milliseconds();

    //Services are stored by increasing computation time;


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

stask_t *services_get_task() {

    //TODO ERREUR POTENTIELLE ICI !!!!!!!!!
    //TODO ERREUR POTENTIELLE ICI !!!!!!!!!
    //TODO ERREUR POTENTIELLE ICI !!!!!!!!!
    //TODO ERREUR POTENTIELLE ICI !!!!!!!!!
    //TODO ERREUR POTENTIELLE ICI !!!!!!!!!
    //TODO ERREUR POTENTIELLE ICI !!!!!!!!!
    //TODO ERREUR POTENTIELLE ICI !!!!!!!!!
    //TODO ERREUR POTENTIELLE ICI !!!!!!!!!
    //TODO ERREUR POTENTIELLE ICI !!!!!!!!!
    //TODO ERREUR POTENTIELLE ICI !!!!!!!!!
    //TODO ERREUR POTENTIELLE ICI !!!!!!!!!
    //TODO ERREUR POTENTIELLE ICI !!!!!!!!!
    //TODO ERREUR POTENTIELLE ICI !!!!!!!!!
    //TODO ERREUR POTENTIELLE ICI !!!!!!!!!
    //TODO ERREUR POTENTIELLE ICI !!!!!!!!!
    //TODO ERREUR POTENTIELLE ICI !!!!!!!!!
    //TODO ERREUR POTENTIELLE ICI !!!!!!!!!
    //TODO ERREUR POTENTIELLE ICI !!!!!!!!!

    //If the service manager has not been scheduler_initialised :
    if (!services_initialised) {

        //Error, service manager must be initialised before usage;
        kernel_error("service.c : services_get_task : the service manager was not initialised;");

        //Never reached;
        return false;

    }

    //The access to the list is critical;//TODO CRITICAL PATCH;
    kernel_enter_critical_section();

    //Cache the number of pending services;
    size_t nb_elements = pending_services->nb_elements;

    //Leave the critical section;
    kernel_leave_critical_section();

    //If the services list is empty, no services available; This shouldn't have happened;
    if (!nb_elements) {

        //Error, no services can be executed;
        kernel_error("service.c : services_get_task : no services have been registered. Shouldn't have happened;");

    }

    //Cache the timeline;
    uint32_t millis = systick_milliseconds();

    //Services are stored by increasing computation time;


    //The access to the list is critical;//TODO CRITICAL PATCH;
    kernel_enter_critical_section();

    //Services are available if and only if the first service has to be executed;
    service_t * service = (service_t *) (pending_services->first);

    //Leave the critical section;
    kernel_leave_critical_section();


    //If we haven't yet reached the trigger time :
    if (millis < service->next_exec_time) {

        //Error, no services can be executed;
        kernel_error("service.c : services_get_task : no services can be executed. Shouldn't have happened;");

    }

    //The first service exists and can be executed;

    //Remove it;
    llist_remove_first(pending_services);

    //Return the first service of the list;
    return (stask_t *)service;

}


/*
 * services_reprogram : reprograms an executed service if necessary, or delete it if its lifetime ended;
 */

void services_reprogram(service_t *service) {

    //If the service manager has not been scheduler_initialised :
    if (!services_initialised) {

        //Error, service manager must be initialised before usage;
        kernel_error("service.c : services_reprogram : the service manager was not initialised;");

        //Never reached;
        return;

    }


    //If the service is not permanent, and has reached the end of its lifetime :
    if (service->remaining_execs == 1) {

        //Delete the service;
        stask_delete((stask_t *) service);

    } else {
        //If the service must be reprogrammed :

        //Cache the timeline;
        uint32_t millis = systick_milliseconds();

        //Cache the previous computation time, and the period;
        uint32_t period = service->period, next_exec = service->next_exec_time + period;


        //TODO CONDITION. SET A VARIABLE TO DETERMINE STRICTNESS;

        //If the computation is not strict, determine the next time the service must be executed;
        while(next_exec < millis) next_exec += period;

        //Update the next computation time;
        service->next_exec_time = next_exec;

        //Re-insert the service in the pending services queue;
        services_insert(service);

    }

}
