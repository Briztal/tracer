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
#define MAX_COMMAND_SIZE 100

//The maximal size of a word
#define MAX_WORD_SIZE 40

//The maximal size of the argument part
#define MAX_ARGS_SIZE 50

//The maximum depth of the tree. //TODO DETERMINE THE MAXIMUM DEPTH WITH AN ALGORITHM
#define MAX_DEPTH 5

//The number of commands that can be received and stored before execution;
#define NB_PENDING_COMMANDS 10

#if defined(GO_LOWER) && defined(GO_UPPER) && defined(CREATE_LEAF)

#ifndef REQUIRE_ARG
#define REQUIRE_ARG(i, name)
#endif

#ifndef ALLOW_MULTIPLICITY
#define ALLOW_MULTIPLICITY(c, name)
#endif


//EEPROM
GO_LOWER(eeprom, move the machine)
CREATE_LEAF(read, eeprom_read, read a parameter in the EEPROM, eeprom_path)
CREATE_LEAF(write, eeprom_write, write a parameter in the EEPROM, eeprom_path)
CREATE_LEAF(reset, eeprom_reset, resets the EEPROM to its default state, none)
CREATE_LEAF(print, eeprom_print, print the content of the EEPROM, none)
GO_UPPER()

//Movement
GO_LOWER(move, move the machine)
CREATE_LEAF(home, move_home, homes the machine using endstops, none)
CREATE_LEAF(zero, move_zero, go to zero coordinates for all carriages, none)
CREATE_LEAF(to, line_to, draw a line with the current carriage to the specified coordinates, x - y - z)
CREATE_LEAF(of, line_of, draw a line with the current carriage to the specified offsets, x - y - z)
GO_UPPER()

//Setup
GO_LOWER(set, setup machine parameters)
CREATE_LEAF(carriage, set_carriage, set the current carriage and its speed, carriage_id - speed)
CREATE_LEAF(speed, set_speed, set the speed for the specified carriage, carriage_id - value)
GO_UPPER()

//test
GO_LOWER(test, various tests)
CREATE_LEAF(stepper, stepper_test, moves the stepper 0 at constant speed, none)
CREATE_LEAF(temp, temp_test, reads temps on every thermistor, none)
CREATE_LEAF(heater, heater_test, enables and disables every heaters, none)
GO_UPPER()


CREATE_LEAF(demo, action, a simple demo with all axis, i)

#undef GO_LOWER
#undef GO_UPPER
#undef CREATE_LEAF
#undef CREATE_CALLABLE_LEAF
#undef REQUIRE_ARG
#undef ALLOW_MULTIPLICITY

#endif


