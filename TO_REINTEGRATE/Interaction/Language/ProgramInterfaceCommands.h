/*
  TreeInterfaceCommands.h - Part of TRACER

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

#include <config.h>
#ifdef ENABLE_PROGRAM_INTERFACE

#ifndef TRACER_TREEINTERFACECOMMANDS_H
#define TRACER_TREEINTERFACECOMMANDS_H


#include <stdint.h>
#include <TaskScheduler/task_state_t.h>
#include <Project/InterfaceCommands/_interface_data.h>

class ProgramInterfaceCommands {


    //The system canal
public:

    static task_state_t system_canal_function(char *command, uint8_t size);

    static task_state_t parameters_system_canal(char *command, uint8_t size);

    static task_state_t pid_system_canal(char *command, uint8_t size);

    static task_state_t loop_system_canal(char *command, uint8_t size);

    static task_state_t actions_system_canal(char *command, uint8_t size);

    static task_state_t steppers_system_canal(char *command, uint8_t size);

    static task_state_t EEPROM_system_canal(char *command, uint8_t size);



    //User methods


#define GO_UPPER

#define GO_LOWER(i, name)

#define CREATE_LEAF(i, name, ...)\
    /*
     * First, we will declare the method you will actually implement, that takes the index of the data in the
     * data storage.
     * You have 3 return codes you can use :
     *  - 0 : not enough space in the task pool. Task will be reprogrammed automatically.
     *  - 1 : argument_t error. A message with the correct syntax will be displayed.
     *  - 2 and upper. : correct computation.
     */\
    static task_state_t name(uint8_t arguments_index);\
    /*
     * Then, we will declare and implement a pre_processor that eases your work.
     *   It starts by calling your implementation, passing only the argument_t index.
     *   If your command does not succeeds, it logs the syntax informato
     * data storage.
     */\
    static task_state_t _##name(void *ptr) {\
        program_interface_data_t *data = (program_interface_data_t*) ptr;\
        task_state_t b = name(data->arguments_index);\
        /*remove the data, and do not reprogram the task*/\
        if (b == complete) {ProgramInterface::validate_task(data->arguments_index);}\
        return b;\
    }

#define CREATE_CALLABLE_LEAF(i, name)\
    CREATE_LEAF(i, name)

#include <Config/program_interface_config.h>

#undef GO_UPPER
#undef GO_LOWER
#undef CREATE_LEAF
#undef CREATE_CALLABLE_LEAF




};


#endif //TRACER_TREEINTERFACECOMMANDS_H


#endif
