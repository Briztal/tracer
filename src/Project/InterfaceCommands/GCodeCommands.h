/*
  GCodeInterfaceCommands.h - Part of TRACER

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

#include <Config/control_config.h>

#ifdef ENABLE_GCODE_INTERFACE

#include <Control/Controllers/GCode/GCodeArguments.h>
#include <TaskScheduler/TaskScheduler.h>


class GCodeCommands {


    /*
     * Here is declared the method you will actually implement, that takes the index of the arguments in the
     * arguments storage.
     */

public :


#define GCODE_COMMAND(command, name)\
    static task_state_t name(char *);

#include <Config/gcode_interface_config.h>

#undef GCODE_COMMAND



//TODO COMMENT AND COMMENT TERMINALCOMMANDS

#define PARSE_ARGUMENTS(arguments) {if (!GCodeArguments::parse_arguments(arguments)) return invalid_arguments;}

#define REQUIRE_ALL_ARGUMENTS(arg_string) {if (!GCodeArguments::verify_all_identifiers_presence(arg_string)) return invalid_arguments;}

#define REQUIRE_ONE_ARGUMENTS(arg_string) {if (!GCodeArguments::verify_one_identifiers_presence(arg_string)) return invalid_arguments;}

#define CHECK_ARGUMENT(identifier) (GCodeArguments::verify_identifier_presence(identifier))

#define GET_ARG(identifier) GCodeArguments::get_argument(identifier)

#define GET_ARG_VALUE(identifier) GCodeArguments::get_argument_value(identifier)
};


#endif //TRACER_GCODEINTERFACECOMMANDS_H
