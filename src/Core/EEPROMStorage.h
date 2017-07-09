/*
  EEPROMStorage.h - Part of TRACER

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

#ifndef CODE_SETTINGSPROFILEN_H
#define CODE_SETTINGSPROFILEN_H

#include <avr/io.h>
#include "../config.h"

/*
 * The EEPROM Storage Class
 *
 * This class manges the storage in EEPROM of all data required by the program to work.
 *
 * It stores data for :
 *      - control loops (kp, ki, kd);
 *      - Stepper Motors (steps/mm, speed, acceleration ...)
 *      - Custom data you may have specified in EEPROM_profile.h
 */

class EEPROMStorage {

#ifdef ENABLE_STEPPER_CONTROL

public :

    //Stepper motors data
    static float *const sizes;
    static float *const steps;
    static float *const maximum_speeds;
    static float *const accelerations;

private:

    //Stepper motors data
    static float *const default_sizes;
    static float *const defaultSteps;
    static float *const default_max_speeds;
    static float *const default_acceleration;

    static void read_stepper(int *indice, unsigned char axis_nb, float *size, float *steps, float *mspeed,
                             float *acceleration);

    static void write_stepper(int *indice, float size, float steps, float mspeed, float accceleration);

#endif

#ifdef ENABLE_ASSERV

public :

    //PID data
    static float *const kps;
    static float *const kis;
    static float *const kds;

private :

    //PID default data
    static float *const default_kps;
    static float *const default_kis;
    static float *const default_kds;

    static void write_pid(int *indice, float kp, float ki, float kd);

    static void read_pid(int *indice, unsigned char pid_nb, float *kps, float *kis, float *kds);

#endif

public :

    //Custom data
#define EEPROM_BOOL(name, default_value) static bool name;
#define EEPROM_CHAR(name, default_value) static char name;
#define EEPROM_INT(name, default_value) static int name;
#define EEPROM_LONG(name, default_value) static long name;
#define EEPROM_FLOAT(name, default_value) static float name;

#include "../config.h"


#undef EEPROM_BOOL
#undef EEPROM_CHAR
#undef EEPROM_INT
#undef EEPROM_LONG
#undef EEPROM_FLOAT

private :

    //Custom default data
#define EEPROM_BOOL(name, default_value) static bool default_##name;
#define EEPROM_CHAR(name, default_value) static char default_##name;
#define EEPROM_INT(name, default_value) static int default_##name;
#define EEPROM_LONG(name, default_value) static long default_##name;
#define EEPROM_FLOAT(name, default_value) static float default_##name;

#include "../config.h"


#undef EEPROM_BOOL
#undef EEPROM_CHAR
#undef EEPROM_INT
#undef EEPROM_LONG
#undef EEPROM_FLOAT

public :

    static void begin();

    static void print_stored_data();

private :

    static bool extractProfile();

    static void saveProfile();

    static void setDefaultProfile();

    //Primitive data methods

    static void write_char(int *indice, char value);
    static void write_int(int *indice, int value);
    static void write_long(int *indice, long value);
    static void write_float(int *indice, float value);

    static char read_char(int *indice);
    static int read_int(int *indice);
    static long read_long(int *indice);
    static float read_float(int *indice);
};


#endif //CODE_SETTINGSPROFILE_H
