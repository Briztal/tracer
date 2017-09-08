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

class TerminalInterfaceCommands {

#define TI_STD_ARGS char *dptr, uint8_t size


    //-----------------------------------------------Custom functions---------------------------------------------------

public:

#define GO_UPPER

#define GO_LOWER(...)

#define CREATE_LEAF(i, name, ...)\
    static bool name(char * dptr, uint8_t size);

#define CREATE_CALLABLE_LEAF(i, name, ...)\
    CREATE_LEAF(i, name)

#include "terminal_interface_config.h"

#undef GO_UPPER
#undef GO_LOWER
#undef CREATE_LEAF
#undef CREATE_CALLABLE_LEAF

#endif

};


#endif //TRACER_TREEINTERFACECOMMANDS_H
