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

class TerminalInterfaceCommands {

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


//TODO MAKE NEW MACROS, FOR PARSING ALL ARGUMENTS (WITH IDENTIFIERS), CHECK IDENTIFIERS PRESENCE, AND GET AN ARGUMENT
//Below is a macro that simplifies the recuperation of argument pointer and size.
//#define GET_ARGS(index, ptr) char *(ptr) = TerminalInterface::get_arguments(index);

//#define GET_NB_WORDS(ptr) StringUtils::get_nb_words((ptr))

//#define GET_NEXT_WORD(args) args += StringUtils::get_next_word(args, word_buffer, MAX_WORD_SIZE);

#endif //TRACER_TREEINTERFACECOMMANDS_H
