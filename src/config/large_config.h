//TODO METTRE DES UNDEFS

///######################################################PARTS##########################################################

/*
 * This section defines which parts of LowLevel will be enabled in your project.
 * Disabling features you don't use will reduce the project's size.
 * Parts may require hardware or software features that you can enable in "hardware_language_abstraction.h"
 * (Serial Port, or Strings for example).
 */

//Uncomment this line to enable regulation loops.
#define ENABLE_ASSERV

//Uncomment this line to enable regulation loops.
#define ENABLE_STEPPER_CONTROL



//####################################################Interface#########################################################

//Uncomment this line to enable the command interface.
#define ENABLE_TREE_INTERFACE

//Uncomment this line to enable the gcode interface.
//#define ENABLE_GCODE_INTERFACE

//You must define the main command Interface. You must uncomment only one of lines below
#define MAIN_CI_TREE
//#define MAIN_CI_GCODE

//The baudrate of the serial link   //TODO PHYSICAL_LINK_CONFIG
#define BAUDRATE 115200

//The maximum size of a data in one message
#define PACKET_SIZE 200
//######################################################Core############################################################

/*
 * This section defines how many tasks can be memorised by the core.
 */

/* The permanent task list
 * You can here define the main loop of the Core.
 * ADD_PERMANENT TASK is used in Core to write automatically the loop function.
 * Add this macro with a void ()(void), and it will be executed.
 */

#ifdef ADD_PERMANENT_TASK

#ifdef ENABLE_TREE_INTERFACE
ADD_PERMANENT_TASK(TI::read_serial)
#endif

#ifdef ENABLE_GCODE_INTERFACE
ADD_PERMANENT_TASK(GI::read_serial)
#endif

#endif

//The number of punctual tasks
#define MAX_TASKS 10

//######################################################SENSORS#########################################################


/* Thermistors :
 * For each thermistor you may want to include, add a line like the following :
 *
 * THERMISTOR(indice, type_table, type_size)
 *
 * where indices are strictly consecutive, starting by 0, and type_table and type_size are macros names,
 *      for your thermistor. Those are defined in thermistor_data.h.
 */

#ifdef THERMISTOR

THERMISTOR(0, B57540G0104F000_TABLE, B57540G0104F000_SIZE)

#endif

//######################################################ASSERV##########################################################

/* This section sets up the asserv code structure.
 * The structure is composed of external parameters, representing the information you possess about your environment,
 * and PIDs, computing data computed from external parameters. PID Outputs can be computed, to call actions.
 */


/* External Parameters : an external parameter, computed with sensors
 * For each parameter, put one line like behind and provide the parameter name
 * EXTERNAL_PARAMETER(i, name, min, max, unit)
 */

#ifdef EXTERNAL_PARAMETER

EXTERNAL_PARAMETER(0, temp_hotend, 0, 300, degree)
EXTERNAL_PARAMETER(1, temp_hotbed, 0, 300, degree)

#endif

/* PID : a PID.
 * For each PID, put one line like behind and provide the three required parameter
 * PID(indice, name, kp, ki, kd)
 */

//Set the number of PIDs
#define NB_PIDS 2

#ifdef PID

PID(0, hotend, 1, 0, 0)
PID(1, hotbed, 1, 1, 0)
#endif


/* Loop Functions : a interrupt called function
 * For each loop function, put one line like behind and provide the two required parameter
 * LOOP_FUNCTION(indice, name, period_ms)
 */

#define NB_LOOPS 2

#ifdef LOOP_FUNCTION

LOOP_FUNCTION(0, temps, 10);
LOOP_FUNCTION(1, ts,    105);

#endif


//######################################################ACTIONS#########################################################

//TODO DOC ABOUT I MEANING (need to have an indice to process data from logger)


/* Binary Actions (on - off)
 * For each actuator you want to control in on-off mode, put one line like behind and provide the four required parameter
 * BINARY(i, name, powerPin, enableValue)
 */

#ifdef BINARY

BINARY(0, binary, 9, 1)
BINARY(1, binary2, 9, 1)

#endif


/* Continuous Actions : a power command, output in [|0, 255|]
 * For each actuator you want to control in linear, put one line like behind and provide the three required parameter
 * CONTINUOUS(i, name, powerPin, maxValue)
 */
#define NB_CONTINUOUS 3

#ifdef CONTINUOUS

CONTINUOUS(0, continuous1, 8, 600)
CONTINUOUS(1, continuous2, 8, 600)
CONTINUOUS(2, continuous3, 8, 600)

#endif

/* Servomotor Actions
 * For each servo you want to control, put one line like behind and provide the three required parameter
 * SERVO(i, name, dataPin, minValue, maxValue)
 */

#define NB_SERVOS 3
#ifdef SERVO

SERVO(0, servo, 4, 0, 1)
SERVO(1, servo2, 4, 0, 1)
SERVO(2, servo3, 4, 0, 1)

#endif


//###############################################CURVES SETTINGS########################################################
//The maximum number of curve points in a GCode Command (used in Bezier curves for ex)
#define MAX_CURVE_POINTS 5

//#################################################LOG_SETTINGS#########################################################

//Log settings. Uncomment those lines to output (resp) position and overflow marker in Monitor
#define position_log


//distance btw two position logs
#ifdef position_log
#define LOG_SPACE_MAX 5
#endif

//###########################################STEPPER_CONTROLLER_SETTINGS###############################################

#define MOTION_DATA_QUEUE_SIZE 20

#define DISTANCE_TARGET 15//TODO MAKE A KERNEL SECTION IN THIS FILE


#define sig_t uint32_t
#define delay_t uint32_t

//---------------------------------------------------STEPPER_ABSTRACTION------------------------------------------------

//Number of axis in the high level coordinates system
#define NB_AXIS 3


//Axis settings : for each axis of the machine, put one line like behind, and provide all parameters//TODO DOC
#ifdef AXIS
//AXIS(i, j, si, st, sp, a)

//      id, letter, size,   steps,  speed,  acceler.,   jerk)
AXIS(   0,  '0',    170,    80.16,  500.,   1350.,      20.)

#endif

//------------------------------------------------------STEPPER_MOTORS--------------------------------------------------


//Number of stepper motors, must be set accordingly to the next lines
#define NB_STEPPERS 4

//Steppers settings : for each stepper of the machine, put one line like behind, and provide all parameters//TODO DOC

#ifdef STEPPER
//#define STEPPER(i, j, r, sig, ps, pd, dp,  pp, ve, pmi, vi, pma, va)

//TODO DOC
//      id, sig,    relat.  pStep,  pDir,   dir+    pPower, vEnab,  pMin,   VMin,   pMax,   vMax)
STEPPER(0,  1,      0,      54,     55,     LOW,    38,     LOW,    3,      HIGH,   2,      HIGH)
STEPPER(1,  2,      0,      60,     61,     LOW,    56,     LOW,    14,     HIGH,   1,      HIGH)
STEPPER(2,  4,      0,      46,     48,     LOW,    62,     LOW,    18,     HIGH,   19,     HIGH)
STEPPER(3,  8,      1,      26,     28,     LOW,    24,     LOW,    18,     HIGH,   19,     HIGH)

/*
STEPPER(4,  16,     1,      26,     28,     LOW,    24,     LOW,    18,     HIGH,   19,     HIGH)
STEPPER(5,  32,     1,      26,     28,     LOW,    24,     LOW,    18,     HIGH,   19,     HIGH)
STEPPER(6,  64,     1,      26,     28,     LOW,    24,     LOW,    18,     HIGH,   19,     HIGH)
STEPPER(7,  128,    1,      26,     28,     LOW,    24,     LOW,    18,     HIGH,   19,     HIGH)
STEPPER(8,  256,    1,      26,     28,     LOW,    24,     LOW,    18,     HIGH,   19,     HIGH)
STEPPER(9,  512,    1,      26,     28,     LOW,    24,     LOW,    18,     HIGH,   19,     HIGH)
STEPPER(10,  1024,    1,      26,     28,     LOW,    24,     LOW,    18,     HIGH,   19,     HIGH)
STEPPER(11,  2048,    1,      26,     28,     LOW,    24,     LOW,    18,     HIGH,   19,     HIGH)
STEPPER(12,  4096,    1,      26,     28,     LOW,    24,     LOW,    18,     HIGH,   19,     HIGH)
STEPPER(13,  8192,    1,      26,     28,     LOW,    24,     LOW,    18,     HIGH,   19,     HIGH)
STEPPER(14,  16384,    1,      26,     28,     LOW,    24,     LOW,    18,     HIGH,   19,     HIGH)
STEPPER(15,  32768,    1,      26,     28,     LOW,    24,     LOW,    18,     HIGH,   19,     HIGH)
STEPPER(16,  65536,    1,      26,     28,     LOW,    24,     LOW,    18,     HIGH,   19,     HIGH)
*/

#endif

#ifdef STEPPER_DATA

//STEPPER_DATA(i, j, si, st, sp, a)

//TODO REMOVE SIZE
//              id, letter, size,   steps,  speed,  acceler.,   jerk)
STEPPER_DATA(   0,  '0',    170,    80.16,  500.,   1350.,      20.)
STEPPER_DATA(   1,  '1',    170.,   80.16,  500.,   1200.,      20.)
STEPPER_DATA(   2,  '3',    150.,   80.16,  500.,   1200.,      20.)
STEPPER_DATA(   3,  '3',    150.,   80.16,  500.,   1200.,      20.)

/*
STEPPER_DATA(   4,  '3',    150.,   80.16,  500.,   1200.,      20.)
STEPPER_DATA(   5,  '3',    150.,   80.16,  500.,   1200.,      20.)
STEPPER_DATA(   6,  '3',    150.,   80.16,  500.,   1200.,      20.)
STEPPER_DATA(   7,  '3',    150.,   80.16,  500.,   1200.,      20.)
STEPPER_DATA(   8,  '3',    150.,   80.16,  500.,   1200.,      20.)
STEPPER_DATA(   9,  '3',    150.,   80.16,  500.,   1200.,      20.)
STEPPER_DATA(   10,  '3',    150.,   80.16,  500.,  1200.,      20.)
STEPPER_DATA(   11,  '3',    150.,   80.16,  500.,  1200.,      20.)
STEPPER_DATA(   12,  '3',    150.,   80.16,  500.,  1200.,      20.)
STEPPER_DATA(   13,  '3',    150.,   80.16,  500.,  1200.,      20.)
STEPPER_DATA(   14,  '3',    150.,   80.16,  500.,  1200.,      20.)
STEPPER_DATA(   15,  '3',    150.,   80.16,  500.,  1200.,      20.)
STEPPER_DATA(   16,  '3',    150.,   80.16,  500.,  1200.,      20.)
*/

#endif

//-----------------------------------------------------CARTESIAN_GROUPS-------------------------------------------------

//TODO DOC
#define NB_CARTESIAN_GROUPS 2

#ifdef CARTESIAN_GROUP

//              id,     a0      a1      a2      maxSpeed
CARTESIAN_GROUP(0,      0,      1,      2,      500     )
CARTESIAN_GROUP(1,      3,      -1,     -1,     500     )
/*
CARTESIAN_GROUP(2,      6,      7,      8,     500     )
CARTESIAN_GROUP(3,      9,      10,     11,     500     )
CARTESIAN_GROUP(4,      12,     13,     14,     500     )
CARTESIAN_GROUP(5,      15,     16,     -1,     500     )
*/

#endif

//###############################################EEPROM SETTINGS########################################################

/* EEPROM custom data definition : for each variable you need to save in EEPROM, write one of the following lines
 *
 * EEPROM_BOOL(name, default_value)
 * EEPROM_CHAR(name, default_value)
 * EEPROM_INT(name, default_value)
 * EEPROM_LONG(name, default_value)
 * EEPROM_FLOAT(name, default_value)
 *
 * Each one of these functions will create a variable of the specified type with the name provided
 *  in the EPROMStorage class, that will be saved in EEPROM, and take the default value provided
 */


#if defined(EEPROM_BOOL) && defined(EEPROM_CHAR) && defined(EEPROM_INT) && defined(EEPROM_LONG) && defined(EEPROM_FLOAT)

EEPROM_FLOAT(maximum_speed, 500);

#endif