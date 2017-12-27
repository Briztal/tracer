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

#include <config.h>

#include "_eeprom_storage_data.h"

#include <hardware_language_abstraction.h>


/*
 * The EEPROM Storage Class
 *
 * This class manges the storage in EEPROM of all data required by the program to work.
 *
 * It stores data for :
 *
 *      - PIDs (kp, ki, kd);
 *      - Interaction Loops (period_ms)
 *      - Continuous Actuators (ending)
 *      - Servo Actuators(min and ending)
 *      - Stepper Motors (sizes, steps/unit, regulation_speed, acceleration)
 *      - Cartesian Speed Groups (steps/mm, regulation_speed, acceleration ...)
 *      - Custom data you may have specified in your config_files.h
 */

class EEPROMStorage {


public :

    static void initialise_data();

    //----------------------------------------------EEPROM methods------------------------------------------------------

public :

    //Save the profile in the EEPROM
    static void write_profile();


    //Reset to the default profile, and save in the EEPROM
    static void set_default_profile();

private :

    //Read the profile from the EEPROM
    static bool extract_profile();


    //Write data in the EEPROM
    static void read_data(uint16_t * index, size_t size, uint8_t *data);

    //Read data in the EEPROM
    static void write_data(uint16_t * index, size_t size, uint8_t *const data);


    //-------------------------------------------Stepper control data---------------------------------------------------


#ifdef ENABLE_STEPPER_CONTROL
/*
public :

    //Stepper motors data
    static stepper_data_t *const steppers_data;

    //Group speeds
    static float *const group_maximum_speeds;

    //Coordinate interface structure
    static coordinate_interface_data_t coordinate_interface_data;

 */
#endif


    //---------------------------------------------Closed loops data----------------------------------------------------

#ifdef ENABLE_CONTROL_LOOPS

public :

    /*
    //GENERATE_PID data
    static pid_data_t *const pids_data;

    //Interaction loops data
    static float *const loop_periods;
*/
#endif


    //--------------------------------------------------Actuators---------------------------------------------------------

public :

    /*
    //Continuous actions
    static continuous_data_t *const continuous_data;

    //Servo actions
    static servo_data_t *const servos_data;
*/

    //------------------------------------------------Custom data-------------------------------------------------------

public :
    //static custom_data_t custom_data;


};


#endif
