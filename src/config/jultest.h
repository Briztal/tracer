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
#define ENABLE_ASSERV

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

EXTERNAL_PARAMETER(0, width1, 0, 1, cm)

#endif

/* PID : a PID.
 * For each PID, put one line like behind and provide the three required parameter
 * PID(indice, name, kp, ki, kd)
 */

//Set the number of PIDs
#define NB_PIDS 1

#ifdef PID

PID(0, PD, 1, 0, 0)

#endif


/* Loop Functions : a interrupt called function
 * For each loop function, put one line like behind and provide the two required parameter
 * LOOP_FUNCTION(indice, name, period_ms)
 */

#ifdef LOOP_FUNCTION

LOOP_FUNCTION(0, control1, 100);

#endif


//######################################################ACTIONS#########################################################

//TODO DOC ABOUT I MEANING (need to have an indice to process data from logger)


/* Binary Actions (on - off)
 * For each actuator you want to control in on-off mode, put one line like behind and provide the four required parameter
 * BINARY(i, name, powerPin, enableValue)
 */

#ifdef BINARY

BINARY(0, BiBiBiBinaire, 9, 1)

#endif


/* Continuous Actions : a power command, output in [|0, 255|]
 * For each actuator you want to control in linear, put one line like behind and provide the three required parameter
 * CONTINUOUS(i, name, powerPin, maxValue)
 */
#ifdef CONTINUOUS
#define NB_CONTINUOUS 1

CONTINUOUS(0, suus, 8, 1)

#endif

/* Servomotor Actions
 * For each servo you want to control, put one line like behind and provide the three required parameter
 * SERVO(i, name, dataPin, minValue, maxValue)
 */

#ifdef SERVO

SERVO(0, SANS_EDULCORANT!!, 4, 600, 1600)

#endif


//###############################################CURVES SETTINGS########################################################
//The maximum number of curve points in a GCode Command (used in Bezier curves for ex)
#define MAX_CURVE_POINTS 5

//#################################################LOG_SETTINGS#########################################################

//Log settings. Uncomment those lines to output (resp) position and overflow marker in Monitor
//#define position_log;
#define ovf_log

//distance btw two position logs
#ifdef position_log
#define LOG_SPACE_MAX 5
#endif

//###########################################STEPPER_CONTROLLER_SETTINGS###############################################

//Number of stepper motors, must be set accordingly to the next lines
#define NB_STEPPERS 5

//Axis settings : for each axis of the machine, put one line like behind, and provide all parameters//TODO DOC
#ifdef STEPPER //STEPPER(id, letter, size, steps, speed, acceleration, dir+, pinStep, onValue, pinDir, pinPower, pinEndMin, pinEndMax)

//TODO CLARIFIER LES VALEURS DES ENDSTOPS : VALEUR NON APPUYEE
STEPPER(0, '0', 170, 80.16, 500, 1500, LOW, 54, 55, 38, 3, HIGH, 2, HIGH);

STEPPER(1, '1', 170., 80.16, 500, 1500, LOW, 60, 61, 56, 14, HIGH, 1, HIGH);

STEPPER(2, 'Z', 150., 2540, 4, 1000, LOW, 46, 48, 62, 18, HIGH, 19, HIGH);

STEPPER(3, '3', 150., 2540, 4, 1000, LOW, 46, 48, 62, 18, HIGH, 19, HIGH);

STEPPER(4, 'X', 150., 2540, 4, 1000, LOW, 46, 48, 62, 18, HIGH, 19, HIGH);


#endif

#define maxSpeed 500

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