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


bool _ConfigChecker::check_config(String *message) {

    //Check actions config;
    if (!check_actions(message)) return false;

    //Check transmission layers and control configuration;
    if (!check_transmission(message)) return false;

    //Check transmission layers and control configuration;
    if (!check_control_loops(message)) return false;

    return true;

}

/*
 * check_actions : this function verifies the action configuration;
 */

bool _ConfigChecker::check_actions(String *message) {

    //First, we will count the number of continuous actions.

    //Initialise the state as correct;
    bool state = true;

    //Reset the counter;
    uint8_t counter =

    //A macro to increment the counter for each action;
#define CONTINUOUS(...) 1 +

    //Count every continuous action;
#include <Config/actions_config.h>

    //Undef the macro for safety
#undef CONTINUOUS

     + 0;

    if (counter != NB_CONTINUOUS) {

        //Return a message and fail;
        *message = "the number of continuous actions specified doesn't match the listed actions. Check actions_config.h..";
        return false;

    }

    /*
     * Then, we will check if indices are consecutives;
     */
    counter = 0;

    //A macro to count controls;
#define CONTINUOUS(i, ...) \
    if (counter != i) {\
        *message = "PWM with index "#i" should have the index "+String(counter)+". Check actions_config.h";\
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
        *message = "The servo period cannot contain the number of servos you specified. Check actions_config.h.. Please reduce the amount of steppers or increase the period.";
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
        *message = "the number of servos specified doesn't match the listed servos. Check actions_config.h.";
        return false;

    }

    /*
     * Then, we will check if indices are consecutives;
     */
    counter = 0;

    //A macro to count controls;
#define SERVO(i, ...) \
    if (counter != i) {\
        *message = "Servo with index "#i" should have the index "+String(counter)+". Check actions_config.h";\
        return false;\
    }\
    counter++;

    //Count every control;
#include "Config/actions_config.h"

    //Undef the macro for safety;
#undef SERVO

    return true;

}


/*
 * check_transmission : This function verifies the configuration of the control chains;
 */

bool _ConfigChecker::check_transmission(String *message) {

    /*
     * First, we will check the number of controls;
     */

    uint8_t counter =

            //A macro to count controls;
#define EXTERNAL_CONTROL(c, p, s, transmission) 1 +

            //Count every control;
#include "Config/control_config.h"

    //Undef the macro for safety;
#undef EXTERNAL_CONTROL

    0;

    //Check the number of controls
    if (counter != NB_CONTROLS) {

        //Return a message and fail;
        *message = "the number of controls specified doesn't match the listed control. Check control_config.h.";
        return false;

    }


    /*
     * Next, we will check if one transmission link is used twice.
     */

    //Declare an array of functions;
    void (*functions[NB_CONTROLS])(void);

    //The counter will now count how many functions are in the array.
    counter  = 0;

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
            *message = "the transmission layer "#transmission" is used multiple times. Check control_config.h.";\
            return false;\
        }\
    }\
    /*Add the function to the array;*/\
    functions[counter++] = function;

    //Check every transmission layer used;
#include "Config/control_config.h"

    //Undef the amcro for safety;
#undef EXTERNAL_CONTROL

    //If no error, succeed;
    return true;

}


bool _ConfigChecker::check_control_loops(String *message) {

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
        *message = "the number of control loops specified doesn't match the listed loops. Check control_loops_config.h.";
        return false;

    }

    /*
     * Then, we will check if indices are consecutives;
     */
    counter = 0;

    //A macro to count controls;
#define LOOP_FUNCTION(i, ...) \
    if (counter != i) {\
        *message = "loop with index "#i" should have the index "+String(counter)+". Check control_loops_config.h";\
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
#define PID(...) 1 +

//Count every control;
#include "Config/control_loops_config.h"

    //Undef the macro for safety;
#undef PID

    0;

    //Check the number of controls
    if (counter != NB_PIDS) {

        //Return a message and fail;
        *message = "the number of control loops specified doesn't match the listed loops. Check control_loops_config.h.";
        return false;

    }

    /*
     * Then, we will check if indices are consecutives;
     */
    counter = 0;

    //A macro to count controls;
#define PID(i, ...) \
    if (counter != i) {\
        *message = "pid with index "#i" should have the index "+String(counter)+". Check control_loops_config.h";\
        return false;\
    }\
    counter++;

    //Count every control;
#include "Config/control_loops_config.h"

    //Undef the macro for safety;
#undef PID

    return true;
}


bool _ConfigChecker::check_controllers(String *message) {
    return false;
}

