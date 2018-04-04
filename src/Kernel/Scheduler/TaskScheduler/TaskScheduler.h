/*
  TaskScheduler.h - Part of TRACER

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


#ifndef PROJECT_SYSTEM_H
#define PROJECT_SYSTEM_H

#include <Config/kernel_config.h>

#include <DataStructures/Containers/Queue.h>

#include <Kernel/Interaction/CommunicationPipe.h>

#include "_task_scheduler_data.h"

//All files that include this library may require the generation of a scheduler.
#include "scheduler_generation.h"

//The file scheduler_safe_use.txt contains documentation and help about the scheduler. I Hope it will help you.

namespace TaskScheduler {

    //-------------------------------------------- Parsing - execution cycle -------------------------------------------

    //Initialise the scheduler in a safe state;
    void initialise_data();


    //-------------------------------------------- Flood -------------------------------------------

    //Flood the task array;
    void flood();


    //-------------------------------------------------- State getters -------------------------------------------------

    //Returns the number of type-[type] tasks that can be scheduled;
    const uint8_t available_spaces();

    //Verify that nb_tasks can be scheduled;
    bool verify_schedulability(uint8_t nb_tasks);


    //------------------------------------------- Task scheduling - execution ------------------------------------------

    //Schedule a task;
    bool schedule_task(task_t *task);

    //Build and schedule a task;
    bool schedule_task(task_state_t (*f)(void *), void *args);


    //------------------------------------------ Pool and Sequences processing -----------------------------------------

    //Clear the task pool and all task sequences;
    void clear();



};


#endif //PROJECT_SYSTEM_H
