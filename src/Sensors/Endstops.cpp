/*
  Sensors.h -  Part of TRACER

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

#include "Endstops.h"

#include "hardware_language_abstraction.h"


//------------------------------------------ Const definition ------------------------------------------

//Initialise a counting macro;
#define ENDSTOP(...) 1 +

//Declare the endstop number;
const uint8_t NB_ENDSTOPS =

        //Count all endstops;
#include <Config/sensors_config.h>

        //Finish the count;
        0;

//Undef the macro for safety;
#undef ENDSTOP


//---------------------------------------------------- Init ----------------------------------------------------


/*
 * initialise_hardware : this function sets all GPIO in input mode;
 */

void Endstops::initialise_hardware() {

    //Create a macro that will read the state of an endstop
#define ENDSTOP(i, pin, enable_value) pin_mode_input(pin);

    //Read all endstops;
#include <Config/stepper_control_config.h>

    //Undef the macro for safety;
#undef ENDSTOP

}


/*
 * initialise_data : this function resets the states array;
 */

void Endstops::initialise_data() {

    //Reset all states;
    memset(states, 0, NB_ENDSTOPS * sizeof(bool));

}


//---------------------------------------------------- Direct read ----------------------------------------------------


/*
 * read_endstop_state : this function explicitly reads and returns the state of an endstop;
 */

bool Endstops::read_endstop_state(uint8_t index) {

    //Create a macro that will read an endstop
#define ENDSTOP(i, pin, enable_value)\
    case i:\
        /*Return true only if we read the enabled state, and save it;*/\
        return (states[i] = (digital_read((i)) == (enable_value)));

    //Try all possibilities for [index]. This is not efficient, reason why get_endstop_state and read_endstops exist.
    switch(index) {

        //Read all endstops;
#include <Config/stepper_control_config.h>

        //Undef the macro for safety;
#undef ENDSTOP

        //if the index does not correspond to any endstop, return false;
        default:
            return false;

    }

}

//---------------------------------------------------- Undirect read ----------------------------------------------------


/*
 * get_endstop_state : read the state of an endstop into the states array.
 *
 *  It is faster than a direct read, but you must ensure that the state has been updated recently;
 */

bool Endstops::get_endstop_state(uint8_t index) {

    //If the endstop doesn't exist :
    if (index >= NB_ENDSTOPS) {

        //Reset by default;
        return false;

    }

    //Return the previously read state;
    return states[index];

}


/*
 * read_endstops : read all endstops and save their state in the states array;
 */

void Endstops::read_endstops(uint8_t index) {

    //Create a macro that will read the state of an endstop
#define ENDSTOP(i, pin, enable_value) states[i] = (digital_read((i)) == (enable_value));\

        //Read all endstops;
#include <Config/stepper_control_config.h>

        //Undef the macro for safety;
#undef ENDSTOP

}

//---------------------------------------------------- Static declaration - definition ----------------------------------------------------

//Declare and init the states array;
bool t_endst_states[NB_ENDSTOPS]{false};
bool *const Endstops::states = t_endst_states;