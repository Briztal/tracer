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
  aint32_t with TRACER.  If not, see <http://www.gnu.org/licenses/>.

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
 *
 *      - PIDs (kp, ki, kd);
 *      - Control Loops (period_ms)
 *      - Continuous Actions (max)
 *      - Servo Actions(min and max)
 *      - Stepper Motors (sizes, steps/unit, speed, acceleration)
 *      - Cartesian Speed Groups (steps/mm, speed, acceleration ...)
 *      - Custom data you may have specified in your config.h
 */

class EEPROMStorage {


public :

    static float read(char *data, uint8_t size);

    static float write(char *data, uint8_t size);

    static void setDefaultProfile();

    static void saveProfile();



#ifdef ENABLE_STEPPER_CONTROL

public :

    //Stepper motors data
    static float *const sizes;
    static float *const steps;
    static volatile float *const maximum_speeds;
    static float *const accelerations;
    static float *const group_maximum_speeds;


private:

    static void read_stepper(int *indice, uint8_t axis_nb, float *size, float *steps, float *mspeed,
                             float *acceleration);

    static void write_stepper(int *indice, float size, float steps, float mspeed, float accceleration);

#endif

#ifdef ENABLE_ASSERV

public :

    //PID data
    static float *const kps;
    static float *const kis;
    static float *const kds;

    //Control loops data
    static uint16_t *const loop_periods;

private :

    static void write_pid(int *indice, float kp, float ki, float kd);

    static void read_pid(int *indice, uint8_t pid_nb, float *kps, float *kis, float *kds);

#endif

//Actions

public :
    static float *const continuous_max;
    static float *const servos_min;
    static float *const servos_max;

public :

    //Custom data
#define EEPROM_BOOL(name, default_value) static bool name;
#define EEPROM_CHAR(name, default_value) static char name;
#define EEPROM_INT(name, default_value) static int name;
#define EEPROM_LONG(name, default_value) static int32_t name;
#define EEPROM_FLOAT(name, default_value) static float name;

#include "../config.h"


#undef EEPROM_BOOL
#undef EEPROM_CHAR
#undef EEPROM_INT
#undef EEPROM_LONG
#undef EEPROM_FLOAT

public :

    static void begin();

    static void print_stored_data();

    static void send_structure();

private :

    static bool extractProfile();

    //Primitive data methods

    static void write_char(int *indice, char value);
    static void write_int(int *indice, int value);
    static void write_int32_t(int *indice, int32_t value);
    static void write_float(int *indice, float value);

    static char read_char(int *indice);
    static int read_int(int *indice);
    static int32_t read_int32_t(int *indice);
    static float read_float(int *indice);

};


#endif
