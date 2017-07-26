/*
  GCodeInterface.cpp - Part of TRACER

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

#include "GCodeInterface.h"
#include "../../hardware_language_abstraction.h"
#include "../../Core/MachineControllerSystem.h"


void CI::begin() {
    serial_begin(BAUDRATE);

    delay_ms(100);

    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);
}


void GCodeInterface::echo(const String msg) {
    serial_echo_str(msg+"\n");
}

void GCodeInterface::read_serial() {

    while(serial_available()) {

        //Read the serial
        char read_char = (char)serial_read();

        //If the recieved char is a line feed or a carriage return
        if ((read_char == 10) || (read_char == 13)) {

            //If a char has effectively been received
            if (gcode_size) {


                //Parse the GCode
                parse();

                //Reset the data_in
                reset();

            }
        } else {

            //Append the read char to data_in
            *(data_in++) = read_char;
            gcode_size++;

        }
    }
}

void GCodeInterface::reset() {
    gcode_size = 0;
    data_in = data_in_0;
}


bool GCodeInterface::parse() {

    init_parsing();
    
    char command_name[GCODE_MAX_DEPTH];
    
    char c;
    float value;

    unsigned char command_size = get_command(command_name);
    if (!command_size)
        return false;


    while (gcode_size) {
        //Get next word and analyse_parameter, if it fails, return false;
        get_parameter(&c, &value);
        analyse_parameter(c, value);
    }

    execute(command_name, command_size);

    return true;

}



void GCodeInterface::init_parsing() {

    //INIT :

    data_in = data_in_0;
    motion_size = curve_points_column = curve_points_row = 0;
    free_row_cases = NB_STEPPERS - 1;
    free_rows = MAX_CURVE_POINTS - 1;

    curve_width = NB_STEPPERS;

    for (int p = 0; p < NB_PARAMETERS; p++)
        v_parameters[p] = false;


}


unsigned char GCodeInterface::get_command(char *command) {
    do {
        if (!gcode_size--) {
            return 0;
        }
    } while ((*command = *(data_in++)) == ' ');

    unsigned char size = 1;
    command++;


    char t;
    while ((gcode_size--) && ((t = *(data_in++)) != ' ')) {
        *(command++) = t;
        size++;
    }
    return size;
}

bool GCodeInterface::get_parameter(char *id, float *value) {
    do {
        if (!gcode_size--) {
            return false;
        }
    } while ((*id = *(data_in++)) == ' ');

    char t;
    String strValue = "";
    while ((gcode_size--) && ((t = *(data_in++)) != ' ')) {
        strValue += t;
    }
    if (!strValue.length()) {
        *value = 0;
    } else {
        *value = strValue.toFloat();
    }
    return true;
}



bool GCodeInterface::analyse_parameter(char c, float value) {

    switch (c) {

//-----------------------AXIS_COORDINATES-------------------

#define AXIS_TREATMENT(i, j, si, st, sp, a, d, pinStep, pd, pp, pmi, vi, pma, va) \
            case j:\
                if (axis_set[i])\
                    return false;\
                *(coords+motion_size) = value;\
                *(axis + motion_size++) = i;\
                axis_set[i] = true;\
                break;

#include "../../config.h"

#undef AXIS_TREATMENT

//-----------------------CURVE PARAMETERS-------------------
        case 'D' ://CURVE_POINT
            *(curve_t + NB_STEPPERS * curve_points_row + curve_points_column) = value;
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
            if (curve_width > curve_points_column)
                curve_width = curve_points_column;
            curve_points_column = 0;
            free_rows--;
            free_row_cases = NB_STEPPERS - 1;
            break;

//-----------------------PARAMETERS-------------------

            //TODO INTEGRER LES ACTIONS

#define GPARAMETER(i, j, k)\
            case j : \
                if (!*(v_parameters+i)){\
                    *(parameters+i) = value;\
                    *(v_parameters+i) = true;\
                }\
                break;

#include "gcode_interface_config.h"

#undef GPARAMETER

        default:
            return false;
    }

    return true;
}


void GCodeInterface::clean_parsing() {
    if (curve_points_column) {
        if (curve_width > curve_points_column)
            curve_width = curve_points_column;
    } else
        curve_points_row--;

    curve_height = ++curve_points_row;

}


void GCodeInterface::execute(char * command, unsigned char command_size) {

    if (!command_size)
        return;
    char c = *(command++);
    command_size--;

#define COMMAND(i, fname) \
    case i : \
        MachineController::fname();\
        return;

#define GO_LOWER(i) \
    case i : \
        if (!(command_size--)) return;\
        c = *(command++);\
        switch(c) {\

#define GO_LOWER_COMMAND(i, fname) \
    case i : \
        if (!(command_size--)) {MachineController::fname();return;}\
        c = *(command++);\
        switch(c) {\


#define GO_UPPER()\
        default : return;\
    }\

    switch(c) {

        #include "gcode_interface_config.h"

                            default: return;
    }

#undef COMMAND
#undef GO_LOWER
#undef GO_LOWER_COMMAND
#undef GO_UPPER


}

#define m GCodeInterface


unsigned char m::gcode_size;

char tdatain[PACKET_SIZE];
char *m::data_in = tdatain;
char *const m::data_in_0 = tdatain;


float tcurve[MAX_CURVE_POINTS][NB_STEPPERS];
float *const m::curve_t = (float *) tcurve;
unsigned char m::curve_width;
unsigned char m::curve_height;

unsigned char m::motion_size, m::curve_points_column, m::curve_points_row, m::free_row_cases, m::free_rows;

unsigned char taxis[NB_STEPPERS];
unsigned char *const m::axis = taxis;

float tcoords[NB_STEPPERS];
float *const m::coords = tcoords;

float tparam[NB_PARAMETERS];
float *const m::parameters = tparam;

bool tbool[NB_PARAMETERS];
bool *const m::v_parameters = tbool;

bool* axis_set;

#undef m

#endif







