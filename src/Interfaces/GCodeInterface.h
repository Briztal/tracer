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

#ifndef CODE_GCODE_EXECUTER_H
#define CODE_GCODE_EXECUTER_H


#include <WString.h>
#include "..//ActionController.h"
#include "../Machine/GCommand.h"



class GCodeExecuter {

public:

    static void begin();

    static bool available() {
        return (bool)empty_strings;
    }

    static void execute();

    static void step_process();


    static void save_gcode(unsigned char gcode_size);

private:

    //String container
    static char *storage;

    static unsigned char *strings_sizes;

    static int pull_index;

    static int push_index;

    static int available_strings;

    static int empty_strings;

    static volatile bool empty;

    static bool pull();

    //Current current_gcode to analyse;

    static bool gcode_processed;

    static char *current_gcode;


    static int available_space;
    static int available_commands;
    static int addIndice;
    static int readIndice;

    static g_command *command;

//--------------------------------------Parsing Fields----------------------------------

    static g_command *commands;


    static float *const curve_t;
    static unsigned char *const curve_width;
    static unsigned char *const curve_height;

    static unsigned int gcode_size, remain;
    static unsigned char cursor, coords_indice, curve_points_column, curve_points_row, free_row_cases, free_rows;

    static bool* axis_set;

    static unsigned char parsing_step;

    //--------------------------------------Parsing Args----------------------------------

    static void startCommand();

    static bool get_next_word(char *command, float *value);

    static bool init_parsing();

    static bool analyse(char c, float value);

    static bool get_type(char c, int value);

    static bool step_parse();

    static void finish_step_parse();

    static void parsing_interrupt_f();

};

#endif //CODE_GCodeExecuter_H
