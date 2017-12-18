/*
  GCodeTreeGenerator.h - Part of TRACER

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


#ifndef TRACER_GCODETREEGENERATOR_H
#define TRACER_GCODETREEGENERATOR_H

#include <Config/control_config.h>

#ifdef ENABLE_GCODE_INTERFACE

#include "GCodeTree.h"
#include "GCodeTree.h"
#include "Config/gcode_interface_config.h"

typedef struct command_line_t {

    char name[GCODE_MAX_DEPTH + 1];

    task_state_t (*function)(char *);

} command_line_t;


typedef enum comp_result_t {
    greater, smaller, equal
} comp_result_t;


class GCodeTreeGenerator {


public:

    static GCodeTree *generate_tree();

    static void print_tree(const GCodeTree *tree);

private:

//TODO COMMENT

    //------------------------------------- Command array -------------------------------------

private:

    //Command array builder
    static command_line_t **build_commands(uint16_t *nb_commands_p);

    //Command array free;
    static void free_commands(command_line_t **commands, const uint16_t nb_commands);


    static uint16_t get_command_nb();

    static bool check_prefix_presence(const char *string, const char *prefix);

    static bool count_children(command_line_t **commands, uint16_t nb_commands, uint16_t line_number, char *prefix,
                               uint8_t *nb_children);

    static GCodeTree *
    generate_tree(command_line_t **commands, uint16_t nb_commands, uint16_t *current_line, uint8_t index);


    static void sort_commands(command_line_t **commands, uint16_t nb_commands);

    static comp_result_t command_comparison(char *command_0, char *command_1);

    static void switch_commands(command_line_t **commands, uint16_t index_0, uint16_t index_1);

    static void save_command_name(command_line_t *command, const char *name);


};

#endif


#endif //TRACER_GCODETREEGENERATOR_H

