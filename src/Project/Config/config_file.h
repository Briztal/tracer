//A string naming your project
#define PROJECT_NAME "TRACER"

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


/*
 * Interface - data-link settings
 *                                  WARNING !!!!!
 *
 * Two interface mustn't be based on the same data link. If they to, data will be dispatched
 *  between the two interfaces, breaking its integrity.
 */

//Uncomment this line to enable the terminal interface.
#define ENABLE_TERMINAL_INTERFACE
#define terminal_interface_link_t usb_serial

//Uncomment this line to enable the program interface.
//#define ENABLE_PROGRAM_INTERFACE
//#define program_interface_link_t usb_serial

//Uncomment this line to enable the gcode interface.
//#define ENABLE_GCODE_INTERFACE
//#define gcode_interface_link_t usb_serial

//#define ENABLE_LCD_INTERFACE
//#define lcd_interface_link_t usb_serial


//You must define the main command Interface. You must uncomment only one of lines below
//#define MAIN_CI_PROGRAM
//#define MAIN_CI_GCODE
#define MAIN_CI_TERMINAL

//The baudrate of the serial link   //TODO PHYSICAL_LINK_CONFIG
#define BAUDRATE 115200

//#define MAX_WORD_SIZE 40

//######################################################TaskScheduler############################################################

//The following line defines the task pool size
#define TASK_POOL_SIZE 20

#define NB_TASK_SEQUENCES 3

/*
 * The following block defines the number of task sequences, their ID, their name and their size
 */

//TASK_SEQUENCE(id, size)
#ifdef TASK_SEQUENCE

TASK_SEQUENCE(0, 20)
TASK_SEQUENCE(1, 10)
TASK_SEQUENCE(2, 10)

#endif

#define NON_SEQUENTIAL 255

//Aliases for sequences ids
#define MOVEMENT_SEQUENCE 0
#define EEPROM_SEQUENCE 1
#define VARIOUS_SEQUENCE 2


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

THERMISTOR(0, hotend_0, A26,    SEMITEC_100_47)
THERMISTOR(1, hotend_1, 22,     SEMITEC_100_47)
THERMISTOR(2, hotend_2, 21,     SEMITEC_100_47)
THERMISTOR(3, hotend_3, 20,     SEMITEC_100_47)
THERMISTOR(4, hotbed,   19,     BED100K)

#endif

//######################################################ASSERV##########################################################

#ifdef ENABLE_ASSERV

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
#define NB_PIDS 5

#ifdef PID

PID(0, hotend_0, 20, 0, 0)
PID(1, hotend_1, 20, 0, 0)
PID(2, hotend_2, 20, 0, 0)
PID(3, hotend_3, 20, 0, 0)
PID(4, hotbed, 20, 0, 0)

#endif


/* Loop Functions : a interrupt called function
 * For each loop function, put one line like behind and provide the two required parameter
 * LOOP_FUNCTION(indice, name, period_ms)
 */

#define NB_LOOPS 2

#ifdef LOOP_FUNCTION

LOOP_FUNCTION(0, temperature, 10);

#endif

#endif

//######################################################ACTIONS#########################################################

//TODO DOC ABOUT I MEANING (need to have an indice to process data from logger)


/* Binary Actions (on - off)
 * For each actuator you want to control in on-off mode, put one line like behind and provide the four required parameter
 * BINARY(i, name, powerPin, enableValue)
 */

#ifdef BINARY


#endif


/* Continuous Actions : a power command, output in [|0, 255|]
 * For each actuator you want to control in linear, put one line like behind and provide the three required parameter
 * CONTINUOUS(i, name, powerPin, maxValue)
 */

#define NB_CONTINUOUS 6

#ifdef CONTINUOUS

CONTINUOUS(0,   hotend_0,   2,      100)
CONTINUOUS(1,   hotend_1,   3,      100)
CONTINUOUS(2,   hotend_2,   4,      100)
CONTINUOUS(3,   hotend_3,   5,      100)
CONTINUOUS(4,   hotbed,     6,      100)
CONTINUOUS(5,   cooling,    23,     100)

#endif

/* Servomotor Actions
 * For each servo you want to control, put one line like behind and provide the three required parameter
 * SERVO(i, name, dataPin, minValue, maxValue)
 */

#define NB_SERVOS 0
#ifdef SERVO

//SERVO(0, servo, 4, 0, 1)

#endif


//###########################################STEPPER_CONTROLLER_SETTINGS###############################################

#ifdef ENABLE_STEPPER_CONTROL
//-------------------------------------------------Kinematics TaskScheduler  Version---------------------------------------------

/*
 * The version of the Stepper Control Kinematics TaskScheduler you want to use. You have 3 versions available :
 *  - 0 : KERNEL0, a basic kernel for less-than-32 bit processor, for cartesian-by-group machines. Only lines available.
 *  - 1 : KERNEL1, a faster kernel for 32 bits processors with FPU, for cartesian-by-group machines. Only lines available.
 *  - 2 : KERNEL2, a more advanced kernel for 32 bits processors with FPU, for non-cartesian machines. Any king of trajectory available.
 */

#define KERNEL 1

#define movement_data_t k1_movement_data

#define sub_movement_data_t k1_sub_movement_data

#define Kinematics KinematicsCore1

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


//-------------------------------------------------------Data types-----------------------------------------------------

/* The signature type. The type size depends on the number of steppers you want to control. Set :
 *  - uint8_t for at most 8 steppers;
 *  - uint16_t for at most 16 steppers;
 *  - uint32_t for at most 32 steppers;
 *  - uint64_t for at most 64 steppers; (not tested, tell me if it works !)
 *
 */
#define sig_t uint32_t

//The delay type
#define delay_t uint32_t

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

//The maximum number of curve points in a GCode Command (used in Bezier curves for ex)
#define MAX_CURVE_POINTS 5

#endif