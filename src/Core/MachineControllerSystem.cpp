/*
  MachineControllerSystem.cpp - Part of TRACER

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

#ifdef ENABLE_COMMAND_INTERFACE

#include "MachineControllerSystem.h"
#include "../Interfaces/CommandInterface/CommandInterface.h"
#include "EEPROMStorage.h"
#define EEPROM_SUBCANAL 1
#define PID_SUBCANAL 2
#define LOOP_SUBCANAL 3
#define ACTION_SUBCANAL 4
#define STEPPER_SUBCANAL 5
#define PARAMETER_SUBCANAL 7


#ifdef MONITOR_CANAL


void MachineController::system_canal_function(char *data, uint8_t size) {



    if (!(size--)) return;
    char sub_canal = *(data++);

    switch(sub_canal) {
        case 0 :
            CI::send_tree_structure();
            break;
        case 1 :
            EEPROM_system_canal(data, size);
            break;
        case 2 :
            pid_system_canal(data, size);
            break;
        case 3 :
            loop_system_canal(data, size);
            break;
        case 4 :
            actions_system_canal(data, size);
            break;
        case 5 :
            steppers_system_canal(data, size);
            break;
        case 6 ://ECHO. DO NOTHING
            //echo_system_canal(data, size);
            break;
        case 7 :
            parameters_system_canal(data, size);
            break;

        default:break;
    }
    return;
}

#endif


//------------------------------------------------CANAL PROCESS FUNCTIONS-----------------------------------------------


void MachineController::parameters_system_canal(char *data, uint8_t size) {

#ifdef ENABLE_ASSERV

    delay(50);


    if (!size) return;
    char sub_canal = *data;

    switch(sub_canal) {
        case 0 :
            /* Init case : send_packet data concerning all External Parameters.
             * Packet Structure : indice, name, min, max, unit
             */

#define EXTERNAL_PARAMETER(indice, name, min, max, unit)\
            CI::prepare_system_packet();CI::add_char_out(PARAMETER_SUBCANAL);CI::add_char_out(0);\
            CI::add_char_out(indice); CI::add_string_out(#name); CI::add_float_out(min) ;\
            CI::add_float_out(max); CI::add_string_out(#unit);CI::send_packet();

#include "../config.h"

#undef EXTERNAL_PARAMETER

        break;
        default:
            break;
    }

#endif

}


void MachineController::pid_system_canal(char *data, uint8_t size) {

#ifdef ENABLE_ASSERV

    if (!size--) return;
    char sub_canal = *data;
    int pi;
    switch(sub_canal) {

        case 0 :
            /* Init case : send_packet data concerning all External Parameters.
             * Packet Structure : indice, name, min, max, unit
             */

            pi = 0;

#define PID(i, name, kp, ki, kd)\
            CI::prepare_system_packet();CI::add_char_out(PID_SUBCANAL);CI::add_char_out(0);\
            CI::add_char_out(i); CI::add_string_out(#name); CI::add_float_out(EEPROMStorage::kps[pi]);\
            CI::add_float_out(EEPROMStorage::kis[pi]); CI::add_float_out(EEPROMStorage::kds[pi]);CI::send_packet();

#include "../config.h"

#undef PID
            break;
        default:
            break;
    }

#endif
}


void MachineController::loop_system_canal(char *data, uint8_t size) {

#ifdef ENABLE_ASSERV

    if (!size) return;
    char sub_canal = *data;

    switch(sub_canal) {
        case 0 :
            /* Init case : send_packet data concerning all External Parameters.
             * Packet Structure : indice, name, min, max, unit
             */

#define LOOP_FUNCTION(indice, name, period_ms)\
            CI::prepare_system_packet();CI::add_char_out(LOOP_SUBCANAL);CI::add_char_out(0);\
            CI::add_char_out(indice); CI::add_string_out(#name); CI::add_int_out(period_ms);\
            CI::send_packet();

#include "../config.h"

#undef LOOP_FUNCTION

            break;
        default:
            break;
    }

#endif

}

void MachineController::actions_system_canal(char *data, uint8_t size) {

    if (!size) return;
    char sub_canal = *data;

    switch(sub_canal) {
        case 0 ://The initialisation case : send_packet a packet for each stepper

#define BINARY(i, name, powerPin, enableValue)\
            CI::prepare_system_packet();CI::add_char_out(ACTION_SUBCANAL);CI::add_char_out(0);CI::add_char_out(0);\
            CI::add_char_out(i);CI::add_string_out(#name);CI::send_packet();\

#define CONTINUOUS(i, name, powerPin, maxValue)\
            CI::prepare_system_packet();CI::add_char_out(ACTION_SUBCANAL);CI::add_char_out(0);CI::add_char_out(1);\
            CI::add_char_out(i);CI::add_string_out(#name);CI::add_float_out(maxValue);CI::send_packet();\

#define SERVO(i, name, dataPin, minValue, maxValue)\
            CI::prepare_system_packet();CI::add_char_out(ACTION_SUBCANAL);CI::add_char_out(0);CI::add_char_out(2);\
            CI::add_char_out(i);CI::add_string_out(#name);CI::add_float_out(minValue);CI::add_float_out(maxValue);\
            CI::send_packet();\

#include "../config.h"

#undef BINARY
#undef CONTINUOUS
#undef SERVO
            break;
        default:
            break;
    }


}





void MachineController::steppers_system_canal(char *data, uint8_t size) {

#ifdef ENABLE_STEPPER_CONTROL

    if (!size) return;

    char sub_canal = *data;

    int group_size;

    switch(sub_canal) {
        case 0 ://The initialisation case : send_packet a packet for each stepper

#define STEPPER_DATA(i, j, ...)\
            CI::prepare_system_packet();CI::add_char_out(STEPPER_SUBCANAL);CI::add_char_out(0);CI::add_char_out(0);\
            CI::add_char_out(i);CI::add_char_out(j);CI::send_packet();

#include "../config.h"

#undef STEPPER_DATA


#define CARTESIAN_GROUP(i, s0, s1, s2, s)\
            group_size = (s0!=-1)+(s1!=-1)+(s2!=-1);\
            CI::prepare_system_packet();CI::add_char_out(STEPPER_SUBCANAL);CI::add_char_out(0);CI::add_char_out(1);\
            CI::add_char_out(i);CI::add_char_out(group_size);if (s0!=-1) CI::add_char_out(s0);\
            if (s1!=-1) CI::add_char_out(s1);if (s2!=-1) CI::add_char_out(s2);CI::send_packet();


#include "../config.h"

#undef CARTESIAN_GROUP


        case 1 :
            break;

        default:
            break;
    }

#endif

}



void MachineController::EEPROM_system_canal(char *data, uint8_t size) {

    if (!size--) return;
    char sub_canal = *(data++);
    float f;
    switch(sub_canal) {
        case 0 : //The initialisation case : send the EEPROM_structure
            EEPROMStorage::send_structure();
            return;

        case 1 : //Read case
            f = EEPROMStorage::read(data, size);
            CI::prepare_EEPROM_packet();
            CI::add_char_out(1);
            CI::add_float_out(f);
            CI::send_packet();
            return;

        case 2 : //Write case
            f = EEPROMStorage::write(data, size);
            CI::prepare_EEPROM_packet();
            CI::add_char_out(2);
            CI::add_float_out(f);
            CI::send_packet();
            return;

        case 3 : //Save profile
            EEPROMStorage::saveProfile();
            CI::prepare_EEPROM_packet();
            CI::add_char_out(3);
            CI::add_float_out(0);
            CI::send_packet();
            return;

        case 4 : //Restore default profile;
            EEPROMStorage::setDefaultProfile();
            CI::prepare_EEPROM_packet();
            CI::add_char_out(4);
            CI::add_float_out(0);
            CI::send_packet();
        default:
            return;
    }


}

#endif

