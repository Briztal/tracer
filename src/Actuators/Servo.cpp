/*
  ServoActions.cpp -  Part of TRACER

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


#include "Servo.h"

#include "hardware_language_abstraction.h"

#include <Config/actions_config.h>

void Servo::initialise_hardware() {

    //Initialise every servo in output mode;
#define SERVO(i, name, pin, ...) pin_mode_output(pin);

    //Expand for every servo;
#include <Config/actions_config.h>

    //Undef for safety;
#undef SERVO

}


void Servo::initialise_data() {

    //Hardware Reset.

    //Initialise every servo in output mode;
#define SERVO(i, name, pin, ...) digital_write(pin, LOW);

    //Expand for every servo;
#include <Config/actions_config.h>

    //Undef for safety;
#undef SERVO


    //Timer reset.
    disable_servo_timer();

    //Interrupt reset.
    disable_servo_interrupt();

    //Reset states;
    memset(states, 0, sizeof(float) * NB_SERVOS);

    //Reset states;
    memset(reloads, 0, sizeof(float) * NB_SERVOS);

    //Reset complement
    for (uint8_t i = 0; i<NB_SERVOS; i++) {
        reloads[i] = servo_period_to_reload(2500);
    }


}


void Servo::write(uint8_t id, float value) {

    //Verify that the required servo exists;
    if(id >= NB_SERVOS) {
        return;
    }

    //As the process for every servo is the same, we will use a macro.
#define SERVO(i, name, pin, min, max, pmin, pmax)\
    case i:\
        /*Force the value to be inside the bounds;*/\
        if (value < (min)) value = (min);\
        if (value > (max)) value = (max);\
        /*Determine the new period;*/\
        write_us(i, (pmin) + (((pmax) - (pmin)) / ((max) - (min))) * (value - (min)));\
        break;

    switch(id) {

        //Expand the code for all servos;
#include <Config/actions_config.h>

        //Undef the macro for safety;
#undef SERVO

        //Final case, no process.
        default:break;

    }


}


/*
 * write_us : this function updates the reload value of the provided timer at the desired one.
 *
 * It is private, and assumes that servo check has already been made;
 */

void Servo::write_us(uint8_t id, float us_period) {

    //Get the timer reload value;
    uint32_t timer_value = servo_period_to_reload(us_period);

    //Assign the timer value;
    reloads[id] = timer_value;

    update_complement();

}


/*
 * start : this function starts the servo interrupt routine.
 */

void Servo::start() {

    //Enable the servo timer;
    enable_servo_timer();

    //Enable the servo interrupt;
    enable_servo_interrupt();

    //Initialise the first servo;
    current_servo = 0;

    //Initialise the current pin;
    current_pin = *pins;

    //Start the interrupt routine;
    interrupt_routine();
}


/*
 * stop : stop a particular servo;
 */

void Servo::stop(uint8_t id) {

    //Verify that the required servo exists;
    if(id >= NB_SERVOS) {
        return;
    }

    //Nullify the reload value for this servo;
    reloads[id] = 0;

    //Update the complementary delay, as one reload value has been modified;
    update_complement();

}


/*
 * stop : stop all servos;
 */

void Servo::stop() {

    //Reset the environment;
    initialise_data();

}


/*
 * update_complement : this function will determine the period complement,
 *  so that a complete servo cycle lasts exactly one period.
 */

void Servo::update_complement() {

    //Intialise the new complement to its default value;
    uint32_t new_complement = SERVO_PERIOD_US;

    bool unactive = true;

    //Substract each duty cycle to the new complement;
    for (uint8_t i = 0; i < NB_SERVOS; i++) {

        //Cache the current reload value;
        uint32_t reload = reloads[i];

        //Update the new complement;
        new_complement -= reload;

        //If the reload is not null, enable;
        if (reload) unactive = false;

    }

    //Update the complement;
    complement = new_complement;

    //if all servos are inactive, stop the routine;
    if (unactive) {
        stop();
    } else {

        //If active and not started :
        if (!started) {

            //Start;
            start();

        }
    }

}


/*
 * interrupt_routine : this function handles the communication with servos;
 */

void Servo::interrupt_routine() {

    //Prevent the timer from re-happening;
    disable_servo_interrupt();

    //If the current state is an actual servo :
    if (current_servo != NB_SERVOS) {

        //Reset the previous data pin;
        digital_write(current_pin, 0);


        //Update the current pin;
        current_pin = pins[current_servo];

        //Set the servo's data pin;
        digital_write(current_pin, 1);

        //Set the new period;
        set_servo_int_reload(reloads[current_servo]);

    } else {
        //If the current state is the complement :

        //Reset the previous data pin;
        digital_write(current_pin, 0);

        //Reset the current servo;
        current_servo = 0;

        //Reset the current pin;
        current_pin = *pins;

        //Set the new period;
        set_servo_int_reload(complement);

    }

    //Re-enable the interrupt if the procedure is still active;
    if (started) {
        enable_servo_interrupt();
    }

}

