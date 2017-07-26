/*
  EEPROMStorage.cpp - Part of TRACER

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

#include "EEPROMStorage.h"
#include "../config.h"
#include "../interface.h"
#include "../config.h"
#include "../hardware_language_abstraction.h"
#include "../Actions/ContinuousActions.h"

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
 *          i : parameter i;
 */


#define PID_CAT (uint8_t) 0
#define LOOP_CAT (uint8_t) 1
#define CONTINUOUS_CAT (uint8_t) 2
#define SERVO_CAT (uint8_t) 3
#define STEPPER_CAT (uint8_t) 4
#define CARTESIAN_GROUP_CAT (uint8_t) 5
#define CUSTOM_CAT (uint8_t) 6


void EEPROMStorage::begin() {
    setDefaultProfile();
    saveProfile();

    /*
    if (!extractProfile()) {
        setDefaultProfile();
        saveProfile();
    }*/

}


float EEPROMStorage::read(char *data, uint8_t size) {
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
                    return kps[id];
                case 1 ://ki
                    return kis[id];
                case 2 ://kd
                    return kds[id];
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
                case 0 ://size
                    return continuous_max[id];
                default:
                    return 0;
            }

        case SERVO_CAT :
            if (size < 2) return 0;
            id = (uint8_t)*(data++);
            if (id>=NB_SERVOS) return 0;
            switch (*(data)) {
                case 0 ://size
                    return servos_min[id];
                case 1 ://size
                    return servos_max[id];
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
                    return sizes[id];
                case 1 ://steps
                    return steps[id];
                case 2 ://speed
                    return maximum_speeds[id];
                case 3 ://acceleration
                    return accelerations[id];
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


float EEPROMStorage::write(char *data, uint8_t size) {

    char t[4];

    /*for (int i = 0; i<4; i++)CI::echo(String((uint8_t)data[i]));return 0;*/
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
                    WRITE_RETURN(kps[id], float);
                case 1 ://ki
                    WRITE_RETURN(kis[id], float);
                case 2 ://kd
                    WRITE_RETURN(kds[id], float);
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
                    WRITE_RETURN(continuous_max[id], float);
                default:
                    return 0;
            }

        case SERVO_CAT :
            if (size < 6) return 0;
            id = (uint8_t)*(data++);
            if (id>=NB_SERVOS) return 0;
            switch (*(data)) {
                case 0 ://min
                    WRITE_RETURN(servos_min[id], float);
                case 1 ://max
                    WRITE_RETURN(servos_max[id], float);
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
                    WRITE_RETURN(sizes[id], float);
                case 1 ://steps
                    WRITE_RETURN(steps[id], float);
                case 2 ://speed
                    WRITE_RETURN(maximum_speeds[id], float);
                case 3 ://acceleration
                    WRITE_RETURN(accelerations[id], float);
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

bool EEPROMStorage::extractProfile() {
    //SAVE MARK CHECK
    if ((EEPROM_read(0) != 79) || (EEPROM_read(1) != 97))
        return false;
    if (EEPROM_read(EEPROM_read(2)) != 239)
        return false;

    int vi = 3;
    int *indice = &vi;

#ifdef ENABLE_STEPPER_CONTROL
    for (uint8_t id = 0; id < NB_STEPPERS; id++)
        read_stepper(indice, id, sizes, steps, (float *) maximum_speeds, accelerations);

    for (uint8_t id = 0; id < NB_CARTESIAN_GROUPS; id++)
        group_maximum_speeds[id] = read_float(indice);

#endif


#ifdef ENABLE_ASSERV
    //Read PIDs
    for (uint8_t id = 0; id < NB_PIDS; id++)
        read_pid(indice, id, kps, kis, kds);

    //Read Control Loops
    for (uint8_t id = 0; id < NB_LOOPS; id++)
        loop_periods[id] = (uint16_t) read_int(indice);

#endif

    //Write Continuous Actions maximums :
    for (uint8_t continuous = 0; continuous<NB_CONTINUOUS; continuous++) {
        continuous_max[continuous] = read_float(indice);
    }

    //Write Servo Actions minimums and maximums :
    for (uint8_t servo = 0; servo<NB_SERVOS; servo++) {
        servos_min[servo] = read_float(indice);
        servos_max[servo] = read_float(indice);
    }

#define EEPROM_BOOL(name, default_val) name = (bool) read_char(indice);

#define EEPROM_CHAR(name, default_val) name = read_char(indice);

#define EEPROM_INT(name, default_val) name = read_int(indice);

#define EEPROM_LONG(name, default_val) name = read_int32_t(indice);

#define EEPROM_FLOAT(name, default_val) name = read_float(indice);

#include "../config.h"

#undef EEPROM_BOOL
#undef EEPROM_CHAR
#undef EEPROM_INT
#undef EEPROM_LONG
#undef EEPROM_FLOAT

    return true;
}

void EEPROMStorage::saveProfile() {
    //SAVE MARK (two symetrical primes happy numbers)
    EEPROM_write(0, 79);
    EEPROM_write(1, 97);
    int vi = 3;
    int *indice = &vi;

#ifdef ENABLE_STEPPER_CONTROL
    //Write axis
    for (int axis = 0; axis < NB_STEPPERS; axis++) {
        write_stepper(indice, sizes[axis], steps[axis], maximum_speeds[axis], accelerations[axis]);
    }

    //Write speed groups
    for (int group = 0; group < NB_CARTESIAN_GROUPS; group++) {
        write_float(indice, group_maximum_speeds[group]);
    }
#endif

#ifdef ENABLE_ASSERV
    //Write PIDs
    for (uint8_t  pid = 0; pid < NB_PIDS; pid++) {
        write_pid(indice, kps[pid], kis[pid], kds[pid]);
    }

    //Write Periods
    for (uint8_t  loop = 0; loop < NB_LOOPS; loop++) {
        write_int(indice, (int) loop_periods[loop]);
    }

#endif

    //Write Continuous Actions maximums :
    for (uint8_t continuous = 0; continuous<NB_CONTINUOUS; continuous++) {
        write_float(indice, continuous_max[continuous]);
    }

    //Write Servo Actions minimums and maximums :
    for (uint8_t servo = 0; servo<NB_SERVOS; servo++) {
        write_float(indice, servos_min[servo]);
        write_float(indice, servos_max[servo]);
    }


#define EEPROM_BOOL(name, default_val) write_char(indice, (char) name);

#define EEPROM_CHAR(name, default_val) write_char(indice, name);

#define EEPROM_INT(name, default_val) write_int(indice, name);

#define EEPROM_LONG(name, default_val) write_int32_t(indice, name);

#define EEPROM_FLOAT(name, default_val) write_float(indice, name);

#include "../config.h"

#undef EEPROM_BOOL
#undef EEPROM_CHAR
#undef EEPROM_INT
#undef EEPROM_LONG
#undef EEPROM_FLOAT


    //END MARK : the last happy number inferior to 255
    EEPROM_write(*indice, 239);

    //Print data size for size check : the "indice" indexed value must equal 239
    EEPROM_write(2, (uint8_t) *indice);
}

void EEPROMStorage::setDefaultProfile() {

    //---------------------------------------------Steppers---------------------------------------------

#ifdef ENABLE_STEPPER_CONTROL

    //Set default steppers data
#define STEPPER_DATA(i, j, size, v_steps, speed, acceleration)\
    sizes[i] = size;\
    steps[i] = v_steps;\
    maximum_speeds[i] = speed;\
    accelerations[i] = acceleration;

#include "../config.h"

#undef STEPPER_DATA

    //Set default group speeds
#define CARTESIAN_GROUP(i, m0, m1, m2, s)\
    group_maximum_speeds[i] = s;

#include "../config.h"

#undef CARTESIAN_GROUP

#endif

    //---------------------------------------------Asserv---------------------------------------------

#ifdef ENABLE_ASSERV

    //PIDs kp, ki, kd
#define PID(i, name, kp, ki, kd)\
    kps[i] = kp;\
    kis[i] = ki;\
    kds[i] = kd;

#include "../config.h"

#undef PID

    //Control loops periods
#define  LOOP_FUNCTION(indice, name, period_ms)\
    loop_periods[indice] = period_ms;

#include "../config.h"

#undef LOOP_FUNCTION

#endif


    //---------------------------------------------Actions---------------------------------------------

    //Continuous max
#define CONTINUOUS(i, name, powerPin, maxValue)\
    continuous_max[i] = maxValue;

#include "../config.h"

#undef CONTINUOUS

    //Servo min and max
#define SERVO(i, name, dataPin, minValue, maxValue)\
    servos_min[i] = minValue; servos_max[i] = maxValue;

#include "../config.h"

#undef SERVO


    //---------------------------------------------Custom---------------------------------------------

#define EEPROM_BOOL(name, default_val) name = default_val;

#define EEPROM_CHAR(name, default_val) name = default_val;

#define EEPROM_INT(name, default_val) name = default_val;

#define EEPROM_LONG(name, default_val) name = default_val;

#define EEPROM_FLOAT(name, default_val) name = default_val;

#include "../config.h"

#undef EEPROM_BOOL
#undef EEPROM_CHAR
#undef EEPROM_INT
#undef EEPROM_LONG
#undef EEPROM_FLOAT


}

//##################################################DATA_WRITE_METHODS##################################################

#ifdef ENABLE_STEPPER_CONTROL

void EEPROMStorage::read_stepper(int *indice, uint8_t axis_nb, float *size, float *steps, float *mspeed,
                                 float *acceleration) {
    *(size + axis_nb) = read_float(indice);
    *(steps + axis_nb) = read_float(indice);
    *(mspeed + axis_nb) = read_float(indice);
    *(acceleration + axis_nb) = read_float(indice);
}

void EEPROMStorage::write_stepper(int *indice, float size, float steps, float mspeed, float accceleration) {
    write_float(indice, size);
    write_float(indice, steps);
    write_float(indice, mspeed);
    write_float(indice, accceleration);
}

#endif

#ifdef ENABLE_ASSERV

void EEPROMStorage::read_pid(int *indice, uint8_t pid_nb, float *kps, float *kis, float *kds) {
    *(kps + pid_nb) = read_float(indice);
    *(kis + pid_nb) = read_float(indice);
    *(kds + pid_nb) = read_float(indice);
}

void EEPROMStorage::write_pid(int *indice, float kp, float ki, float kd) {
    write_float(indice, kp);
    write_float(indice, ki);
    write_float(indice, kd);
}

#endif

//###############################################PRIMITIVE_WRITE_METHODS################################################

char EEPROMStorage::read_char(int *indice) {
    return (char) EEPROM_read((*indice)++);
}

int EEPROMStorage::read_int(int *indice) {
    uint8_t t[2];
    for (int i = 0; i < 2; i++) {
        t[i] = EEPROM_read((*indice)++);
    }
    return *(int *) t;
}

int32_t EEPROMStorage::read_int32_t(int *indice) {
    uint8_t t[4];
    for (int i = 0; i < 4; i++) {
        t[i] = EEPROM_read((*indice)++);
    }
    return *(int32_t *) t;
}

float EEPROMStorage::read_float(int *indice) {
    uint8_t t[4];
    for (int i = 0; i < 4; i++) {
        t[i] = EEPROM_read((*indice)++);
    }
    return *(float *) t;
}

void EEPROMStorage::write_char(int *indice, char value) {
    EEPROM_write((*indice)++, (uint8_t) value);
}

void EEPROMStorage::write_int(int *indice, int value) {
    uint8_t *t = (uint8_t *) &value;
    for (int i = 0; i < 2; i++)
        EEPROM_write((*indice)++, t[i]);
}

void EEPROMStorage::write_int32_t(int *indice, int32_t value) {
    uint8_t *t = (uint8_t *) &value;
    for (int i = 0; i < 4; i++)
        EEPROM_write((*indice)++, t[i]);
}

void EEPROMStorage::write_float(int *indice, float value) {
    uint8_t *t = (uint8_t *) &value;
    for (int i = 0; i < 4; i++)
        EEPROM_write((*indice)++, t[i]);
}


#ifdef ENABLE_TREE_INTERFACE

void EEPROMStorage::send_structure() {
    /*
     * Syntax :
     * reset_tree (beginning) :         0;
     * confirm_tree (end) :             1;
     * go_upper :                       2;
     * go_lower(i, name) :              3, i, name
     * create_leaf(i, name) :           4, i, name
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
    //For each Stepper, mark a case for the size, the number of steps per unit, and maximum speed and acceleration.
#define STEPPER_DATA(indice, ...) EEPROM_LOWER(indice, indice);\
    EEPROM_LEAF(0, size);EEPROM_LEAF(1, steps);EEPROM_LEAF(2, max_speed);EEPROM_LEAF(3, max_acceleration);EEPROM_UPPER;

#include "../config.h"

#undef STEPPER_DATA

    EEPROM_UPPER;

    //---------------------------------------------Cartesian Groups---------------------------------------------

    EEPROM_LOWER(CARTESIAN_GROUP_CAT, speed_groups);
    //For each group, mark a case for its maximum speed
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



void EEPROMStorage::print_stored_data() {

    //Stepper motors
#ifdef ENABLE_STEPPER_CONTROL
    CI::echo("Stepper Motors : ");
    for (uint8_t stepper = 0; stepper < NB_STEPPERS; stepper++) {
        CI::echo("Stepper " + String(stepper));
        CI::echo("size : " + String(sizes[stepper]));
        CI::echo("steps : " + String(steps[stepper]));
        CI::echo("max speed : " + String(maximum_speeds[stepper]));
        CI::echo("acceleration : " + String(accelerations[stepper]) + "\n");
    }

    CI::echo("Speed Groups : ");
    for (int group = 0; group < NB_CARTESIAN_GROUPS; group++) {
        CI::echo("Group " + String(group));
        CI::echo("max speed : " + String(group_maximum_speeds[group]) + "\n");
    }

#endif

#ifdef ENABLE_ASSERV

    //PIDs
    CI::echo("PIDS : ");
    for (int pid = 0; pid < NB_PIDS; pid++) {
        CI::echo("PID " + String(pid));
        CI::echo("kp : " + String(kps[pid]));
        CI::echo("ki : " + String(kis[pid]));
        CI::echo("kd : " + String(kds[pid]) + "\n");
    }

    //Control Loops
    CI::echo("Control Loops : ");
    for (int loop = 0; loop < NB_LOOPS; loop++) {
        CI::echo("Loop " + String(loop));
        CI::echo("period (ms) : " + String(loop_periods[loop]) + "\n");
    }

#endif

    //Continuous Actions
    CI::echo("Continuous Actions : ");
    for (int continuous = 0; continuous < NB_CONTINUOUS; continuous++) {
        CI::echo("Continuous " + String(continuous));
        CI::echo("maximum : " + String(continuous_max[continuous]) + "\n");
    }

    //Servo Actions
    CI::echo("Servo Actions : ");
    for (int servo = 0; servo < NB_SERVOS; servo++) {
        CI::echo("Loop " + String(servo));
        CI::echo("minimum : " + String(servos_min[servo]));
        CI::echo("maximum : " + String(servos_max[servo]) + "\n");
    }

    //Custom Data
    CI::echo("Custom Data : ");
    //Custom data
#define EEPROM_BOOL(name, default_value) CI::echo(String(#name)+" : "+String(name));
#define EEPROM_CHAR(name, default_value) CI::echo(String(#name)+" : "+String(name));
#define EEPROM_INT(name, default_value) CI::echo(String(#name)+" : "+String(name));
#define EEPROM_LONG(name, default_value) CI::echo(String(#name)+" : "+String(name));
#define EEPROM_FLOAT(name, default_value) CI::echo(String(#name)+" : "+String(name));

#include "../config.h"

#undef EEPROM_BOOL
#undef EEPROM_CHAR
#undef EEPROM_INT
#undef EEPROM_LONG
#undef EEPROM_FLOAT

    CI::echo(" ");

}


#endif

#define m EEPROMStorage

#ifdef ENABLE_STEPPER_CONTROL

//Steppers definitions :
float ts[NB_STEPPERS];
float tstt[NB_STEPPERS];
float ms[NB_STEPPERS];
float ta[NB_STEPPERS];


float *const m::sizes = ts;
float *const m::steps = tstt;
float *const m::maximum_speeds = ms;
float *const m::accelerations = ta;

float gms[NB_CARTESIAN_GROUPS];
float *const m::group_maximum_speeds = gms;

#endif

#ifdef ENABLE_ASSERV

//PIDs definitions :
float tkps[NB_PIDS];
float tkis[NB_PIDS];
float tkds[NB_PIDS];

float *const m::kps = tkps;
float *const m::kis = tkis;
float *const m::kds = tkds;

uint16_t tlp[NB_LOOPS];
uint16_t *const m::loop_periods = tlp;


#endif

//Actions
float tcnt[NB_CONTINUOUS];
float *const m::continuous_max = tcnt;

float tsmi[NB_SERVOS];
float tsma[NB_SERVOS];
float *const m::servos_min = tsmi;
float *const m::servos_max = tsma;

//Custom Parameters Definition
#define EEPROM_BOOL(name, default_value) bool m::name;
#define EEPROM_CHAR(name, default_value) char m::name;
#define EEPROM_INT(name, default_value) int m::name;
#define EEPROM_LONG(name, default_value) int32_t m::name;
#define EEPROM_FLOAT(name, default_value) float m::name;

#include "../config.h"

#undef EEPROM_BOOL
#undef EEPROM_CHAR
#undef EEPROM_INT
#undef EEPROM_LONG
#undef EEPROM_FLOAT

