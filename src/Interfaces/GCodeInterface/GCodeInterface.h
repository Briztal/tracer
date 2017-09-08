/*
  GCodeExecuter.h - Part of TRACER

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

#include "../../config.h"
#ifdef ENABLE_GCODE_INTERFACE

#ifndef GCODEINTERFACE
#define GCODEINTERFACE

#include "../../hardware_language_abstraction.h"
#if !defined(HL_SERIAL)
#error CommandInterface requires Serial. If your board and language supports those, enable them in file "hardware_language_abstraction.h"
#endif


#include "GCommand.h"
#include "../../config.h"
#include "gcode_interface_config.h"


#define GI GCodeInterface

class GCodeInterface {


private :

    //--------------------------------------Serial Read Fields--------------------------------------

private :

    static unsigned char command_size;
    static char *data_in;
    static char *const data_in_0;

    //--------------------------------------Parsing Fields--------------------------------------

private :

    static float *const curve_t;
    static unsigned char curve_width;
    static unsigned char curve_height;

    static unsigned char motion_size, curve_points_column, curve_points_row, free_row_cases, free_rows;

    static unsigned char *const axis;
    static float *const coords;
    static float *const parameters;
    static bool *const v_parameters;



    //--------------------------------------Parsing Args----------------------------------

private :

    static bool get_parameter(char *command, float *value);

    static void init_parsing();

    static bool analyse_parameter(char c, float value);

    static bool parse();

    static void clean_parsing();

    static void reset();

    static unsigned char get_command(char *command);

    static void execute(char *command, unsigned char command_size);

    //------------------------------------Standard functions-----------------------------

public :

    static void echo(const String msg);

    static void send_position(float*){}


    static void begin();

    static void read_serial();
};

#endif //CODE_GCodeExecuter_H
#endif