/*
  Core.cpp - Part of TRACER

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
  along with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "Core.h"
#include "../Interfaces/CommandInterface.h"
#include "MachineControllerSystem.h"
#include "../config.h"
#include "EEPROMStorage.h"

#ifdef ENABLE_STEPPER_CONTROL
#include "../StepperControl/StepperController.h"
#include "../StepperControl/MotionScheduler.h"
#include "../StepperControl/MotionExecuter.h"
#endif


void Core::begin() {

#ifdef ENABLE_LOGGER
    CommandInterface::begin();
#endif

#ifdef ENABLE_STEPPER_CONTROL
    EEPROMStorage::begin();
    StepperController::begin();
    MotionScheduler::begin();
    MotionExecuter::start();
#endif

}

#define PROCESS_INTERNAL_TASKS while(internal_tasks.available_elements()) {process_internal_task();}

#define PROCESS_EXTERNAL_TASKS\
        while(external_tasks.available_elements()){PROCESS_INTERNAL_TASKS;process_external_task();}

void Core::run() {
    //Loop will run all permanent, with temporary tasks in between.

#define ADD_PERMANENT_TASK(function_name) \
        PROCESS_EXTERNAL_TASKS\
        PROCESS_INTERNAL_TASKS\
        function_name();\

#include "../config.h"

#undef ADD_PERMANENT_TASK

}



bool Core::add_external_task(void (*f)(char *, unsigned char), char *args, unsigned char size) {
    if (!external_tasks.available_spaces()) {
        return false;
    }

    //Get the push indice for args
    const unsigned char push_indice = external_tasks.push_indice();


    //Push the task
    external_tasks.push(f);


    //Get the args pointer
    char *ptr = external_args+push_indice*(PACKET_SIZE+1);

    //Set the size
    *(ptr++) = size;

    //Copy args
    memcpy(ptr, args, size);


    return true;
}


bool Core::process_external_task() {
    if (!external_tasks.available_elements()) {
        return false;
    }

    //Get the pull indice for args
    const unsigned char pull_indice = external_tasks.pull_indice();

    //Get the task
    void (*f)(char *, unsigned char) = external_tasks.pull();


    //Get the args pointer
    char *ptr = external_args+pull_indice*(PACKET_SIZE+1);

    //Get the args size
    unsigned char size = (unsigned char) *(ptr++);

    //Executing the task;
    (*f)(ptr, size);

    return true;

}


bool Core::add_internal_task(void (*f)()) {
    if (!internal_tasks.available_spaces()) {
        return false;
    }

    internal_tasks.push(f);

    return true;
}


bool Core::process_internal_task() {
    if (!internal_tasks.available_elements()) {
        return false;
    }

    //Executing the task;
    (*(internal_tasks.pull()))();

    return true;

}

Queue<void(*)(char *, unsigned char)> Core::external_tasks(MAX_TASKS);

Queue<void(*)()> Core::internal_tasks(MAX_TASKS);
char args_tab[MAX_TASKS][PACKET_SIZE+1];
char *Core::external_args = (char *) args_tab;


template class Queue<motion_data>;
template class Queue<linear_data>;
template class Queue<int>;
template class Queue<void(*)(char *args, unsigned char args_size)>;
template class Queue<void(*)()>;

