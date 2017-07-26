/*
  MachineControllerSystem.h - Part of TRACER

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

#ifndef CODE_GCODEPARSER_H
#define CODE_GCODEPARSER_H

#include <stdint.h>
#include "../config.h"

class MachineController {


#ifdef ENABLE_COMMAND_INTERFACE

public:

    static void system_canal_function(char *dptr, uint8_t size);

#define GO_UPPER

#define GO_LOWER(i, name)

#define CREATE_LEAF(i, name)\
    static void name(char * dptr, uint8_t size);

#define CREATE_CALLABLE_LEAF(i, name)\
    CREATE_LEAF(i, name)

#include "../Interfaces/CommandInterface/command_interface_config.h"

#undef GO_UPPER
#undef GO_LOWER
#undef CREATE_LEAF
#undef CREATE_CALLABLE_LEAF


private:

    static void parameters_system_canal(char *dptr, uint8_t size);

    static void pid_system_canal(char *dptr, uint8_t size);

    static void loop_system_canal(char *dptr, uint8_t size);

    static void actions_system_canal(char *dptr, uint8_t size);

    static void steppers_system_canal(char *dptr, uint8_t size);

    static void EEPROM_system_canal(char *dptr, uint8_t size);

#endif


#ifdef ENABLE_GCODE_INTERFACE

public :

#define GO_UPPER()

#define GO_LOWER(i)

#define COMMAND(i, name)\
    static void name();

#define GO_LOWER_COMMAND(i, fname) COMMAND(i, fname)


#include "../Interfaces/GCodeInterface/gcode_interface_config.h"

#undef GO_UPPER
#undef GO_LOWER
#undef GO_LOWER_COMMAND
#undef COMMAND

#endif

};


#endif //CODE_GCODEPARSER_H
