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
 * The ByteTreeLanguage Interface configuration file.
 *
 * This file defines the behaviour and the size of the parser.
 */


//The maximum command depth : in our example, the two largest commands are G5D0 and M225 -> GCODE_MAX_DEPTH of 4
#define GCODE_MAX_DEPTH 5

//The maximum number of arguments in a GCodeCommand.
#define GCODE_MAX_ARGS_NB 6

//The maximal size of an argument
#define GCODE_MAX_WORD_SIZE 20

//The number of commands that can be received and stored before execution;
#define GCODE_MAX_PENDING_COMMANDS 10

//The maximum size of a ByteTreeLanguage Command.
#define GCODE_MAX_SIZE 100


/*
 * The following section defines the language of the parser.
 *
 *  It contains all commands that will be supported by the parser.
 *
 *  To add a command, triggering a function in GCodePipe, use the following command :
 *
 *  GCODE_COMMAND(command_name, function)
 *
 *      - command_name :    the name of the ByteTreeLanguage Command (ex : G352)
 *      - function :        the name of the function in GCodePipe that must be called when the command is received.
 *
 *  For example, to call the member function "line" of GCodePipe class when the command "G01" is called,
 *      you will write :
 *
 *      GCODE_COMMAND(line, G01)
 *
 *
 *  You can declare your commands in any order you desire, the only usage of this section is to build tree,
 *      process_t that will involve sorting commands by name;
 */

#ifdef GCODE_COMMAND
//Write your commands here

//Movement
GCODE_COMMAND(G00, home)
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
