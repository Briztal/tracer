//TODO METTRE DES UNDEFS

///######################################################PARTS###########################################################

/*
 * This section defines which parts of LowLevel will be enabled in your project.
 * Disabling features you don't use will reduce the project's size.
 * Parts may require hardware or software features that you can enable in "hardware_language_abstraction.h"
 * (Serial Port, or Strings for example).
 */

//Uncomment this line to enable the logger interface.
#define ENABLE_LOGGER

//Uncomment this line to enable regulation loops.
//#define ENABLE_ASSERV

//Uncomment this line to enable regulation loops.
#define ENABLE_STEPPER_CONTROL

//######################################################Core###########################################################

/*
 * This section defines how many tasks can be memorised by the core.
 */

/* The permanent task list
 * You can here define the main loop of the Core.
 * ADD_PERMANENT TASK is used in Core to write automatically the loop function.
 * Add this macro with a void ()(void), and it will be executed.
 */

#ifdef ADD_PERMANENT_TASK

ADD_PERMANENT_TASK(CommandInterface::read_serial) //TODO MODIFIER CE NOM DE MERDE

#endif

//The number of punctual tasks
#define MAX_TASKS 10



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

EXTERNAL_PARAMETER(0, pname0, 0, 1, cm)
EXTERNAL_PARAMETER(1, pname1, 0, 1, cm)
EXTERNAL_PARAMETER(2, pname2, 0, 1, cm)
EXTERNAL_PARAMETER(3, pname3, 0, 1, cm)

#endif

/* PID : a PID.
 * For each PID, put one line like behind and provide the three required parameter
 * PID(indice, name, kp, ki, kd)
 */

//Set the number of PIDs
#define NB_PIDS 5

#ifdef PID

PID(0, name1, 1, 0, 0)
PID(1, name2, 1, 0, 0)
PID(2, name3, 1, 0, 0)
PID(3, name4, 1, 0, 0)
PID(4, name5, 1, 0, 0)

#endif


/* Loop Functions : a interrupt called function
 * For each loop function, put one line like behind and provide the two required parameter
 * LOOP_FUNCTION(indice, name, period_ms)
 */

#ifdef LOOP_FUNCTION

LOOP_FUNCTION(0, loop, 100);
LOOP_FUNCTION(1, loop1, 100);

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
#ifdef CONTINUOUS
#define NB_CONTINUOUS 1

CONTINUOUS(0, continuous, 8, 80)
CONTINUOUS(1, continuous2, 8, 80)

#endif

/* Servomotor Actions
 * For each servo you want to control, put one line like behind and provide the three required parameter
 * SERVO(i, name, dataPin, minValue, maxValue)
 */

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
#define position_log;


//distance btw two position logs
#ifdef position_log
#define LOG_SPACE_MAX 5
#endif

//###########################################STEPPER_CONTROLLER_SETTINGS###############################################

#define MOTION_DATA_QUEUE_SIZE 20

//Number of stepper motors, must be set accordingly to the next lines
#define NB_STEPPERS 4

//Axis settings : for each axis of the machine, put one line like behind, and provide all parameters//TODO DOC
#ifdef STEPPER //STEPPER(id, char, signature, relative, pinStep, pinDir, dir+, pinPower, enableValue, pinEndMin, OnEndMinValue, pinEndMax, OnEndMaxValue)
//STEPPER(i, j, sig, r, ps, pd, dp,  pp, ve, pmi, vi, pma, va)
//#define STEPPER(i, j, r, sig, ps, pd, dp,  pp, ve, pmi, vi, pma, va)

//TODO CLARIFIER LES VALEURS DES ENDSTOPS, DE LA SIGNATURE
//      id, sig,    relat.  pStep,  pDir,   dir+    pPower, vEnab,  pMin,   VMin,   pMax,   vMax)
STEPPER(0,  1,      0,      54,     55,     LOW,    38,     LOW,    3,      HIGH,   2,      HIGH);
STEPPER(1,  2,      0,      60,     61,     LOW,    56,     LOW,    14,     HIGH,   1,      HIGH);
STEPPER(2,  4,      0,      46,     48,     LOW,    62,     LOW,    18,     HIGH,   19,     HIGH);
STEPPER(3,  8,      0,      26,     28,     LOW,    24,     LOW,    18,     HIGH,   19,     HIGH);

#endif

#ifdef STEPPER_DATA

//STEPPER_DATA(i, j, si, st, sp, a)

//              id, letter, size,   steps,  speed,  acceleration)
STEPPER_DATA(   0,  '0',    170,    80.16,  500,    1350);
STEPPER_DATA(   1,  '1',    170.,   80.16,  500,    1200);
STEPPER_DATA(   2,  '2',    150.,   2540,   3,      20);
STEPPER_DATA(   3,  '3',    150.,   80.16,  500,    1200);

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