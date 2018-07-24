/*
  parser_config.h - Part of TRACER

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
  along with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/

/*
 * The GCode Interface configuration file_tree.
 *
 * This file_tree defines the behaviour of the parser. You can personalise the following parameters :
 *      - The commands (G101, M1 commands for example).
 *      - The Parameters you can pass (X, Y, Z, A, B, etc...)
 *      - The function a special command triggers (ex : in Reprap GCode, G1 triggers a linear enqueue_movement function)
 *
 * Command are created with a simple tree. For example, to include next commands :
 *      G0, G1, M1, M2, G10, M225 and G5D0
 *
 *      You would define the following tree :
 *
 *      G
 *      -- 0 -> g0
 *      -- 1 -> g1
 *      ----- 0 -> g10
 *      -- 5
 *      ----- D
 *      -------- 0 -> g520
 *      M
 *      -- 1 -> m1
 *      -- 2 -> m2
 *      ----- 2
 *      -------- 5 -> m225
 *
 * Where nodes followed by * effectively trigger commands (note that final nodes must always trigger a command)
 *
 * This structure is a little bit hard to initially approach, but it has two major advantages :
 *      - It allows to define all types of commands (1F5EF for example).
 *      - The code generated with it is REALLY more optimised than a typical GCode parser like.
 *          For example, in Marlin, there are two switch (M and G) over all commands indices.
 *          These code practices are extremely inefficient.
 *          Here, at every node, a switch is made over the direct children.
 *
 * To define the command tree, you have access to the three following commands :
 *      COMMAND(k1_position_indice, fname)           : defines a leaf node, that triggers a method in MachineController::fname
 *      GO_LOWER_COMMAND(k1_position_indice, fname)  : defines a non leaf node that triggers MachineController::fname if it terminates a GCode (ex : G1 upper), and go to its level
 *      GO_LOWER(k1_position_indice)                 : defines a non leaf node, that does not trigger any function, and goes to its level
 *      GO_UPPER()                  : returns to the parent node level
 *
 *
 * For example, to effectively implement the tree mentioned above, you may write_data the following commands :
 *
 * GO_LOWER('G')
 * COMMAND('0', g0)
 * GO_LOWER_COMMAND('1', g1)
 * COMMAND('0', g10)
 * GO_UPPER()
 * GO_LOWER('5')
 * GO_LOWER('D')
 * GO_LOWER_COMMAND('0', g520)
 * GO_UPPER()
 * GO_UPPER()
 * GO_UPPER()
 * GO_LOWER('M')
 * GO_LOWER('1', m1)
 * GO_LOWER_COMMAND('2', m2)
 * GO_LOWER('2')
 * COMMAND('5', m255)
 *
 *
 */

//The maximum command depth : in our example, the two largest commands are G5D0 and M225 -> GCODE_MAX_DEPTH of 4
#define GCODE_MAX_DEPTH 5

#if defined(GO_LOWER) && defined(GO_LOWER_COMMAND) && defined(COMMAND) && defined(GO_UPPER)


GO_LOWER('G')
COMMAND('0', g0)
GO_LOWER_COMMAND('1', g1)
COMMAND('0', g10)
GO_UPPER()
GO_LOWER('5')
GO_LOWER('D')
COMMAND('0', g5d0)
GO_UPPER()
GO_UPPER()
GO_UPPER()
GO_LOWER('M')
COMMAND('1', m1)
GO_LOWER_COMMAND('2', m2)
GO_LOWER('2')
COMMAND('5', m225)
GO_UPPER()
GO_UPPER()
GO_UPPER()

#endif


#ifndef NB_PARAMETERS
#define NB_PARAMETERS 9
#endif

#ifdef GCODE_PARAMETER
#undef F;

/*
    GCode Parameters :
    place here all parameters you need in your commands, with this parameter scheme :
    GCODE_PARAMETER_STATEMENT(indice, 'matching letter', matching letter)

    Rules to follow :
    - All indices must be consecutives;
    - Matching letter mustn't collide with steppers letters;
*/



GCODE_PARAMETER(0, 'V', V)
GCODE_PARAMETER(1, 'A', A)
GCODE_PARAMETER(2, 'B', B)
GCODE_PARAMETER(3, 'S', S)
GCODE_PARAMETER(4, 'R', R)
GCODE_PARAMETER(5, 'U', U)
GCODE_PARAMETER(6, 'C', C)
GCODE_PARAMETER(7, 'E', E)
GCODE_PARAMETER(8, 'L', L)

#endif

