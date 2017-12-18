/*
  terminal_interface_config.h - Part of TRACER

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

//The maximal size of a command (id + arguments)
#define TERMINAL_MAX_SIZE 100

//The maximal size of an argument
#define TERMINAL_MAX_WORD_SIZE 20

//The maximal number of arguments
#define TERMINAL_MAX_ARGS_NB 6

//The maximum depth of the tree. //TODO DETERMINE THE MAXIMUM DEPTH WITH AN ALGORITHM
#define TERMINAL_MAX_DEPTH 5

//The number of commands that can be received and stored before execution;
#define TERMINAL_MAX_PENDING_COMMANDS 10

//Uncomment this line to be prompted about Terminal Tasks execution.
#define TERMINAL_EXECUTION_CONFIRMATION

#if defined(GO_LOWER) && defined(GO_UPPER) && defined(CREATE_LEAF)

#ifndef REQUIRE_ARG
#define REQUIRE_ARG(i, name)
#endif

#ifndef ALLOW_MULTIPLICITY
#define ALLOW_MULTIPLICITY(c, name)
#endif


//EEPROM
CREATE_LEAF(eeprom, eeprom, manage_eeprom)

//Movement
GO_LOWER(move, move the machine)
CREATE_LEAF(home, home, homes the machine)
CREATE_LEAF(line, line, draw a line with the current carriage to the specified coordinates)
GO_UPPER()

//Setup
GO_LOWER(set, setup real-time machine parameters)

CREATE_LEAF(power, enable_steppers, enable or disable all steppers)
CREATE_LEAF(position, set_position, enable or disable all steppers)
CREATE_LEAF(extrusion, set_extrusion, set the current carriage and its speed)
CREATE_LEAF(cooling, set_cooling, enable or disable the cooling)
CREATE_LEAF(hotend, set_hotend, set a hotends temperature)
CREATE_LEAF(hotbed, set_hotbed, set the bed temperature)

GO_UPPER()

//Get
GO_LOWER(get, get real time data)
CREATE_LEAF(temp, get_temps, display temperatures)
CREATE_LEAF(regs, get_regulations, display the temperatures regulation states)
GO_UPPER()


//test
GO_LOWER(test, various tests)
CREATE_LEAF(stepper, stepper_test, moves the stepper 0 at constant speed)
CREATE_LEAF(temp, temp_test, reads temps on every thermistor)
CREATE_LEAF(flood, flood, flood the movement manager)

GO_UPPER()


#undef GO_LOWER
#undef GO_UPPER
#undef CREATE_LEAF
#undef CREATE_CALLABLE_LEAF
#undef REQUIRE_ARG
#undef ALLOW_MULTIPLICITY

#endif


