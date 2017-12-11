//
// Created by root on 10/12/17.
//

#ifndef TRACER_GCODETREEGENERATOR_H
#define TRACER_GCODETREEGENERATOR_H


#include "GCodeTree.h"
#include "GCodeTree.h"
#include "Project/Config/gcode_interface_config.h"

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


#endif //TRACER_GCODETREEGENERATOR_H
