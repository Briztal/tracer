/*
  GCodeExecuter.cpp - Part of TRACER

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


#include "GCodeInterface.h"
#include "Logger.h"
#include "GCommand.h"

void GCodeExecuter::begin() {
    Logger::set_data_pointer(storage + DATA_SIZE * push_index);
}

bool step_lock = false;

void GCodeExecuter::step_process() {

    //if we currently process, abort
    if (step_lock) return;

    if (!gcode_processed || (available_space && available_strings && pull())) {
        String s;
        for (int i = 0; i < gcode_size; i++) {
            s += current_gcode[i];
        }
        //Lock
        step_lock = true;

        command = commands + addIndice;

        unsigned char *axis = (*command).axis;
        for (int i = 0; i < NB_AXIS; i++, axis++) {
            *axis = NB_AXIS;
        }

        parsing_step = 0;
        MsTimer2::stop();
        MsTimer2::set(1, (void (*)()) parsing_interrupt_f);
        MsTimer2::start();
    }
}


/** This function updates the data pointer of the logger.
 * The Logger stores the received data in a char*
 * This method is called by the logger when he receives a GCode, to persist the GCode for process.
 */
void GCodeExecuter::save_gcode(unsigned char gcode_size) {

    //If there is an empty space
    if (empty_strings) {

        //If no string is present and if actual command is not present
        if (gcode_processed) {
            gcode_processed = false;
            current_gcode = storage + DATA_SIZE * push_index;
            GCodeExecuter::gcode_size = gcode_size;
            return;
        }

        //Set the saved current_gcode size
        strings_sizes[push_index] = gcode_size;

        //Storage indices update
        if (!push_index--)
            push_index = STORAGE_SIZE - 1;
        empty_strings--;
        available_strings++;

        //Update Logger's data_pointer to save the GCode command, with the new push_index value.
        Logger::set_data_pointer(storage + DATA_SIZE * push_index);

    }
}

//This command updates current_gcode (char*) at the address of the current command to process
bool GCodeExecuter::pull() {

    if (available_strings) {

        //current_gcode update
        current_gcode = storage + DATA_SIZE * pull_index;
        gcode_size = strings_sizes[pull_index];

        //Storage indices update
        if (!pull_index--)
            pull_index = STORAGE_SIZE - 1;
        gcode_processed = false;
        available_strings--;
        empty_strings++;
        return true;
    }
    return false;
}

bool lock = false;

void GCodeExecuter::parsing_interrupt_f() {
    if (lock) return;
    lock = true;
    if (!step_parse()) {
        Logger::runInterrupts();
    }

    lock = false;
}

bool GCodeExecuter::step_parse() {
    if (gcode_processed) return true;

    char c;
    float value;

    switch (parsing_step) {

        case 0:
            if (!init_parsing())
                return false;
            break;
        case 1:
            if (!get_next_word(&c, &value) || !(get_type(c, (int) value)))
                return false;
            break;
        case 2:
            while (cursor < gcode_size) {
                //Get next word and analyse, if it fails, return false;
                return get_next_word(&c, &value) && analyse(c, value);
            }
            break;
        case 3:
            finish_step_parse();
        default:
            break;
    }

    parsing_step++;
    return true;

}

bool GCodeExecuter::get_next_word(char *command, float *value) {
    do {
        if (!remain--) {
            return false;
        }
    } while ((*command = current_gcode[cursor++]) == ' ');

    char t;
    String strValue = "";
    while ((remain--) && ((t = current_gcode[cursor++]) != ' ')) {
        strValue += t;

    }
    if (!strValue.length()) {
        *value = 0;
    } else {
        *value = strValue.toFloat();
    }
    return true;
}

bool GCodeExecuter::get_type(char c, int value) {
    // COMMAND TYPE
    if (c == 'G') {
        command->command_type = 0;
        command->command_id = value;
        return true;
    } else if (c == 'M') {
        command->command_type = 1;
        command->command_id = value;
        return true;
    }
    return false;
}

bool GCodeExecuter::init_parsing() {

    cursor = 0;
    remain = gcode_size;

    if (gcode_size == 0)
        return false;

    //INIT :

    coords_indice = curve_points_column = curve_points_row = 0;
    free_row_cases = NB_AXIS - 1;
    free_rows = MAX_CURVE_POINTS - 1;

    command->command_type = -1;
    command->command_id = -1;
    command->speed_set = false;
    *curve_width = NB_AXIS;

    for (int p = 0; p < PARAMETERS_NB; p++)
        command->v_parameters[p] = false;


    for (int axis = 0; axis < NB_AXIS; axis++) {
        axis_set[axis] = false;
    }

    return true;
}


bool GCodeExecuter::analyse(char c, float value) {

    switch (c) {

//-----------------------AXIS_COORDINATES-------------------

#define AXIS_TREATMENT(i, j, si, st, sp, a, d, pinStep, pd, pp, pmi, vi, pma, va) \
            case j:\
                if (axis_set[i])\
                    return false;\
                *(command->coords+coords_indice) = (long) (value * SettingsProfileN::steps[i]);\
                *(command->axis + coords_indice++) = i;\
                axis_set[i] = true;\
                break;

#include "../config.h"

#undef AXIS_TREATMENT

//-----------------------CURVE PARAMETERS-------------------
        case 'D' ://CURVE_POINT
            *(curve_t + NB_AXIS * curve_points_row + curve_points_column) = value;
            if (!free_row_cases)
                return false;
            curve_points_column++;
            free_row_cases--;
            break;

        case 'N' : //Carriage return in Curve points
            if (!curve_points_column)
                break;
            if (!free_rows)
                return false;
            curve_points_row++;
            if (*curve_width > curve_points_column)
                *curve_width = curve_points_column;
            curve_points_column = 0;
            free_rows--;
            free_row_cases = NB_AXIS - 1;
            break;

//-----------------------SPEED AND PARAMETERS-------------------

        case 'F':
            command->speed = value;
            command->speed_set = true;
            break;

            //TODO INTEGRER LES ACTIONS

#define GPARAMETER(i, j, k)\
            case j : \
                if (!*(command->v_parameters+i)){\
                    *(command->parameters+i) = value;\
                    *(command->v_parameters+i) = true;\
                }\
                break;

#include "../parser_config.h"

#undef GPARAMETER

        default:
            return false;
    }

    return true;
}


void GCodeExecuter::finish_step_parse() {
    if (curve_points_column) {
        if (*curve_width > curve_points_column)
            *curve_width = curve_points_column;
    } else
        curve_points_row--;

    *curve_height = ++curve_points_row;
    command->motion_size = coords_indice;

    //TODO GAFFE A CA, PAS PARAMETRABLE. RAJOUTER DES MACROS DANS GCODESYNTAX
    if (command->command_type == 0 && command->command_id == 1) {
        Logger::echo("pos_set");
        Motion::set_end_position(command->coords, command->axis, coords_indice);
    }

    available_space--;
    available_commands++;
    addIndice++;
    if (addIndice == COMMAND_NB)
        addIndice = 0;

    Logger::runInterrupts();
    gcode_processed = true;
    step_lock = false;

}

void GCodeExecuter::execute() {
    if (available_commands) {

        command = commands + readIndice;

        Motion::unsorted_axis_t = command->axis;//
        Motion::destinations = command->coords;
        Motion::parameters_t = command->parameters;
        Motion::v_parameters_t = command->v_parameters;
        Motion::axis_t = command->axis;

        if (command->speed_set)
            ActionController::setSpeed(command->speed);

        ActionController::parameters = command->parameters;

        startCommand();

        readIndice++;
        if (readIndice == COMMAND_NB)
            readIndice = 0;
        available_space++;
        available_commands--;
    }
}

void GCodeExecuter::startCommand() {


    switch (command->command_type) {
        case 0:
            switch (command->command_id) {

#define GCOMMAND(i, fname)\
                case i :\
                    ActionController::fname();\
                break;

#include "../parser_config.h"

#undef GCOMMAND

                default:
                    break;
            }
            break;
        case 1 :
            switch (command->command_id) {

#define MCOMMAND(i, fname)\
                case i :\
                    ActionController::fname();\
                break;

#include "../parser_config.h"

#undef MCOMMAND

                default:
                    break;
            }
        default:
            break;
    }
}


#define m GCodeExecuter

char tabS[STORAGE_SIZE][DATA_SIZE];
char *m::storage = (char *) tabS;

unsigned char tabLen[STORAGE_SIZE];
unsigned char *m::strings_sizes = tabLen;


int m::pull_index = STORAGE_SIZE - 1, m::push_index = STORAGE_SIZE - 1;


int m::available_strings = 0;

int m::empty_strings = STORAGE_SIZE;

bool m::pull();

bool m::gcode_processed = true;

char *m::current_gcode;

int m::available_space = COMMAND_NB;
int m::available_commands;
int m::addIndice = 0;
int m::readIndice = 0;

g_command *m::command;
g_command *m::commands = new g_command[COMMAND_NB];

float *const m::curve_t = Motion::curve_t;
unsigned char *const m::curve_width = &Motion::curve_width;
unsigned char *const m::curve_height = &Motion::curve_height;

unsigned int m::gcode_size, m::remain;
unsigned char m::cursor, m::coords_indice, m::curve_points_column, m::curve_points_row, m::free_row_cases, m::free_rows;

bool as[NB_AXIS];
bool *m::axis_set = as;


unsigned char m::parsing_step;


#undef m;











