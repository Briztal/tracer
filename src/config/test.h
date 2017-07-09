//TODO CLEAN THIS MESS
//##############################################SOFTWARE_SETTINGS#######################################################
//The maximum number of curve points in a GCode Command (used in Bezier curves for ex)
#define MAX_CURVE_POINTS 7
#define DATA_SIZE 100

//#################################################LOG_SETTINGS#########################################################

//Log settings. Uncomment those lines to output (resp) position and overflow marker in Monitor
#define position_log;
#define ovf_log

//distance btw two position logs
#ifdef position_log
#define LOG_SPACE_MAX 5
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
AXIS_TREATMENT(0, '0', 300, 80.16, 600, 2000, HIGH, 54, 55, 38, 3, HIGH, 2, HIGH);

AXIS_TREATMENT(1, '1', 300., 80.16, 600, 2000, HIGH, 46, 48, 62, 18, HIGH, 19, HIGH);

AXIS_TREATMENT(2, '2', 300., 80.16, 600, 2000, HIGH, 46, 48, 62, 18, HIGH, 19, HIGH);

#endif

//GENERAL VALUES
//You can put here any extra value you may desire to access across the code

#define laserMaxPower 20.
#define maxSpeed 600;

//EXTRA PINOUT
//You can put here any extra pin you may want to access across the code

#define ventilationPin 0
#define laserCoolingPin 0
#define laserPin 0

//######################################################ACTIONS#########################################################
//TODO DOC ABOUT I MEANING
//Servomotor Actions
//For each servo you want to control, put one line like behind and provide the three requires parameter
//SERVO(i, dataPin, minValue, maxValue)
#ifdef SERVO

SERVO(0, 4, 0, 1)

#endif

//Binary Actions (on - off)
//For each actuator you want to control in on-off mode, put one line like behind and provide the four requires parameter
//BIANRY(i, powerPin, enableValue)
#ifdef BINARY

BINARY(0, 9, HIGH)

#endif

#define NB_CONTINUOUS 1

//Continuous Actions : a power command, output in [|0, 255|]
//For each actuator you want to control in linear, put one line like behind and provide the three requires parameter
//CONTINUOUS(i, powerPin, maxValue)
#ifdef CONTINUOUS

CONTINUOUS(0, 8, 80)

#endif
