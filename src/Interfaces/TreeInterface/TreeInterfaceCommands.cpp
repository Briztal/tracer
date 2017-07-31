
#include "../../config.h"
#ifdef ENABLE_TREE_INTERFACE

#include "TreeInterfaceCommands.h"
#include "../../interface.h"
#include "../../Core/EEPROMStorage.h"

#define EEPROM_SUBCANAL 1
#define PID_SUBCANAL 2
#define LOOP_SUBCANAL 3
#define ACTION_SUBCANAL 4
#define STEPPER_SUBCANAL 5
#define PARAMETER_SUBCANAL 7


#ifdef MONITOR_CANAL


void TreeInterfaceCommands::system_canal_function(char *data, uint8_t size) {



    if (!(size--)) return;
    char sub_canal = *(data++);

    switch(sub_canal) {
        case 0 :
            TI::send_tree_structure();
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


void TreeInterfaceCommands::parameters_system_canal(char *data, uint8_t size) {

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
            TI::prepare_system_packet();TI::add_char_out(PARAMETER_SUBCANAL);TI::add_char_out(0);\
            TI::add_char_out(indice); TI::add_string_out(#name); TI::add_float_out(min) ;\
            TI::add_float_out(max); TI::add_string_out(#unit);TI::send_packet();

#include "../../config.h"

#undef EXTERNAL_PARAMETER

        break;
        default:
            break;
    }

#endif

}


void TreeInterfaceCommands::pid_system_canal(char *data, uint8_t size) {

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
            TI::prepare_system_packet();TI::add_char_out(PID_SUBCANAL);TI::add_char_out(0);\
            TI::add_char_out(i); TI::add_string_out(#name); TI::add_float_out(EEPROMStorage::kps[pi]);\
            TI::add_float_out(EEPROMStorage::kis[pi]); TI::add_float_out(EEPROMStorage::kds[pi]);TI::send_packet();

#include "../../config.h"

#undef PID
            break;
        default:
            break;
    }

#endif
}


void TreeInterfaceCommands::loop_system_canal(char *data, uint8_t size) {

#ifdef ENABLE_ASSERV

    if (!size) return;
    char sub_canal = *data;

    switch(sub_canal) {
        case 0 :
            /* Init case : send_packet data concerning all External Parameters.
             * Packet Structure : indice, name, min, max, unit
             */

#define LOOP_FUNCTION(indice, name, period_ms)\
            TI::prepare_system_packet();TI::add_char_out(LOOP_SUBCANAL);TI::add_char_out(0);\
            TI::add_char_out(indice); TI::add_string_out(#name); TI::add_int_out(period_ms);\
            TI::send_packet();

#include "../../config.h"

#undef LOOP_FUNCTION

            break;
        default:
            break;
    }

#endif

}

void TreeInterfaceCommands::actions_system_canal(char *data, uint8_t size) {

    if (!size) return;
    char sub_canal = *data;

    switch(sub_canal) {
        case 0 ://The initialisation case : send_packet a packet for each stepper

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

#include "../../config.h"

#undef BINARY
#undef CONTINUOUS
#undef SERVO
            break;
        default:
            break;
    }
}


void TreeInterfaceCommands::steppers_system_canal(char *data, uint8_t size) {

#ifdef ENABLE_STEPPER_CONTROL

    if (!size) return;

    char sub_canal = *data;

    int group_size;

    switch(sub_canal) {
        case 0 ://The initialisation case : send_packet a packet for each stepper

#define STEPPER_DATA(i, j, ...)\
            TI::prepare_system_packet();TI::add_char_out(STEPPER_SUBCANAL);TI::add_char_out(0);TI::add_char_out(0);\
            TI::add_char_out(i);TI::add_char_out(j);TI::send_packet();

#include "../../config.h"

#undef STEPPER_DATA

#define CARTESIAN_GROUP(i, s0, s1, s2, s)\
            group_size = (s0!=-1)+(s1!=-1)+(s2!=-1);\
            TI::prepare_system_packet();TI::add_char_out(STEPPER_SUBCANAL);TI::add_char_out(0);TI::add_char_out(1);\
            TI::add_char_out(i);TI::add_char_out(group_size);if (s0!=-1) TI::add_char_out(s0);\
            if (s1!=-1) TI::add_char_out(s1);if (s2!=-1) TI::add_char_out(s2);TI::send_packet();

#include "../../config.h"

#undef CARTESIAN_GROUP

        case 1 :
            break;

        default:
            break;
    }

#endif

}



void TreeInterfaceCommands::EEPROM_system_canal(char *data, uint8_t size) {

    if (!size--) return;
    char sub_canal = *(data++);
    float f;
    switch(sub_canal) {
        case 0 : //The initialisation case : send the EEPROM_structure
            EEPROMStorage::send_structure();
            return;

        case 1 : //Read case
            f = EEPROMStorage::read(data, size);
            TI::prepare_EEPROM_packet();
            TI::add_char_out(1);
            TI::add_float_out(f);
            TI::send_packet();
            return;

        case 2 : //Write case
            f = EEPROMStorage::write(data, size);
            TI::prepare_EEPROM_packet();
            TI::add_char_out(2);
            TI::add_float_out(f);
            TI::send_packet();
            return;

        case 3 : //Save profile
            EEPROMStorage::saveProfile();
            TI::prepare_EEPROM_packet();
            TI::add_char_out(3);
            TI::add_float_out(0);
            TI::send_packet();
            return;

        case 4 : //Restore default profile;
            EEPROMStorage::setDefaultProfile();
            TI::prepare_EEPROM_packet();
            TI::add_char_out(4);
            TI::add_float_out(0);
            TI::send_packet();
        default:
            return;
    }

}




#include "../../Actions/ContinuousActions.h"
#include "../../StepperControl/SpeedPlanner.h"
#include "../../StepperControl/LinearMovement/LinearMovement.h"
#include "../../StepperControl/LinearMovement/HomingMovement.h"
#include "../../StepperControl/SpeedManager.h"
#include "../../StepperControl/MovementExecuter.h"

void TreeInterfaceCommands::action(char * dptr, uint8_t size) {

    CI::echo("400");
    SpeedManager::print_speed_distance();
    float coords[NB_STEPPERS]{0};
    coords[0] = 100;
    coords[1] = 160;
    coords[2] = 100;
    coords[3] = 40;
    coords[4] = 50;
    coords[5] = 138;
    coords[6] = 82;
    coords[7] = 43;
    coords[8] = 120;
    coords[9] = 12;
    coords[10] = 152;
    coords[11] = 90;
    coords[12] = 14;
    coords[13] = 142;
    coords[14] = 160;
    coords[15] = 35;
    coords[16] = 100;
    ContinuousActions::setLinearPower0(1);
    SpeedPlanner::set_speed_for_group(0, 500);
    SpeedPlanner::set_speed_group(0);

    //TODO TESTER AVEC LES Z

    LinearMovement::prepare_motion(coords);


    delay(20);

    coords[0] = 40;
    coords[1] = 110;
    coords[2] = 40;
    coords[3] = 110;
    coords[4] = 40;
    coords[5] = 110;
    coords[6] = 40;
    coords[7] = 110;
    coords[8] = 40;
    coords[9] = 110;
    coords[10] = 40;
    coords[11] = 110;
    coords[12] = 40;
    coords[13] = 110;
    coords[14] = 40;
    coords[15] = 110;
    coords[16] = 40;
    LinearMovement::prepare_motion(coords);

    delay(20);

    MovementExecuter::enqueue_homing_movement();

    delay(20);

    coords[0] = 75;
    coords[1] = 75;
    coords[2] = 75;
    coords[3] = 75;
    coords[4] = 75;
    coords[5] = 75;
    coords[6] = 75;
    coords[7] = 75;
    coords[8] = 75;
    coords[9] = 75;
    coords[10] = 75;
    coords[11] = 75;
    coords[12] = 75;
    coords[13] = 75;
    coords[14] = 75;
    coords[15] = 75;
    coords[16] = 75;
    LinearMovement::prepare_motion(coords);


    CI::echo("EXIT");

}

void TreeInterfaceCommands::home(char * dptr, uint8_t size) {

    HomingMovement::move();

}

#endif

