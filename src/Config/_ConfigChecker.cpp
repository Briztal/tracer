/*
  _ConfigChecker.cpp Part of TRACER

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


#include "_ConfigChecker.h"

#include <Config/actions_config.h>

#include <DataStructures/string/tstring.h>


bool _ConfigChecker::check_config(tstring &message) {

    //Check actions config;
    if (!check_actions(message)) return false;

    //Check transmission layers and control configuration;
    if (!check_transmission(message)) return false;

    //Check transmission layers and control configuration;
    if (!check_controllers(message)) return false;

    //Check transmission layers and control configuration;
    if (!check_control_loops(message)) return false;

    //Check transmission layers and control configuration;
    if (!check_sensors(message)) return false;

    //Check transmission layers and control configuration;
    if (!check_stepper_control(message)) return false;

    return true;

}

/*
 * check_actions : this function verifies the action configuration;
 */

bool _ConfigChecker::check_actions(tstring &message) {

    //First, we will count the number of continuous actions.

    //Reset the counter;
    uint8_t counter =

            //A macro to increment the counter for each action;
#define CONTINUOUS(...) 1 +

            //Count every continuous action;
#include <Config/actions_config.h>

            //Undef the macro for safety
            #undef CONTINUOUS

            +0;

    if (counter != NB_CONTINUOUS) {

        //Return a message and fail;
        message = tstring() + "There should be " + (uint8_t) NB_SERVOS + " PWMGPIO declarations, but only " + counter +
                  " are provided. Check stepper_config.h.";
        return false;

    }

    /*
     * Then, we will check if indices are consecutives;
     */
    counter = 0;

    //A macro to count controls;
#define CONTINUOUS(i, ...) \
    if (counter != i) {\
        message = tstring()+ "PWM with index "#i" should have the index "+counter+". Check actions_config.h.";\
        return false;\
    }\
    counter++;

    //Count every control;
#include "Config/actions_config.h"

    //Undef the macro for safety;
#undef CONTINUOUS


    //----------- Servos -----------

    if (NB_SERVOS * 2500 > SERVO_PERIOD_US) {

        //Return a message and fail;
        message = tstring() +
                  "The servo period cannot contain the number of servos you specified. Check actions_config.h. Please reduce the amount of steppers or increase the period.";
        return false;

    }

    //We will count the number of servos

    //Reset the counter;
    counter =

            //A macro to increment the counter for each action;
#define SERVO(...) 1 +

//Count every continuous action;
#include <Config/actions_config.h>

//Undef the macro for safety
#undef SERVO

0;

    //Check the number of servos;
    if (counter != NB_SERVOS) {

        //Return a message and fail;
        message = tstring() + "There should be " + (uint8_t) NB_SERVOS + " servo motor declarations, but only " + counter +
                  " are provided. Check stepper_config.h.";
        return false;

    }

    /*
     * Then, we will check if indices are consecutives;
     */
    counter = 0;

    //A macro to count controls;
#define SERVO(i, ...) \
    if (counter != i) {\
        message = tstring()+"Servo with index "#i" should have the index "+(uint8_t)counter+". Check actions_config.h.";\
        return false;\
    }\
    counter++;

    //Count every control;
#include "Config/actions_config.h"

    //Undef the macro for safety;
#undef SERVO

    //No error. Complete;
    return true;

}


/*
 * check_transmission : This function verifies the configuration of the control chains;
 */

bool _ConfigChecker::check_transmission(tstring &message) {

    /*
     * First, we will check the number of controls;
     */

    uint8_t counter =

            //A macro to count controls;
#define COMMUNICATION_PIPE(c, p, s, transmission) 1 +

            //Count every control;
#include "Config/control_config.h"

            //Undef the macro for safety;
            #undef COMMUNICATION_PIPE

            0;

    //Check the number of controls
    if (counter != NB_PIPES) {

        //Return a message and fail;
        message = tstring() + "There should be " + (uint8_t) NB_PIPES + " controllers declarations, but only " +
                  counter + " are provided. Check stepper_config.h.";
        return false;

    }


    /*
     * Next, we will check if one transmission link is used twice.
     */

    //Declare an array of functions;
    void (*functions[NB_PIPES])(void);

    //The counter will now count how many functions are in the array.
    counter = 0;

    void (*function)(void);

    //To check if a transmission layer is used multiple times, we will compare addresses of the begin method;
#define EXTERNAL_CONTROL(c, p, s, transmission)\
    /*Cache the init function;*/\
    function = transmission::begin;\
    /*For every function added;*/\
    for (uint8_t i = 0; i<counter;i++) {\
        /*If the function pointer matched one previously added;*/\
        if (function == functions[i]) {\
            /*Log and fail;*/\
            message = tstring() + "the transmission layer "#transmission" is used multiple times. Check control_config.h.";\
            return false;\
        }\
    }\
    /*Add the function to the array;*/\
    functions[counter++] = function;

    //Check every transmission layer used;
#include "Config/control_config.h"

    //Undef the amcro for safety;
#undef EXTERNAL_CONTROL


    //No error. Complete;
    return true;

}


bool _ConfigChecker::check_control_loops(tstring &message) {

    /*
     * First, we will check the number of control loops;
     */

    uint8_t counter =

            //A macro to count controls;
#define LOOP_FUNCTION(...) 1 +

            //Count every control;
#include "Config/control_loops_config.h"

            //Undef the macro for safety;
            #undef LOOP_FUNCTION

            0;

    //Check the number of controls
    if (counter != NB_LOOPS) {

        //Return a message and fail;
        message = tstring() + "There should be " + (uint8_t) NB_LOOPS + " control loops declarations, but only " +
                  counter + " are provided. Check stepper_config.h.";
        return false;

    }

    /*
     * Then, we will check if indices are consecutives;
     */
    counter = 0;

    //A macro to count controls;
#define LOOP_FUNCTION(i, ...) \
    if (counter != i) {\
        message = tstring() + "loop with index "#i" should have the index "+counter+". Check control_loops_config.h.";\
        return false;\
    }\
    counter++;

    //Count every control;
#include "Config/control_loops_config.h"

    //Undef the macro for safety;
#undef LOOP_FUNCTION


    /*
     * Now, we will check the number of PIDs;
     */
    counter =

            //A macro to count controls;
#define GENERATE_PID(...) 1 +

//Count every control;
#include "Config/control_loops_config.h"

//Undef the macro for safety;
#undef GENERATE_PID

0;

    //Check the number of controls
    if (counter != NB_PIDS) {

        //Return a message and fail;
        message = tstring() + "There should be " + (uint8_t) NB_PIDS + " pid declarations, but only " + counter +
                  " are provided. Check stepper_config.h.";
        return false;

    }

    /*
     * Then, we will check if indices are consecutives;
     */
    counter = 0;

    //A macro to count controls;
#define GENERATE_PID(i, ...) \
    if (counter != i) {\
        message = tstring() + "pid with index "#i" should have the index "+counter+". Check control_loops_config.h";\
        return false;\
    }\
    counter++;

    //Count every control;
#include "Config/control_loops_config.h"

    //Undef the macro for safety;
#undef GENERATE_PID

    //No error. Complete;
    return true;
}


bool _ConfigChecker::check_controllers(tstring &message) {

    /*
     * ByteTreeLanguage check : We will check that no ByteTreeLanguage command has its size greater than the maximum size;
     */

    //Cache for the name;
    string str_name;

    //Check a command size;
#define GCODE_COMMAND(name, ...)\
    str_name = #name;\
    if(str_name.length() > GCODE_MAX_DEPTH) { \
        message = tstring()+"GCode command "#name" is longer than "+ (uint8_t)GCODE_MAX_DEPTH+" chars. Check controller_gcode_config.h";\
        return false;\
    }

    //Check every command;
#include "Config/controller_gcode_config.h"

    //Undef the macro for safety;
#undef GCODE_COMMAND

    //No error. Complete;
    return true;

}

bool _ConfigChecker::check_sensors(tstring &message) {

    /*
     * Then, we will check if indices are consecutives;
     */

    uint8_t counter = 0;

    //A macro to count controls;
#define THERMISTOR(i, ...) \
    if (counter != i) {\
        message = tstring()+ "thermistor with index "#i" should have the index "+counter+". Check sensors_config.h.";\
        return false;\
    }\
    counter++;

    //Count every control;
#include "Config/sensors_config.h"

    //Undef the macro for safety;
#undef THERMISTOR

    //No error. Complete;
    return true;

}

bool _ConfigChecker::check_stepper_control(tstring &message) {

    //First, we will count the number of stepper motors.

    //Reset the counter;
    uint8_t counter =

            //A macro to increment the counter for each stepper;
#define STEPPER(...) 1 +

            //Count every stepper;
#include <Config/stepper_control_config.h>

            //Undef the macro for safety
            #undef STEPPER

            +0;

    if (counter != NB_STEPPERS) {

        //Return a message and fail;
        message = tstring() + "There should be " + (uint8_t) NB_STEPPERS + " stepper motor declarations, but only " +
                  counter + " are provided. Check stepper_config.h.";
        return false;

    }

    /*
     * Then, we will check if indices are consecutives;
     */
    counter = 0;

    //A macro to check an index;
#define STEPPER(i, ...) \
    if (counter != i) {\
        message = tstring()+"Stepper with index "#i" should have the index "+counter+". Check stepper_control_config.h.";\
        return false;\
    }\
    counter++;

    //Check every index;
#include "Config/stepper_control_config.h"

    //Undef the macro for safety;
#undef STEPPER

    //Next, we will count the number of stepper data declaration.

    //Reset the counter;
    counter =

            //A macro to increment the counter for each stepper;
#define STEPPER_DATA(...) 1 +

//Count every stepper;
#include <Config/stepper_control_config.h>

//Undef the macro for safety
#undef STEPPER_DATA

+0;

    if (counter != NB_STEPPERS) {

        //Return a message and fail;
        message =
                tstring("There should be ") + (uint8_t) NB_STEPPERS + " stepper data declarations, but only " + counter +
                " are provided. Check stepper_config.h.";
        return false;

    }

    /*
     * Then, we will check if indices are consecutives;
     */
    counter = 0;

    //A macro to check an index;
#define STEPPER_DATA(i, ...) \
    if (counter != i) {\
        message = tstring() + "Stepper data with index "#i" should have the index "+counter+". Check stepper_control_config.h.";\
        return false;\
    }\
    counter++;

    //Check every index;
#include "Config/stepper_control_config.h"

    //Undef the macro for safety;
#undef STEPPER_DATA


    //Finally, we will count the number of cartesian groups declaration.

    //Reset the counter;
    counter =

            //A macro to increment the counter for each stepper;
#define CARTESIAN_GROUP(...) 1 +

//Count every stepper;
#include <Config/stepper_control_config.h>

//Undef the macro for safety
#undef CARTESIAN_GROUP

+0;

    if (counter != NB_CARTESIAN_GROUPS) {

        //Return a message and fail;
        message = tstring() + "There should be " + (uint8_t) NB_CARTESIAN_GROUPS +
                  " cartesian group declarations, but only " + counter + " are provided. Check stepper_config.h.";
        return false;

    }

    /*
     * Then, we will check if indices are consecutives, if all positive_rotation motor index refer to an existing stepper,
     *  and if all maximum speeds are strictly positive_rotation;
     */
    counter = 0;

#define CHECK_STEPPER_INDEX(n, index) \
    if ((index) >= NB_STEPPERS)  {\
        message = tstring() + " the cartesian group "#n" refers to the stepper "#index" that doesn't exist. Check stepper_control_config.h.";\
        return false;\
    }\

    //A macro to check an index;
#define CARTESIAN_GROUP(i, a, b, c, speed) \
    if (counter != (i)) {\
        message = tstring() + " the cartesian group with index "#i" should have the index "+counter+". Check stepper_control_config.h.";\
        return false;\
    }\
    CHECK_STEPPER_INDEX(i, a)\
    CHECK_STEPPER_INDEX(i, b)\
    CHECK_STEPPER_INDEX(i, c)\
    if ((float)(speed) <= 0.) {\
        message = tstring() + " the maximum speed of the cartesian group with index "#i" is negative or null. Check stepper_control_config.h.";\
        return false;\
    }\
    counter++;

    //Check every index;
#include "Config/stepper_control_config.h"

    //Undef the macro for safety;
#undef CARTESIAN_GROUP

    //No error. Complete;
    return true;
}
