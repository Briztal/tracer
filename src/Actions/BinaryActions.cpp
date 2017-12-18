/*
  BinaryActions.cpp - Part of TRACER

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

#include "BinaryActions.h"


#include "../hardware_language_abstraction.h"



/*
 * This function will set up GPIO in the correct mode;
 */

void BinaryActions::initialise_hardware() {

    //Set the pin in output mode;
#define BINARY(i, name, pin, ev) pin_mode_output(pin, 0);

#include <Config/actions_config.h>

#undef BINARY

}

/*
 * This function will reset GPIO at their zero value;
 */
void BinaryActions::initialise_data() {

    //Write a logical zero in the pin;
#define BINARY(i, name, pin, ev) digital_write(pin, 0);

#include <Config/actions_config.h>

#undef BINARY

}


#define BINARY(i, name, pin, ev) \
void BinaryActions::enable##i() {\
    digital_write(pin, ev);\
}\
void BinaryActions::disable##i() {\
    digital_write(pin, !ev);\
}

#include <Config/actions_config.h>

#undef BINARY

