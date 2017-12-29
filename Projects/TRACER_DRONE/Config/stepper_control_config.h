/*
  stepper_control_config.h - Part of TRACER

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

/*
 * This file configures the Stepper control module.
 */

//------------------------------------------------------ Steppers ------------------------------------------------------

/*
 * The stepper motors are the physical actuators the module will manipulate. Each one represents a physical axis of
 *      your machine.
 */

//Number of stepper motors, must be set accordingly to the next lines
#define NB_STEPPERS 9

/*
 * Steppers settings : for each stepper of the machine, put one line like behind, and provide all parameters;
 *
 *  STEPPER(i, signature, relative, pin_step, pin_dir, positive_dir,  power_pin, enable_value, endstop_min_pin, on_value_min, endstop_max_pin, on_value_max)
 *
 *  - i : the index of the stepper. Indices start at 0, and are strictly consecutive;
 *  - sig : the signature of the stepper : constant that must be set to 2 ^ i;
 *  - pin_step : the pin used to trigger a step;
 *  - pin_dir : the pin used to set the step direction;
 *  - positive_dir : the boolean value for the positive direction (makes coordinates increase);
 *  - power_pin : the pin used to power the steppers;
 *  - enable_value : the boolean value to provide to power up steppers;
 */

#ifdef STEPPER

//      id, sig,    relat.  pStep,  pDir,   dir+    pPower, vEnab)
STEPPER(0,  1,      0,      7,      8,      LOW,    9,      LOW)
STEPPER(1,  2,      0,      10,     11,     LOW,    12,     LOW)
STEPPER(2,  4,      0,      18,     17,     LOW,    16,     LOW)
STEPPER(3,  8,      0,      15,     14,     LOW,    13,     LOW)
STEPPER(4,  16,     0,      24,     25,     LOW,    26,     LOW)
STEPPER(5,  32,     1,      27,     28,     LOW,    29,     LOW)
STEPPER(6,  64,     1,      30,     31,     LOW,    32,     LOW)
STEPPER(7,  128,    1,      39,     38,     LOW,    37,     LOW)
STEPPER(8,  256,    1,      36,     35,     LOW,    34,     LOW)

/*

//      id, sig,    relat.  pStep,  pDir,   dir+    pPower, vEnab,  pMin,   VMin,   pMax,   vMax)
STEPPER(0,  1,      0,      7,      8,      LOW,    9,      LOW,    A21,    HIGH,   0,      HIGH)
STEPPER(1,  2,      0,      10,     11,     LOW,    12,     LOW,    A22,    HIGH,   0,      HIGH)
STEPPER(2,  4,      0,      18,     17,     LOW,    16,     LOW,    A10,    HIGH,   0,      HIGH)
STEPPER(3,  8,      0,      15,     14,     LOW,    13,     LOW,    A11,    HIGH,   0,      HIGH)
STEPPER(4,  16,     0,      24,     25,     LOW,    26,     LOW,    A25,    HIGH,   0,      HIGH)
STEPPER(5,  32,     1,      27,     28,     LOW,    29,     LOW,    0,      HIGH,   0,      HIGH)
STEPPER(6,  64,     1,      30,     31,     LOW,    32,     LOW,    0,      HIGH,   0,      HIGH)
STEPPER(7,  128,    1,      39,     38,     LOW,    37,     LOW,    0,      HIGH,   0,      HIGH)
STEPPER(8,  256,    1,      36,     35,     LOW,    34,     LOW,    0,      HIGH,   0,      HIGH)

*/

#endif



/*
 * Steppers data settings : every stepper has data stored in the EEPROMMap. You must provide default values
 *  for every stepper, using command STEPPER_DATA, and the same indexation than in the last paragraph;
 *
 * for each stepper of the machine, put one line like behind, and provide all parameters :
 *
 * STEPPER_DATA(id, letter, size, steps_per_unit, speed, acceleration, jerk)
 *
 *  - i : the index of the stepper. Indices start at 0, and are strictly consecutive;
 *  - steps_per_unit : the default steps_per_unit per unit for stepper i;
 *  - speed : the default maximum speed for stepper i;
 *  - acceleration : the default maximal acceleration for stepper i;
 *  - jerk : the default maximum jerk for stepper i;
 *
 */

#ifdef STEPPER_DATA

//              id, steps_per_unit,  speed,  acceler.,   jerk)
STEPPER_DATA(   0,  80.16,  1000,    1000,      20.)
STEPPER_DATA(   1,  80.16,  1000.,   1000,      20.)
STEPPER_DATA(   2,  80.16,  1000.,   1000,      20.)
STEPPER_DATA(   3,  80.16,  1000.,   1000,      20.)
STEPPER_DATA(   4,  80.16,  1000.,   1000.,      20.)
STEPPER_DATA(   5,  80.16,  1000.,   1000.,      20.)
STEPPER_DATA(   6,  80.16,  1000.,   1000.,      20.)
STEPPER_DATA(   7,  80.16,  1000.,   1000.,      20.)
STEPPER_DATA(   8,  80.16,  1000.,   1000.,      20.)


#endif

//---------------------------------------------------- Cartesian groups ------------------------------------------------

/*
 * In order to regulate speed, you must specify all cartesian groups of your machine.
 *
 *  A cartesian group is a group of at most 3 coordinates, in which the global speed, that will be regulated,
 *      can be defined as the norm of the speed vector, restricted to these coordinates.
 *
 *  WARNING : These coordinates are high level coordinates.
 *
 *  Stepper coordinates are images of high_level coordinates through the geometry_translate function.
 *
 *  They might have no physical meaning for stepper, but are used to control the machine.
 *
 *  Ex :
 *      A polar machine har a polar stepper coordinate system, but can be controlled with a cartesian
 *          high level coordinate system, as long as the translation from (x, y) to (theta, phi) is known,
 *          and implemented in geometry_translate.
 *
 *      In this example, there will be one cartesian group, containing x and y axis;
 */

//The number of cartesian groups;
#define NB_CARTESIAN_GROUPS 8


/*
 * To define a cartesian group, you may use the following command :
 *
 *  CARTESIAN_GROUP(id, s0, s1, s2, max_speed)
 *      - i : the index of the cartesian group. Indices start at 0, and are strictly consecutive;
 *      - s{0, 1, 2} : indices of steppers belonging to the group. If it contains less than 3 steppers,
 *          pad with one or two -1;
 *      - max_speed : the speed group's maximum speed;
 */

#ifdef CARTESIAN_GROUP

//              id,     a0      a1      a2      maxSpeed
//Carriage 0
CARTESIAN_GROUP(0,      0,      2,      4,      1000     )
//Carriage 1
CARTESIAN_GROUP(1,      1,      2,      4,     1000     )
//Carriage 2
CARTESIAN_GROUP(2,      1,      3,      4,     1000     )
//Carriage 3
CARTESIAN_GROUP(3,      0,      3,      4,     1000     )
//Extruder 0
CARTESIAN_GROUP(4,      5,      -1,     -1,     1000     )
//Extruder 1
CARTESIAN_GROUP(5,      6,      -1,     -1,     1000     )
//Extruder 2
CARTESIAN_GROUP(6,      7,      -1,     -1,     1000     )
//Extruder 3
CARTESIAN_GROUP(7,      8,      -1,     -1,     1000     )

#endif

//------------------------------------------------ Kinematics Core Version ---------------------------------------------

/*
 * The version of the Stepper Interaction Kinematics TaskScheduler you want to use. You have 3 versions available :
 *  - 0 : a basic algorithm for less-than-32 bit processor, for cartesian-by-group machines. Only lines available.
 *  - 1 : a better algorithm for 32 bits processors with FPU;
 *  - 2 : a more advanced algorithm for 32 bits processors with FPU. Recommended for complex machines;
 */

//The version of KinematicsCore
#define CORE_VERSION 2

//-------------------------------------------------- Distance Bounds ---------------------------------------------------

//The minimum number of steps_per_unit between two consecutive positions;
#define MINIMUM_DISTANCE_LIMIT 8

//The ideal number of steps_per_unit between two consecutive positions;
#define DISTANCE_TARGET 11

//The maximum number of steps_per_unit between two consecutive positions;
#define MAXIMUM_DISTANCE_LIMIT 14

//------------------------------------------------------ Data types ----------------------------------------------------

/* The signature type. The type size depends on the number of steppers you want to control. Set :
 *  - uint8_t for at most 8 steppers;
 *  - uint16_t for at most 16 steppers;
 *  - uint32_t for at most 32 steppers;
 *  - uint64_t for at most 64 steppers; (not tested, tell me if it works !)
 */

#define sig_t uint32_t

//The delay type
#define delay_t uint32_t


//--------------------------------------------------------- Log --------------------------------------------------------

//Log settings. Uncomment those lines to output (resp) position and overflow marker in Monitor
#define position_log

//-------------------------------------------------------- Other -------------------------------------------------------

//The maximum number of pre-processable movements.
#define MOVEMENT_DATA_QUEUE_SIZE 20

//The maximum number of pre-processable sub_movements.
#define SUB_MOVEMENT_DATA_QUEUE_SIZE 40

//The maximum number of curve points in a GCode Command (used in Bezier curves for ex)
#define MAX_CURVE_POINTS 5
