/*
  ServoActions.h -  Part of TRACER

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

#ifndef CODE_SERVOACTIONS_H
#define CODE_SERVOACTIONS_H

#include "stdint.h"

class Servo {

public:

    //Setup Actuators in output mode;
    static void initialise_hardware();

    //Disable the timer
    static void initialise_data();

    //Set the output of a servo at a particular value;
    static void write(uint8_t id, float value);

    //Start the routine;
    static void start();

    //Disable a servo by id;
    static void stop(uint8_t id);

    //Disable all servos;
    static void stop();


private:

    //Set the output of a servo at a particular value;
    static void write_us(uint8_t id, float us_period);

    //Update the complement to all delays;
    static void update_complement();

    //The interrupt function;
    static void interrupt_routine();

    //The status flag;
    static volatile bool started;

    //States;
    static bool *states;

    //Periods;
    static uint32_t *reloads;

    //Pins;
    static uint8_t *pins;

    //Current pin;
    static uint8_t current_pin;

    //The complement to all delays;
    static uint32_t complement;

    static uint8_t current_servo;

};


#endif //CODE_SERVOACTIONS_H
