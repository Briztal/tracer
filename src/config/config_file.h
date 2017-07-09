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



//################################################YOUR PROJECT'S PART####################################################