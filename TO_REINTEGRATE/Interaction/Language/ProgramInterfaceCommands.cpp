/*
  TreeInterfaceCommands.cpp - Part of TRACER

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
#ifdef ENABLE_PROGRAM_INTERFACE


#include <Interfaces/ProgramInterface/ProgramInterface.h>
#include "ProgramInterfaceCommands.h"
#include <StepperControl/MachineInterface.h>
#include <Actions/ContinuousActions.h>
#include <Storage/EEPROMStorage.h>
#include <interface.h>
#include <StepperControl/Machine.h>
#include <Storage/_eeprom_storage_data.h>
#include <Storage/EEPROMInterface.h>


//#define EEPROM_SUBCANAL 1
#define PID_SUBCANAL 2
#define LOOP_SUBCANAL 3
#define ACTION_SUBCANAL 4
#define STEPPER_SUBCANAL 5
#define PARAMETER_SUBCANAL 7

task_state_t ProgramInterfaceCommands::system_canal_function(char *data, uint8_t size) {

    CI::echo("data "+String(size));



    if (!(size--)) return invalid_arguments;

    char sub_canal = *(data++);

    switch (sub_canal) {
        case 0 :
            return TI::send_tree_structure();
        case 1 :
            return EEPROM_system_canal(data, size);
        case 2 :
            return pid_system_canal(data, size);
        case 3 :
            return loop_system_canal(data, size);
        case 4 :
            return actions_system_canal(data, size);
        case 5 :
            return steppers_system_canal(data, size);
        case 6 ://ECHO. DO NOTHING
            return complete;//echo_system_canal(data, size);
        case 7 :
            return parameters_system_canal(data, size);
        default:
            return invalid_arguments;
    }
}


//------------------------------------------------CANAL PROCESS FUNCTIONS-----------------------------------------------


task_state_t ProgramInterfaceCommands::parameters_system_canal(char *data, uint8_t size) {


#ifdef ENABLE_CONTROL_LOOPS

    delay_ms(50);

    if (!size) return invalid_arguments;

    char sub_canal = *data;

    switch (sub_canal) {
        case 0 :
            /* Init case : send_packet data concerning all External Parameters.
             * Packet Structure : indice, name, beginning, ending, unit
             */

#define EXTERNAL_PARAMETER(indice, name, beginning, ending, unit)\
            TI::prepare_system_packet();TI::add_char_out(PARAMETER_SUBCANAL);TI::add_char_out(0);\
            TI::add_char_out(indice); TI::add_string_out(#name); TI::add_float_out(beginning) ;\
            TI::add_float_out(ending); TI::add_string_out(#unit);TI::send_packet();

#include <config.h>

#undef EXTERNAL_PARAMETER

            return complete;
        default:
            return invalid_arguments;
    }


#else

    return complete;


#endif

}


task_state_t ProgramInterfaceCommands::pid_system_canal(char *data, uint8_t size) {



#ifdef ENABLE_CONTROL_LOOPS
    
    pid_data_t *pid_p;
    
    if (!size) return invalid_arguments;
    char sub_canal = *data;
    int pi;
    switch (sub_canal) {

        case 0 :
            /* Init case : send_packet data concerning all External Parameters.
             * Packet Structure : indice, name, beginning, ending, unit
             */

#define PID(i, name, ...)\
            pid_p = EEPROMStorage::pids_data+i;\
            TI::prepare_system_packet();TI::add_char_out(PID_SUBCANAL);TI::add_char_out(0);\
            TI::add_char_out(i); TI::add_string_out(#name); TI::add_float_out(pid_p->kp);\
            TI::add_float_out(pid_p->ki); TI::add_float_out(pid_p->kd);TI::send_packet();

#include <config.h>

#undef PID
            return complete;
        default:
            return invalid_arguments;
    }

#else

    return complete;

#endif
}


task_state_t ProgramInterfaceCommands::loop_system_canal(char *data, uint8_t size) {

#ifdef ENABLE_CONTROL_LOOPS

    if (!size) return invalid_arguments;
    char sub_canal = *data;

    switch (sub_canal) {
        case 0 :
            /* Init case : send_packet data concerning all External Parameters.
             * Packet Structure : indice, name, beginning, ending, unit
             */

#define LOOP_FUNCTION(indice, name, period_ms)\
            TI::prepare_system_packet();TI::add_char_out(LOOP_SUBCANAL);TI::add_char_out(0);\
            TI::add_char_out(indice); TI::add_string_out(#name); TI::add_int_out(period_ms);\
            TI::send_packet();

#include <config.h>

#undef LOOP_FUNCTION


            return complete;
        default:
            return invalid_arguments;
    }

#else

    return complete;

#endif

}

task_state_t ProgramInterfaceCommands::actions_system_canal(char *data, uint8_t size) {

    if (!size) return invalid_arguments;
    char sub_canal = *data;

    switch (sub_canal) {
        case 0 ://The initialisation case : send_packet a packet for each trajectory_control

#define BINARY(i, name, powerPin, enableValue)\
            TI::prepare_system_packet();TI::add_char_out(ACTION_SUBCANAL);TI::add_char_out(0);TI::add_char_out(0);\
            TI::add_char_out(i);TI::add_string_out(#name);TI::send_packet();\

#define CONTINUOUS(i, name, powerPin, maxValue)\
            TI::prepare_system_packet();TI::add_char_out(ACTION_SUBCANAL);TI::add_char_out(0);TI::add_char_out(1);\
            TI::add_char_out(i);TI::add_string_out(#name);TI::add_float_out(maxValue);TI::send_packet();\

#define SERVO(i, name, dataPin, minValue, maxValue)\
            TI::prepare_system_packet();TI::add_char_out(ACTION_SUBCANAL);TI::add_char_out(0);TI::add_char_out(2);\
            TI::add_char_out(i);TI::add_string_out(#name);TI::add_float_out(minValue);TI::add_float_out(maxValue);\
            TI::send_packet();\


#include <config.h>

#undef BINARY
#undef CONTINUOUS
#undef SERVO
            return complete;
        default:
            return invalid_arguments;
    }
}


task_state_t ProgramInterfaceCommands::steppers_system_canal(char *data, uint8_t size) {



#ifdef ENABLE_STEPPER_CONTROL

    if (!size) return invalid_arguments;

    char sub_canal = *data;

    int group_size;

    switch (sub_canal) {
        case 0 ://The initialisation case : send_packet a packet for each trajectory_control

#define STEPPER_DATA(i, j, ...)\
            TI::prepare_system_packet();TI::add_char_out(STEPPER_SUBCANAL);TI::add_char_out(0);TI::add_char_out(0);\
            TI::add_char_out(i);TI::add_char_out(j);TI::send_packet();

#include <config.h>

#undef STEPPER_DATA

#define CARTESIAN_GROUP(i, s0, s1, s2, s)\
            group_size = (s0!=-1)+(s1!=-1)+(s2!=-1);\
            TI::prepare_system_packet();TI::add_char_out(STEPPER_SUBCANAL);TI::add_char_out(0);TI::add_char_out(1);\
            TI::add_char_out(i);TI::add_char_out(group_size);if (s0!=-1) TI::add_char_out(s0);\
            if (s1!=-1) TI::add_char_out(s1);if (s2!=-1) TI::add_char_out(s2);TI::send_packet();

#include <config.h>

#undef CARTESIAN_GROUP

            return complete;
        default:
            return invalid_arguments;
    }

#else

    return complete;

#endif
}


task_state_t ProgramInterfaceCommands::EEPROM_system_canal(char *data, uint8_t size) {
    
    if (!size--) return invalid_arguments;
    char sub_canal = *(data++);
    float f;
    switch (sub_canal) {

        case 0 : //The initialisation case : send the EEPROM_structure
            EEPROMInterface::send_structure();
            return complete;

        case 1 : //Read case
            f = 0;//Storage::read_integer(data, size);
            TI::prepare_EEPROM_packet();
            TI::add_char_out(1);
            TI::add_float_out(f);
            TI::send_packet();
            return complete;

        case 2 : //Write case
            f = 0;//EEPROMInterface::write_data(data, size);
            TI::prepare_EEPROM_packet();
            TI::add_char_out(2);
            TI::add_float_out(f);
            TI::send_packet();
            return complete;

        case 3 : //Save profile
            EEPROMStorage::write_profile();
            TI::prepare_EEPROM_packet();
            TI::add_char_out(3);
            TI::add_float_out(0);
            TI::send_packet();
            return complete;

        case 4 : //Restore default profile;
            EEPROMStorage::set_default_profile();
            TI::prepare_EEPROM_packet();
            TI::add_char_out(4);
            TI::add_float_out(0);
            TI::send_packet();
        default:
            return invalid_arguments;
    }

    
}


task_state_t ProgramInterfaceCommands::action(uint8_t args_index) {

    Machine::set_speed_for_carriage(0, 150);
    Machine::set_carriage(0);
    Machine::line_to(50, 50, 0);
    Machine::line_to(100, 100, 0);

    Machine::zero_carriages();

    Machine::set_speed_for_carriage(1, 150);
    Machine::set_carriage(1);
    Machine::line_to(50, 50, 0);
    Machine::line_to(100, 100, 0);

    Machine::zero_carriages();

    Machine::set_speed_for_carriage(2, 150);
    Machine::set_carriage(2);
    Machine::line_to(50, 50, 0);
    Machine::line_to(100, 100, 0);

    Machine::zero_carriages();

    Machine::set_speed_for_carriage(3, 150);
    Machine::set_carriage(3);
    Machine::line_to(50, 50, 0);
    Machine::set_carriage(0);
    Machine::line_to(100, 100, 0);

    Machine::zero_carriages();

    float position[NB_AXIS]{0};
    position[0] = 500;
    position[1] = 500;
    position[2] = 500;
    position[3] = 500;

    MachineInterface::set_speed_group(0);
    MachineInterface::set_speed_for_group(0, 200);

    MachineInterface::linear_movement(position);

    CI::echo("EXIT");

    return complete;

}

task_state_t ProgramInterfaceCommands::home(uint8_t args_index) {

    //HomingMovement::plan_movement();

    return complete;
}

#endif

