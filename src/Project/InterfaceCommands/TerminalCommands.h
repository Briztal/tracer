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
#include <Interfaces/_interface_data.h>
#include <Interfaces/Interfaces.h>
#include <TaskScheduler/TaskScheduler.h>

class TerminalCommands {

    //-----------------------------------------------Custom functions---------------------------------------------------

public:

    /*
     * Here is declared the method you will actually implement, that takes the index of the arguments in the
     * arguments storage.
     */

#define GO_UPPER(...)

#define GO_LOWER(...)

#define CREATE_LEAF(i, name, ...) static task_state_t name(char *arguments);

#define CREATE_CALLABLE_LEAF(i, name, ...) CREATE_LEAF(i, name)

#include <Project/Config/terminal_interface_config.h>

#undef GO_UPPER
#undef GO_LOWER
#undef CREATE_LEAF
#undef CREATE_CALLABLE_LEAF

#endif


};


//TODO COMMENT AND COMMENT GCODEINTERFACECOMMANDS
#define PARSE_ARGUMENTS(arguments) {if (!TerminalInterface::parse_arguments(arguments)) return invalid_arguments;}

#define REQUIRE_ALL_ARGUMENTS(arg_string) {if (!TerminalInterface::verify_all_identifiers_presence(arg_string)) return invalid_arguments;}

#define REQUIRE_ONE_ARGUMENTS(arg_string) {if (!TerminalInterface::verify_one_identifiers_presence(arg_string)) return invalid_arguments;}

#define CHECK_ARGUMENT(identifier) (TerminalInterface::verify_identifier_presence(identifier))

#define GET_ARG(identifier) TerminalInterface::get_argument(identifier)

#define GET_ARG_VALUE(identifier) TerminalInterface::get_argument_value(identifier)


#endif //TRACER_TREEINTERFACECOMMANDS_H
