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
 * The GCode Interface configuration file.
 *
 * This file defines the behaviour of the parser. You can personalise the following parameters :
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
 * For example, to effectively implement the tree mentioned above, you may write the following commands :
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

//The maximum number of arguments in a GCodeCommand.
#define GCODE_MAX_ARGS_NB 6

//The maximal size of an argument
#define GCODE_MAX_WORD_SIZE 20

//The number of commands that can be received and stored before execution;
#define GCODE_MAX_PENDING_COMMANDS 10

//The maximum size of a GCode Command.
#define GCODE_MAX_SIZE 100




#if defined(GCODE_COMMAND)

//Movement
GCODE_COMMAND(G01, home)
GCODE_COMMAND(G01, line)

//Setup
GCODE_COMMAND(G10, enable_steppers)
GCODE_COMMAND(G11, set_position)
GCODE_COMMAND(G12, set_extrusion)
GCODE_COMMAND(G13, set_cooling)
GCODE_COMMAND(G14, set_hotend)
GCODE_COMMAND(G15, set_hotbed)

//Get
GCODE_COMMAND(G20, get_temps)
GCODE_COMMAND(G21, get_regulations)

//test
GCODE_COMMAND(G30, stepper_test)
GCODE_COMMAND(G31, temp_test)
GCODE_COMMAND(G32, action)

//Eeprom
GCODE_COMMAND(G4, eeprom)


/*
//G COMMANDS
GO_LOWER('G')


//G0
COMMAND('0', linear_move)

GO_LOWER('1')

//G10
COMMAND('0', retract)

//G11
COMMAND('1', unretract)

//G12
COMMAND('2', clean_tools)

//G17
COMMAND('7', plane_xy)

//G18
COMMAND('8', plane_zx)

//G19
COMMAND('9', plane_yz)

GO_UPPER()


//G2
GO_LOWER_COMMAND('2', arc_move_p)

//G20
COMMAND('0', units_to_inches)

//G21
COMMAND('1', units_to_millimeters)

//G28
COMMAND('8', home)

GO_UPPER()


//G3
GO_LOWER_COMMAND('3', arc_move_n)

//G32
COMMAND('2', auto_level)


GO_UPPER()


//G4
COMMAND('4', dwell)

//G9
GO_LOWER(9)

//G90
COMMAND('0', absolute_position)

//G91
COMMAND('1', relative_position)

//G92
COMMAND('2', set_position)

GO_UPPER()

GO_UPPER()


//M COMMANDS
GO_LOWER('M')


//M0
COMMAND('0', stop)


//M1
GO_LOWER('1')

//M17
COMMAND('7', enable_steppers)

//M18
COMMAND('8', disable_steppers)

GO_UPPER()


//M8..
GO_LOWER('8')

//M82
COMMAND('2', absolute_extruder)

//M83
COMMAND('3', relative_extruder)

GO_UPPER()


//M9...
GO_LOWER('9')

COMMAND('2', set_steps_per)

GO_UPPER()



GO_UPPER()


*/




#endif


#define NB_PARAMETERS 9

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



GCODE_PARAMETER(0,  'T',    T)
GCODE_PARAMETER(1,  'S',    S)
GCODE_PARAMETER(2,  'P',    P)
GCODE_PARAMETER(3,  'X',    X)
GCODE_PARAMETER(4,  'Y',    Y)
GCODE_PARAMETER(5,  'Z,     Z)
GCODE_PARAMETER(6,  'I',    I)
GCODE_PARAMETER(7,  'J',    J)
GCODE_PARAMETER(8,  'D',    D)
GCODE_PARAMETER(9), 'H',    H)
GCODE_PARAMETER(10, 'F',    F)
GCODE_PARAMETER(11, 'R',    R)
GCODE_PARAMETER(12, 'Q',    Q)
GCODE_PARAMETER(13, 'E',    E)
GCODE_PARAMETER(14, 'N',    N)

#endif

