//######################################################PARTS###########################################################

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

EXTERNAL_PARAMETER(0, pname, 0, 1, cm)

#endif

/* PID : a PID.
 * For each PID, put one line like behind and provide the three required parameter
 * PID(indice, name, kp, ki, kd)
 */

#ifdef PID

PID(0, name, 1, 0, 0)

#endif


/* Loop Functions : a interrupt called function
 * For each loop function, put one line like behind and provide the two required parameter
 * LOOP_FUNCTION(indice, name, period_ms)
 */

#ifdef LOOP_FUNCTION

LOOP_FUNCTION(0, loop, 100);

#endif


//######################################################ACTIONS#########################################################

//TODO DOC ABOUT I MEANING (need to have an indice to process data from logger)


/* Binary Actions (on - off)
 * For each actuator you want to control in on-off mode, put one line like behind and provide the four requires parameter
 * BIANRY(i, name, powerPin, enableValue)
 */

#ifdef BINARY

BINARY(0, binary, 9, 1)

#endif


/* Continuous Actions : a power command, output in [|0, 255|]
 * For each actuator you want to control in linear, put one line like behind and provide the three requires parameter
 * CONTINUOUS(i, name, powerPin, maxValue)
 */
#ifdef CONTINUOUS
#define NB_CONTINUOUS 1

CONTINUOUS(0, continuous, 8, 80)

#endif

/* Servomotor Actions
 * For each servo you want to control, put one line like behind and provide the three requires parameter
 * SERVO(i, name, dataPin, minValue, maxValue)
 */

#ifdef SERVO

SERVO(0, servo, 4, 0, 1)

#endif



//##############################################SOFTWARE_SETTINGS#######################################################
//The maximum number of curve points in a GCode Command (used in Bezier curves for ex)
#define MAX_CURVE_POINTS 7
#define DATA_SIZE 100

//#################################################LOG_SETTINGS#########################################################

//Log settings. Uncomment those lines to output (resp) position and overflow marker in Monitor
//#define position_log;
#define ovf_log

//distance btw two position logs
#ifdef position_log
#define LOG_SPACE_MAX 1
#endif


//Gcode Storage size, and max number of processed commands
#define STORAGE_SIZE 5
#define COMMAND_NB 5


//TODO PILOTAGE DE BRAS : DENAVIT HARTENBERG (1955) kinematic notation

//###############################################MACHINE_SETTINGS#######################################################

//Number of axis, must be set accordingly to the next lines
#define NB_AXIS 3

//Axis settings : for each axis of the machine, put one line like behind, and provide all parameters//TODO DOC
#ifdef AXIS_TREATMENT //AXIS_TREATMENT(id, letter, size, steps, speed, acceleration, dir+, pinStep, onValue, pinDir, pinPower, pinEndMin, pinEndMax)

//TODO CLARIFIER LES VALEURS DES ENDSTOPS : VALEUR NON APPUYEE
AXIS_TREATMENT(0, '0', 180, 37.7, 100, 100, HIGH, 54, 55, 38, 3, HIGH, 2, HIGH);

AXIS_TREATMENT(1, '1', 180, 37.7, 100, 100, HIGH, 60, 61, 56, 14, HIGH, 1, HIGH);

AXIS_TREATMENT(2, '2', 360, 400, 10, 100, LOW, 46, 48, 62, 18, HIGH, 19, HIGH);


#endif

//GENERAL VALUES
//You can put here any extra value you may desire to access across the code

#define laserMaxPower 20.
#define maxSpeed 300;
