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
#include <config.h>
#include "../Actions/ContinuousActions.h"


/*
 * initialise_data : this function initialises properly the EEPROM cache :
 *  It reads the profile saved in the EEPROM, and if it is corrupted, resets the default profile.
 *
 */

void EEPROMStorage::init() {
    set_default_profile();
    write_profile();

    /*
    if (!extract_profile()) {
        setDefaultProfile();
        write_profile();
    }*/

}


/*
 * read_data : this function reads the data at index [index] in the EEPROM.
 *
 */

void EEPROMStorage::read_data(uint16_t *index, size_t size, uint8_t *data) {

    for (uint16_t i = 0; i<size; i++) {
        data[i] = EEPROM_read(*(index++));
    }
}


/*
 * write_data : this function writes the data at index [index] in the EEPROM.
 *
 */

void EEPROMStorage::write_data(uint16_t *index, size_t size, uint8_t *const data) {

    for (uint16_t i = 0; i<size; i++) {
        EEPROM_write(*(index++), data[i]);
    }
}


/*
 * extract_profile : this function reads the entire profile in the EEPROM.
 * 
 *  Returns true if the profile was correctly read_integer, returns false otherwise.
 *  
 */

bool EEPROMStorage::extract_profile() {
    //SAVE MARK CHECK
    if ((EEPROM_read(0) != 79) || (EEPROM_read(1) != 97))
        return false;
    if (EEPROM_read(EEPROM_read(2)) != 239)
        return false;

    uint16_t vi = 3;
    uint16_t *index = &vi;

#ifdef ENABLE_STEPPER_CONTROL

    //Write stepper data
    read_data(index, NB_STEPPERS * sizeof(stepper_data_t), (uint8_t*)steppers_data);

    //Write regulation_speed groups
    read_data(index, NB_CARTESIAN_GROUPS * sizeof(float), (uint8_t*)group_maximum_speeds);

    read_data(index, sizeof(coordinate_interface_data_t), (uint8_t*)&coordinate_interface_data);

#endif

#ifdef ENABLE_ASSERV
    //Write PIDs
    read_data(index, NB_PIDS * sizeof(pid_data_t), (uint8_t*)pids_data);

    //Write Periods
    read_data(index, NB_LOOPS * sizeof(uint16_t), (uint8_t*)loop_periods);

#endif

    //Write Continuous Actions maximums :
    read_data(index, NB_CONTINUOUS * sizeof(continuous_data_t), (uint8_t*)continuous_data);


    //Write Servo Actions minimums and maximums :
    read_data(index, NB_SERVOS * sizeof(servo_data_t), (uint8_t*)servos_data);


    //Write the custom data;
    read_data(index, sizeof(custom_data_t), (uint8_t*)&custom_data);


    return true;
}


/*
 * write_profile : this function writes the current profile in memory.
 *
 */

void EEPROMStorage::write_profile() {

    //SAVE MARK (two symetrical primes happy numbers)
    EEPROM_write(0, 79);
    EEPROM_write(1, 97);

    uint16_t vi = 3;
    uint16_t *index = &vi;

#ifdef ENABLE_STEPPER_CONTROL

    //Write stepper data
    write_data(index, NB_STEPPERS * sizeof(stepper_data_t), (uint8_t*) steppers_data);

    //Write regulation_speed groups
    write_data(index, NB_CARTESIAN_GROUPS * sizeof(float), (uint8_t*) group_maximum_speeds);

    //Write the custom data;
    write_data(index, sizeof(coordinate_interface_data_t), (uint8_t*)&coordinate_interface_data);

#endif

#ifdef ENABLE_ASSERV

    //Write PIDs
    write_data(index, NB_PIDS * sizeof(pid_data_t), (uint8_t*) pids_data);

    //Write Periods
    write_data(index, NB_LOOPS * sizeof(uint16_t), (uint8_t*) loop_periods);

#endif

    //Write Continuous Actions maximums :
    write_data(index, NB_CONTINUOUS * sizeof(continuous_data_t), (uint8_t*) continuous_data);


    //Write Servo Actions minimums and maximums :
    write_data(index, NB_SERVOS * sizeof(servo_data_t), (uint8_t*) servos_data);


    //Write the custom data;
    write_data(index, sizeof(custom_data_t), (uint8_t*)&custom_data);


    //END MARK : the last happy number inferior to 255
    EEPROM_write(*index, 239);

    //Print data size for size check : the "index" indexed value must equal 239
    EEPROM_write(2, (uint8_t) *index);
}


/*
 * set_default_profile : this function writes the default profile in the EEPROM.
 *
 */

void EEPROMStorage::set_default_profile() {

    //---------------------------------------------Steppers---------------------------------------------

#ifdef ENABLE_STEPPER_CONTROL
    
    

    //Set default steppers data

    stepper_data_t *step_p;

#define STEPPER_DATA(i, j, d_size, d_steps, d_speed, d_acc, d_jerk)\
    step_p = steppers_data+i;\
    step_p->size = d_size;\
    step_p->steps = d_steps;\
    step_p->maximum_speed = d_speed;\
    step_p->acceleration = d_acc;\
    step_p->jerk = d_jerk;

#include <config.h>

#undef STEPPER_DATA

    
    //Set default group steppers_speeds
#define CARTESIAN_GROUP(i, m0, m1, m2, s)\
    group_maximum_speeds[i] = s;

#include <config.h>

#undef CARTESIAN_GROUP

#endif

    
    //---------------------------------------------Asserv---------------------------------------------
    
#ifdef ENABLE_ASSERV

    //PIDs kp, ki, kd
    pid_data_t *pid_p;

#define PID(i, name, d_kp, d_ki, d_kd)\
    pid_p = pids_data+i;\
    pid_p->kp = d_kp;\
    pid_p->ki = d_ki;\
    pid_p->kd = d_kd;

#include <config.h>

#undef PID

    //Control loops periods
#define  LOOP_FUNCTION(index, name, period_ms)\
    loop_periods[index] = period_ms;

#include <config.h>

#undef LOOP_FUNCTION

#endif


    //---------------------------------------------Actions---------------------------------------------

    //Continuous ending
#define CONTINUOUS(i, name, powerPin, maxValue)\
    (continuous_data+i)->max = maxValue;

#include <config.h>

#undef CONTINUOUS

    //Servo beginning and ending
    
    servo_data_t *servo_p;

#define SERVO(i, name, dataPin, minValue, maxValue)\
    servo_p = servos_data+i;\
    servo_p->min = minValue;\
    servo_p->max = maxValue;

#include <config.h>

#undef SERVO

    //---------------------------------------------Custom---------------------------------------------

    custom_data = custom_data_t();

}


#define m EEPROMStorage


#ifdef ENABLE_STEPPER_CONTROL

//Steppers :
stepper_data_t tsdt[NB_STEPPERS];
stepper_data_t *const m::steppers_data = tsdt;

//Cartesian groups
float gms[NB_CARTESIAN_GROUPS];
float *const m::group_maximum_speeds = gms;

//Coordinates Abstraction
coordinate_interface_data_t m::coordinate_interface_data = coordinate_interface_data_t();
#endif


#ifdef ENABLE_ASSERV

//PIDs :
pid_data_t tpdt[NB_PIDS];
pid_data_t *const m::pids_data = tpdt;

//Loops
float tlp[NB_LOOPS];
float *const m::loop_periods = tlp;

#endif

//Actions
continuous_data_t tcnt[NB_CONTINUOUS];
continuous_data_t *const m::continuous_data = tcnt;

servo_data_t tserdt[NB_SERVOS];
servo_data_t *const m::servos_data = tserdt;

custom_data_t m::custom_data = custom_data_t();


