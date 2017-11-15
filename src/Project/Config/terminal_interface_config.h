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
GO_LOWER(eeprom, set persistent machine parameters)
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
GO_LOWER(set, setup real-time machine parameters)

CREATE_LEAF(carriage, set_carriage, set the current carriage and its speed, carriage_id - speed)
CREATE_LEAF(speed, set_speed, set the speed for the specified carriage, carriage_id - value)
CREATE_LEAF(power, enable_steppers, enable or disable all steppers, 0 or 1)

CREATE_LEAF(cooling_p, set_cooling_power, enable or disable the cooling, power)
CREATE_LEAF(cooling_en, enable_cooling, enable or disable the cooling, 0 or 1)

CREATE_LEAF(hotend_target, set_hotend_temp, set a hotends temperature, 0 or 1)
CREATE_LEAF(hotend_en, enable_hotend, enable or disable a hotend, 0 or 1)

CREATE_LEAF(bed_target, set_bed_temp, set the bed temperature, 0 or 1)
CREATE_LEAF(bed_en, enable_bed, enable or disable the bed, 0 or 1)

GO_UPPER()

//Get
GO_LOWER(get, get real time data)
CREATE_LEAF(temp, get_temps, display temperatures, none)
CREATE_LEAF(regs, get_regulations, display the temperatures regulation states, none)
GO_UPPER()



//test
GO_LOWER(test, various tests)
CREATE_LEAF(stepper, stepper_test, moves the stepper 0 at constant speed, none)
CREATE_LEAF(temp, temp_test, reads temps on every thermistor, none)
GO_UPPER()


CREATE_LEAF(demo, action, a simple demo with all axis, i)

#undef GO_LOWER
#undef GO_UPPER
#undef CREATE_LEAF
#undef CREATE_CALLABLE_LEAF
#undef REQUIRE_ARG
#undef ALLOW_MULTIPLICITY

#endif


