/*
  StepperController.h - Part of TRACER

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

#include <config.h>

#ifdef ENABLE_STEPPER_CONTROL

#ifndef CODE_STEPPER_CONTROLLER_H
#define CODE_STEPPER_CONTROLLER_H

#include <stdint.h>
#include <Config/stepper_control_config.h>


class Steppers {


public:

    //Initialise Actuators
    static void initialise_hardware();

    //Reset Actuators
    static void initialise_data();


    //-------------------------------------------------------Power------------------------------------------------------

public:

    //Enable a specific group of steppers, and disable the others.
    static void enable(sig_t signature);

    //Enable all steppers
    static void enable();

    //Disable all steppers
    static void disable();


    //----------------------------------------------------Directions----------------------------------------------------

public:

    //Set the directions of all steppers, according to signatures
    static void set_directions(sig_t negative_signatures);

private:

    //The current direction signature
    static sig_t direction_signature;


    //--------------------------------------------------------Step------------------------------------------------------

public:

    //Step a specific group of steppers
    static void fastStep(sig_t id);


    //----------------------------------------------------Position log--------------------------------------------------

    //Only if the position log is enabled
#ifdef position_log

public:

    //Send the position through the main interface
    static void send_position();

private:

    //Increment and position for every stepper
#define STEPPER(i, ...) \
    static int32_t incr##i;\
    public:static int32_t pos##i;\

#include <Config/stepper_control_config.h>

#undef STEPPER

#endif

};


#endif

#endif