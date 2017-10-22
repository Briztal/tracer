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
#define MAX_WORD_SIZE 20

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

//Insert your tree here
CREATE_LEAF(demo, action, a simple demo with all axis, i)
CREATE_LEAF(stress, stress, a repetitive command to show the kinetics limits, i)
CREATE_LEAF(ex, home, home the machine, <dir>)
GO_LOWER(move, move the machine)
CREATE_LEAF(line, line, draw a line to the specified coordinates, <coordinates>)
GO_UPPER()
GO_LOWER(setup, setup the machine)
CREATE_LEAF(speed, speed, set the speed, <value>)
GO_UPPER()

#undef GO_LOWER
#undef GO_UPPER
#undef CREATE_LEAF
#undef CREATE_CALLABLE_LEAF
#undef REQUIRE_ARG
#undef ALLOW_MULTIPLICITY

#endif


