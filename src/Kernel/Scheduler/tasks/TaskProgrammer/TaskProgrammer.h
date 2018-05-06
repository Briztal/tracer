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

#include <DataStructures/Containers/DynamicPointerBuffer.h>
#include "stdint.h"
#include "ProgrammedTask.h"

namespace TaskProgrammer {


    //----------------------------------- Initialisation -----------------------------------

    //Initialise task queues in a safe state;
    static void reset();


    //----------------------------------- Builders -----------------------------------

    //Program a one shot task;
    static void program_punctual_task(task_function_t task, uint32_t offset);

    //Program a repetitive task;
    static void program_repetitive_task(task_function_t task, uint32_t offset, uint32_t period, uint32_t nb_schedulings);

    //Program a repetitive task with offset equal to the period;
    static void program_repetitive_task(task_function_t task, uint32_t period, uint32_t nb_schedulings);

    //Program an infinite task;
    static void program_infinite_task(task_function_t task, uint32_t offset, uint32_t period);

    //Program an infinite task with offset equal to the period;
    static void program_infinite_task(task_function_t task, uint32_t period);

    //Un-program a task;
    static void deprogram_task(task_function_t task);


    //----------------------------------- Execution -----------------------------------

    //Schedule tasks according to the current time (provided by kernel)
    static void process_tasks();


};


#endif //TRACER_TASKPROGRAMMER_H
