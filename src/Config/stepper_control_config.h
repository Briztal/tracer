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


//-----------------------------------------------High Level Coordinate System-------------------------------------------

/*
 * The high level coordinate system is the system you will use to send explicit coordinates. They will be interpreted
 *  by the kernel, who will trace you movements.
 *
 *  High level coordinates are not meant to have a physical meaning (a particular stepper orientation).
 *
 *  you must also define the translation from high level coordinates to stepper positions, in implementing
 *      the translate and revert methods in the MachineAbstraction class.
 *
 */

//Number of axis in the high level coordinates system
#define NB_AXIS 9

//Axis settings : for each axis of the machine, put one line like behind, and provide all parameters//TODO DOC
#ifdef AXIS

//AXIS(i, j, si, st, sp, a)

//      id, letter,)
AXIS(   0,  'V')
AXIS(   1,  'W')
AXIS(   2,  'X')
AXIS(   3,  'Y')
AXIS(   4,  'Z')
AXIS(   5,  'A')
AXIS(   6,  'B')
AXIS(   7,  'C')
AXIS(   8,  'D')

#endif

//------------------------------------------------------Stepper Motors--------------------------------------------------

/*
 * The stepper motors are the physical actuators the kernel will manipulate. Each one represents a physical axis of
 *      your machine.
 */

//Number of stepper motors, must be set accordingly to the next lines
#define NB_STEPPERS 9

//Steppers settings : for each stepper of the machine, put one line like behind, and provide all parameters//TODO DOC

#ifdef STEPPER

//#define STEPPER(i, sig, r, ps, pd, dp,  pp, ve, pmi, vi, pma, va)

//TODO DOC

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

#endif


#ifdef STEPPER_DATA

//STEPPER_DATA(i, j, si, st, sp, a)

//TODO REMOVE SIZE
//              id, letter, size,   steps,  speed,  acceler.,   jerk)
STEPPER_DATA(   0,  '0',    170,    80.16,  1000,    1000,      20.)
STEPPER_DATA(   1,  '1',    170.,   80.16,  1000.,   1000,      20.)
STEPPER_DATA(   2,  '3',    150.,   80.16,  1000.,   1000,      20.)
STEPPER_DATA(   3,  '3',    150.,   80.16,  1000.,   1000,      20.)
STEPPER_DATA(   4,  '3',    150.,   80.16,  1000.,   1000.,      20.)
STEPPER_DATA(   5,  '3',    150.,   80.16,  1000.,   1000.,      20.)
STEPPER_DATA(   6,  '3',    150.,   80.16,  1000.,   1000.,      20.)
STEPPER_DATA(   7,  '3',    150.,   80.16,  1000.,   1000.,      20.)
STEPPER_DATA(   8,  '3',    150.,   80.16,  1000.,   1000.,      20.)


#endif

//-----------------------------------------------------CARTESIAN_GROUPS-------------------------------------------------

//TODO DOC
#define NB_CARTESIAN_GROUPS 8

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

//-------------------------------------------------Kinematics Core Version---------------------------------------------

/*
 * The version of the Stepper Control Kinematics TaskScheduler you want to use. You have 3 versions available :
 *  - 0 : KERNEL0, a basic kernel for less-than-32 bit processor, for cartesian-by-group machines. Only lines available.
 *  - 1 : KERNEL1, a faster kernel for 32 bits processors with FPU, for cartesian-by-group machines. Only lines available.
 *  - 2 : KERNEL2, a more advanced kernel for 32 bits processors with FPU, for non-cartesian machines. Any king of trajectory available.
 */

#define KERNEL 2

#define movement_data_t k2_movement_data

#define sub_movement_data_t k2_sub_movement_data

#define Kinematics KinematicsCore2


//------------------------------------------------- Distance Bounds---------------------------------------------

//The minimum number of steps between two consecutive positions;
#define MINIMUM_DISTANCE_LIMIT 8

//The ideal number of steps between two consecutive positions;
#define DISTANCE_TARGET 11

//The maximum number of steps between two consecutive positions;
#define MAXIMUM_DISTANCE_LIMIT 14

//-------------------------------------------------------Data types-----------------------------------------------------

/* The signature type. The type size depends on the number of steppers you want to control. Set :
 *  - uint8_t for at most 8 steppers;
 *  - uint16_t for at most 16 steppers;
 *  - uint32_t for at most 32 steppers;
 *  - uint64_t for at most 64 steppers; (not tested, tell me if it works !)
 */

#define sig_t uint32_t

//The delay type
#define delay_t uint32_t


//-------------------------------------------------------Command System-------------------------------------------------

/*
 * The Control coordinate system is an abstraction layer, over the high level coordinate system, made to ease
 *  the control of the machine. As high level axis can be numerous, this system allows you divide your axis in groups,
 *  for example, and to control them in groups.
 *
 *  To configure you layer of abstraction, you may require some persistent parameters, that will be saved in the
 *      EEPROM.
 *
 *  To create a persistent variable, you may use the following command :
 *
 *  COORD_INTERFACE_VARIABLE(name, default)
 *
 *  It will create a float, named "name", with the default value "default".
 *
 */

#ifdef COORD_INTERFACE_VARIABLE

COORD_INTERFACE_VARIABLE(x0_offset, 10)
COORD_INTERFACE_VARIABLE(x1_offset, 230)
COORD_INTERFACE_VARIABLE(y0_offset, 10)
COORD_INTERFACE_VARIABLE(y1_offset, 230)
COORD_INTERFACE_VARIABLE(z_offset, 10)
COORD_INTERFACE_VARIABLE(x_max_sum, 400)
COORD_INTERFACE_VARIABLE(y_max_sum, 400)

#endif


//----------------------------------------------------------Log---------------------------------------------------------

//Log settings. Uncomment those lines to output (resp) position and overflow marker in Monitor
#define position_log

//distance btw two position logs
#ifdef position_log
#define LOG_SPACE_MAX 5
#endif


//---------------------------------------------------------Other--------------------------------------------------------

//The maximum number of pre-processable movements.
#define MOVEMENT_DATA_QUEUE_SIZE 20

//The maximum number of pre-processable sub_movements.
#define SUB_MOVEMENT_DATA_QUEUE_SIZE 40

//The maximum number of curve points in a GCode Command (used in Bezier curves for ex)
#define MAX_CURVE_POINTS 5
