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

#include <config.h>
#ifdef ENABLE_GCODE_INTERFACE


#include <TaskScheduler/TaskScheduler.h>

class GCodeInterfaceCommands {

#ifdef ENABLE_GCODE_INTERFACE

public :

#define GO_UPPER()

#define GO_LOWER(i)

#define COMMAND(i, name)\
    static task_state_t name(void *);

#define GO_LOWER_COMMAND(i, fname) COMMAND(i, fname)


#include <Project/Config/gcode_interface_config.h>

#undef GO_UPPER
#undef GO_LOWER
#undef GO_LOWER_COMMAND
#undef COMMAND

#endif
};


#endif //TRACER_GCODEINTERFACECOMMANDS_H
