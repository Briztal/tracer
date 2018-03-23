/*
  ProgrammedTask.h - Part of TRACER

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

#ifndef TRACER_PROGRAMMEDTASK_H
#define TRACER_PROGRAMMEDTASK_H

#include <Kernel/Scheduler/TaskScheduler/task_state_t.h>

#include <cstdint>


typedef task_state_t (*task_pointer_t)(void *);

class ProgrammedTask {

    //------------------------------- Initialisation -------------------------------

public:

    //Constructor;
    ProgrammedTask(task_pointer_t task, uint32_t offset, uint32_t period, uint32_t nb_scheduling);

    //Destructor;
    ~ProgrammedTask() = default;


    //------------------------------- Fields -------------------------------

public:

    //A flag, set if the task must be executed;
    bool isSchedulable;

    //The task itself;
    task_pointer_t task;

    //The time at which the next execution is supposed to happen;
    uint32_t nextExecutionTime;

    //The period of the task;
    uint32_t period;

    //The remaining number of time the function must be scheduled;
    uint32_t remainingSchedulings;

};


#endif //TRACER_PROGRAMMEDTASK_H
