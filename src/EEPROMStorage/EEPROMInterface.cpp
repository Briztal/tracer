//
// Created by root on 10/10/17.
//

#ifdef suus

#include "EEPROMInterface.h"


/*
 * Structure sub_canals :
 *      0 : PIDs
 *          indice :
 *              0 : kp
 *              1 : ki
 *              2 : kd
 *      1 : Loops
 *          indice :
 *              period_ms
 *      2 : Continuous Actions
 *          indice
 *              maximum
 *      3 : Servo Actions
 *          indice
 *              minimum
 *              maximum
 *      4 : steppers
 *          indice
 *              0 : size
 *              1 : steps
 *              2 : max_speeds
 *              3 : acceleration
 *      5 : speed_groups
 *          indice
 *              max_speed
 *      6 : custom_parameters;
 *          k1_position_indice : parameter k1_position_indice;
 */



#define PID_CAT (uint8_t) 0
#define LOOP_CAT (uint8_t) 1
#define CONTINUOUS_CAT (uint8_t) 2
#define SERVO_CAT (uint8_t) 3
#define STEPPER_CAT (uint8_t) 4
#define CARTESIAN_GROUP_CAT (uint8_t) 5
#define CUSTOM_CAT (uint8_t) 6


//###############################################PRIMITIVE_WRITE_METHODS################################################



void EEPROMStorage::print_stored_data() {

    //Stepper motors
#ifdef ENABLE_STEPPER_CONTROL
    CI::echo("Stepper Motors : ");
    for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {
        CI::echo("Stepper " + str(stepper));
        CI::echo("size : " + str(sizes[stepper]));
        CI::echo("steps : " + str(steps[stepper]));
        CI::echo("max regulation_speed : " + str(maximum_speeds[stepper]));
        CI::echo("acceleration : " + str(accelerations[stepper]) + "\n");
        CI::echo("jerk : " + str(jerks[stepper]) + "\n");
    }

    CI::echo("Speed Groups : ");
    for (int group = 0; group < NB_CARTESIAN_GROUPS; group++) {
        CI::echo("Group " + str(group));
        CI::echo("max regulation_speed : " + str(group_maximum_speeds[group]) + "\n");
    }

#endif

#ifdef ENABLE_ASSERV

    //PIDs
    CI::echo("PIDS : ");
    for (int pid = 0; pid < NB_PIDS; pid++) {
        CI::echo("PID " + str(pid));
        CI::echo("kp : " + str(kps[pid]));
        CI::echo("ki : " + str(kis[pid]));
        CI::echo("kd : " + str(kds[pid]) + "\n");
    }

    //Control Loops
    CI::echo("Control Loops : ");
    for (int loop = 0; loop < NB_LOOPS; loop++) {
        CI::echo("Loop " + str(loop));
        CI::echo("period (ms) : " + str(loop_periods[loop]) + "\n");
    }

#endif

    //Continuous Actions
    CI::echo("Continuous Actions : ");
    for (int continuous = 0; continuous < NB_CONTINUOUS; continuous++) {
        CI::echo("Continuous " + str(continuous));
        CI::echo("maximum : " + str(continuous_max[continuous]) + "\n");
    }

    //Servo Actions
    CI::echo("Servo Actions : ");
    for (int servo = 0; servo < NB_SERVOS; servo++) {
        CI::echo("Loop " + str(servo));
        CI::echo("minimum : " + str(servos_min[servo]));
        CI::echo("maximum : " + str(servos_max[servo]) + "\n");
    }

    //Custom Data
    CI::echo("Custom Data : ");
    //Custom data
#define EEPROM_BOOL(name, default_value) CI::echo(str(#name)+" : "+str(name));
#define EEPROM_CHAR(name, default_value) CI::echo(str(#name)+" : "+str(name));
#define EEPROM_INT(name, default_value) CI::echo(str(#name)+" : "+str(name));
#define EEPROM_LONG(name, default_value) CI::echo(str(#name)+" : "+str(name));
#define EEPROM_FLOAT(name, default_value) CI::echo(str(#name)+" : "+str(name));

#include "../config.h"

#undef EEPROM_BOOL
#undef EEPROM_CHAR
#undef EEPROM_INT
#undef EEPROM_LONG
#undef EEPROM_FLOAT

    CI::echo(" ");

}


#ifdef ENABLE_TREE_INTERFACE

void EEPROMStorage::send_structure() {
    /*
     * Syntax :
     * reset_tree (beginning) :         0;
     * confirm_tree (end) :             1;
     * go_upper :                       2;
     * go_lower(k1_position_indice, name) :              3, k1_position_indice, name
     * create_leaf(k1_position_indice, name) :           4, k1_position_indice, name
     */

    TI::prepare_EEPROM_packet();\
    TI::add_char_out(0);\
    TI::add_char_out(0);\
    TI::send_packet();

#define EEPROM_UPPER\
    TI::prepare_EEPROM_packet();\
    TI::add_char_out(0);\
    TI::add_char_out(2);\
    TI::send_packet();

#define EEPROM_LOWER(i, name)\
    TI::prepare_EEPROM_packet();\
    TI::add_char_out(0);\
    TI::add_char_out(3);\
    TI::add_char_out(i);\
    TI::add_string_out(#name);\
    TI::send_packet();

#define EEPROM_LEAF(i, name)\
    TI::prepare_EEPROM_packet();\
    TI::add_char_out(0);\
    TI::add_char_out(4);\
    TI::add_char_out(i);\
    TI::add_string_out(#name);\
    TI::send_packet();


#ifdef ENABLE_ASSERV

    //---------------------------------------------PIDs---------------------------------------------

    EEPROM_LOWER(PID_CAT, pids);

    //For each PID, mark a case for kp, ki, and kd.
#define PID(i, ...)\
    EEPROM_LOWER(i, i);EEPROM_LEAF(0, kp);EEPROM_LEAF(1, ki);EEPROM_LEAF(2, kd);EEPROM_UPPER;

#include "../config.h"

#undef PID

    EEPROM_UPPER;

    //---------------------------------------------Control loops---------------------------------------------

    EEPROM_LOWER(LOOP_CAT, loops);
    //For each Control loop, mark a case for the period.
#define LOOP_FUNCTION(indice, ...) EEPROM_LOWER(indice, indice);EEPROM_LEAF(0, period);EEPROM_UPPER;

#include "../config.h"

#undef LOOP_FUNCTION

    EEPROM_UPPER;

#endif

    //---------------------------------------------Continuous Actions---------------------------------------------

    EEPROM_LOWER(CONTINUOUS_CAT, binaries);
    //For each Continuous Action, mark a case for the maximum value.
#define CONTINUOUS(indice, ...) EEPROM_LOWER(indice, indice);EEPROM_LEAF(0, max_value);EEPROM_UPPER;

#include "../config.h"

#undef CONTINUOUS


    EEPROM_UPPER;

    //---------------------------------------------Servo Actions---------------------------------------------
    EEPROM_LOWER(SERVO_CAT, continuous);
    //For each Servo Action, mark a case for the minimum and maximum values.
#define SERVO(indice, ...) EEPROM_LOWER(indice, indice);EEPROM_LEAF(0, min_value);EEPROM_LEAF(1, max_value);EEPROM_UPPER;

#include "../config.h"

#undef SERVO

    EEPROM_UPPER;


#ifdef ENABLE_STEPPER_CONTROL

    //---------------------------------------------Steppers---------------------------------------------

    EEPROM_LOWER(STEPPER_CAT, steppers);
    //For each Stepper, mark a case for the size, the number of steps per unit, and maximum regulation_speed and acceleration.
#define STEPPER_DATA(indice, ...) EEPROM_LOWER(indice, indice);\
    EEPROM_LEAF(0, size);EEPROM_LEAF(1, steps);EEPROM_LEAF(2, max_speed);EEPROM_LEAF(3, max_acceleration);EEPROM_LEAF(3, max_jerk);\
    EEPROM_UPPER;

#include "../config.h"

#undef STEPPER_DATA

    EEPROM_UPPER;

    //---------------------------------------------Cartesian Groups---------------------------------------------

    EEPROM_LOWER(CARTESIAN_GROUP_CAT, speed_groups);
    //For each group, mark a case for its maximum regulation_speed
#define CARTESIAN_GROUP(indice, ...) EEPROM_LOWER(indice, indice);\
    EEPROM_LEAF(0, max_speed);EEPROM_UPPER;

#include "../config.h"

#undef CARTESIAN_GROUP

    EEPROM_UPPER;

#endif

    //---------------------------------------------Custom parameters---------------------------------------------
    EEPROM_LOWER(CUSTOM_CAT, custom);

    int custom_indice = 0;
    //Mark a case for each custom parameter
#define EEPROM_FLOAT(name, v) EEPROM_LEAF(custom_indice, name);custom_indice++;
#define EEPROM_BOOL(n, v) EEPROM_FLOAT(n,v)
#define EEPROM_CHAR(n, v) EEPROM_FLOAT(n,v)
#define EEPROM_INT(n, v) EEPROM_FLOAT(n,v)
#define EEPROM_LONG(n, v) EEPROM_FLOAT(n,v)

#include "../config.h"

#undef EEPROM_BOOL
#undef EEPROM_CHAR
#undef EEPROM_INT
#undef EEPROM_LONG
#undef EEPROM_FLOAT

    EEPROM_UPPER;

#undef EEPROM_UPPER
#undef EEPROM_LOWER
#undef EEPROM_LEAF

    //Confirm and fetch the transmitted tree
    TI::prepare_EEPROM_packet();\
    TI::add_char_out(0);\
    TI::add_char_out(1);\
    TI::send_packet();

}


float EEPROMStorage::read(int8_t *data, uint8_t size) {
    if (!(size--)) return 0;
    uint8_t c = (uint8_t)*(data++), id;

    switch (c) {

#ifdef ENABLE_ASSERV

        case PID_CAT :
            if (size < 2) return 0;
            id = (uint8_t)*(data++);
            if (id>=NB_PIDS) return 0;
            switch (*(data)) {
                case 0 ://kp
                    return pids_data[id].kp;
                case 1 ://ki
                    return pids_data[id].ki;
                case 2 ://kd
                    return pids_data[id].kd;
                default:
                    return 0;
            }

        case LOOP_CAT :
            if (size < 2) return 0;
            id = (uint8_t)*(data++);
            if (id>=NB_LOOPS) return 0;
            switch (*(data)) {
                case 0 ://period
                    return loop_periods[id];
                default:
                    return 0;
            }

#endif

        case CONTINUOUS_CAT :
            if (size < 2) return 0;
            id = (uint8_t)*(data++);
            if (id>=NB_CONTINUOUS) return 0;
            switch (*(data)) {
                case 0 ://max
                    return continuous_data[id].max;
                default:
                    return 0;
            }

        case SERVO_CAT :
            if (size < 2) return 0;
            id = (uint8_t)*(data++);
            if (id>=NB_SERVOS) return 0;
            switch (*(data)) {
                case 0 ://min
                    return servos_data[id].min;
                case 1 ://max
                    return servos_data[id].max;
                default:
                    return 0;
            }

#ifdef ENABLE_STEPPER_CONTROL
        case STEPPER_CAT :
            if (size < 2) return 0;
            id = (uint8_t)*(data++);
            if (id>=NB_STEPPERS) return 0;
            switch (*(data)) {
                case 0 ://size
                    return steppers_data[id].size;
                case 1 ://steps
                    return steppers_data[id].steps;
                case 2 ://regulation_speed
                    return steppers_data[id].maximum_speed;
                case 3 ://acceleration
                    return steppers_data[id].acceleration;
                case 4 ://acceleration
                    return steppers_data[id].jerk;
                default:
                    return 0;
            }

        case CARTESIAN_GROUP_CAT :
            if (size < 2) return 0;
            id = (uint8_t)*(data++);
            if (id>=NB_CARTESIAN_GROUPS) return 0;
            switch (*(data)) {
                case 0 ://size
                    return group_maximum_speeds[id];
                default:
                    return 0;
            }
#endif

        case CUSTOM_CAT :
            id  = (uint8_t)*(data++);


#define EEPROM_FLOAT(name, default_val) if (!(id--)) return (float)name;

#define EEPROM_CHAR(name, default_val) EEPROM_FLOAT(name, default_val);

#define EEPROM_INT(name, default_val) EEPROM_FLOAT(name, default_val);

#define EEPROM_LONG(name, default_val) EEPROM_FLOAT(name, default_val);

#define EEPROM_BOOL(name, default_val) EEPROM_FLOAT(name, default_val);

#include "../config.h"

#undef EEPROM_BOOL
#undef EEPROM_CHAR
#undef EEPROM_INT
#undef EEPROM_LONG
#undef EEPROM_FLOAT

        default:
            return 0;
    }
}


float EEPROMStorage::write(int8_t *data, uint8_t size) {

    char t[4];

    /*for (int k1_position_indice = 0; k1_position_indice<4; k1_position_indice++)CI::echo(str((uint8_t)data[k1_position_indice]));return 0;*/
#define WRITE_RETURN(var, type)  for (int i = 0; i<4; i++)t[i] = data[i];return (var = (type)(*(float*)t));

    if (!(size--)) return 0;
    uint8_t c = (uint8_t)*(data++), id;

    switch (c) {

#ifdef ENABLE_ASSERV

        case PID_CAT :

            if (size < 6) return 0;
            id = (uint8_t)*(data++);
            if (id>=NB_PIDS) return 0;

            switch (*(data++)) {
                case 0 ://kp
                WRITE_RETURN(pids_data[id].kp, float);
                case 1 ://ki
                WRITE_RETURN(pids_data[id].ki, float);
                case 2 ://kd
                WRITE_RETURN(pids_data[id].kd, float);
                default:
                    return 0;
            }

        case LOOP_CAT :
            if (size < 6) return 0;
            id = (uint8_t)*(data++);
            if (id>=NB_LOOPS) return 0;
            switch (*(data)) {
                case 0 ://period
                WRITE_RETURN(loop_periods[id], uint16_t);
                default:
                    return 0;
            }

#endif

        case CONTINUOUS_CAT :
            if (size < 6) return 0;
            id = (uint8_t)*(data++);
            if (id>=NB_CONTINUOUS) return 0;
            switch (*(data)) {
                case 0 ://max
                WRITE_RETURN(continuous_data[id].max, float);
                default:
                    return 0;
            }

        case SERVO_CAT :
            if (size < 6) return 0;
            id = (uint8_t)*(data++);
            if (id>=NB_SERVOS) return 0;
            switch (*(data)) {
                case 0 ://min
                WRITE_RETURN(servos_data[id].min, float);
                case 1 ://max
                WRITE_RETURN(servos_data[id].max, float);
                default:
                    return 0;
            }

#ifdef ENABLE_STEPPER_CONTROL

        case STEPPER_CAT :
            if (size < 6) return 0;
            id = (uint8_t)*(data++);
            if (id>=NB_STEPPERS) return 0;
            switch (*(data)) {
                case 0 ://size
                WRITE_RETURN(steppers_data[id].size, float);
                case 1 ://steps
                WRITE_RETURN(steppers_data[id].steps, float);
                case 2 ://regulation_speed
                WRITE_RETURN(steppers_data[id].maximum_speed, float);
                case 3 ://acceleration
                WRITE_RETURN(steppers_data[id].acceleration, float);
                case 4 ://acceleration
                WRITE_RETURN(steppers_data[id].jerk, float);
                default:
                    return 0;
            }

        case CARTESIAN_GROUP_CAT :
            if (size < 6) return 0;
            id = (uint8_t)*(data++);
            if (id>=NB_CARTESIAN_GROUPS) return 0;
            switch (*(data)) {
                case 0 ://size
                WRITE_RETURN(group_maximum_speeds[id], float);
                default:
                    return 0;
            }

#endif

        case CUSTOM_CAT :
            id  = (uint8_t)*(data++);


#define EEPROM_FLOAT(name, default_val) if (!(id--)) WRITE_RETURN(name, float);

#define EEPROM_CHAR(name, default_val) if (!(id--)) WRITE_RETURN(name, char);

#define EEPROM_INT(name, default_val) if (!(id--)) WRITE_RETURN(name, int);

#define EEPROM_LONG(name, default_val) if (!(id--)) WRITE_RETURN(name, int32_t);

#define EEPROM_BOOL(name, default_val) if (!(id--)) WRITE_RETURN(name, bool);

#include "../config.h"

#undef EEPROM_BOOL
#undef EEPROM_CHAR
#undef EEPROM_INT
#undef EEPROM_LONG
#undef EEPROM_FLOAT

        default:
            return 0;
    }
}

#endif

#endif