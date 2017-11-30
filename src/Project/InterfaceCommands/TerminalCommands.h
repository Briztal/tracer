/*
  TerminalInterfaceCommands.h - Part of TRACER

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

#ifdef ENABLE_TERMINAL_INTERFACE


#ifndef TRACER_TREEINTERFACECOMMANDS_H
#define TRACER_TREEINTERFACECOMMANDS_H


#include <stdint.h>
#include "_interface_data.h"
#include "interface.h"
#include <TaskScheduler/TaskScheduler.h>

class TerminalCommands {

    //-----------------------------------------------Custom functions---------------------------------------------------

public:

#define GO_UPPER(...)

#define GO_LOWER(...)

#define CREATE_LEAF(i, name, ...)\
    /*
     * First, we will declare the method you will actually implement, that takes the index of the arguments in the
     * arguments storage.
     * You have 3 return codes you can use :
     *  - 0 : not enough space in the task pool. Task will be reprogrammed automatically.
     *  - 1 : argument error. A message with the correct syntax will be displayed.
     *  - 2 and upper. : correct execution.
     */\
    static task_state_t name(uint8_t arguments_index);\
    /*
     * Then, we will declare and implement a pre_processor that eases your work.
     *   It starts by calling your implementation, passing only the argument index.
     *   If your command does not succeeds, it logs the syntax informato
     * arguments storage.
     */\
    static task_state_t _##name(void *ptr) {\
        terminal_interface_data_t *data = (terminal_interface_data_t*) ptr;\
        task_state_t b = name(data->arguments_index);\
        if (b == invalid_arguments) {\
            TerminalInterface::log_tree_style(data->node, true);\
        }\
        /*remove the arguments, and do not reprogram the task*/\
        if (b == complete) {TerminalInterface::validate_task(data->arguments_index);}\
        return b;\
    }

//TODO REMOVE CETTE MERDE ET FAIRE UN DISPATCHEUR GLOBAL !!


#define CREATE_CALLABLE_LEAF(i, name, ...)\
    CREATE_LEAF(i, name)

#include <Project/Config/terminal_interface_config.h>

#undef GO_UPPER
#undef GO_LOWER
#undef CREATE_LEAF
#undef CREATE_CALLABLE_LEAF

#endif

private:

    static char *const word_buffer;

};


#define PARSE_ARGUMENTS(index) TerminalInterface::parse_arguments(index);

#define NB_ARGUMENTS (TerminalInterface::get_nb_arguments())

#define REQUIRE_ALL_ARGUMENTS(arg_string) {if (!TerminalInterface::verify_all_identifiers_presence(arg_string)) return invalid_arguments;}

#define REQUIRE_ONE_ARGUMENTS(arg_string) {if (!TerminalInterface::verify_one_identifiers_presence(arg_string)) return invalid_arguments;}

#define CHECK_ARGUMENT(identifier) (TerminalInterface::verify_identifier_presence(identifier))

#define GET_ARG(identifier) TerminalInterface::get_argument(identifier)

#define GET_ARG_VALUE(identifier) TerminalInterface::get_argument_value(identifier)


#endif //TRACER_TREEINTERFACECOMMANDS_H
