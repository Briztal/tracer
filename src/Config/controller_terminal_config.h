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


/*
 * The Terminal Interface configuration file.
 *
 * This file defines the behaviour and the size of the parser.
 *
 */

//Put the name of your project here
#define PROJECT_NAME "TRACER"

//The maximal size of a command (id + content)
#define TERMINAL_MAX_SIZE 100

//The maximal size of an argument
#define TERMINAL_MAX_WORD_SIZE 20

//The maximal number of content
#define TERMINAL_MAX_ARGS_NB 6

//The maximum depth of the tree. //TODO DETERMINE THE MAXIMUM DEPTH WITH AN ALGORITHM
#define TERMINAL_MAX_DEPTH 5

//The number of commands that can be received and stored before execution;
#define TERMINAL_MAX_PENDING_COMMANDS 10

//Uncomment this line to be prompted about Terminal Tasks execution.
#define TERMINAL_EXECUTION_CONFIRMATION


/*
 * The following section defines the language of the parser.
 *
 *  This language is a tree, where every node is labeled by a word, and has a certain number of children.
 *
 *      For example, a simple language, supporting the three following commands :
 *          move home       (+args) -> call home_function
 *          move line of    (+args) -> call line_to_function
 *          move line to    (+args) -> call line_of_function
 *          halt            (+args) -> call halt_function
 *
 *      Will have its command tree like below :
 *
 *          move
 *             |- home ------> home_function
 *             |- line
 *             |    |- of ---> line_of_function
 *             |    |- to ---> line_to_function
 *          halt ------------> halt_function
 *
 *  The aim of this section is to define the structure of the tree.
 *
 *  To do this, you can use the 3 following commands :
 *
 *      - CREATE_LEAF(command, name)    : Create a leaf labelled "command", triggering the member function "name"
 *                              of the class TerminalCommands
 *      - GO_LOWER(command) :   following lines will define children of an new node labeled "command";
 *      - GO_UPPER() :          opposite of the previous command, terminate the section of the node defined by
 *                                  the last GO_LOWER
 *
 *
 *      For example, to build the tree in our previous example, we would write the following lines :
 *
 *      //Create a new node "move"
 *      GO_LOWER(move)
 *
 *      //Declare a command "move home" triggering home_function
 *      CREATE_LEAF(home, home_function)
 *
 *      //Create a new node "line"
 *      GO_LOWER(line)
 *
 *      //Declare a command "move line of" triggering line_of_function
 *      CREATE_LEAF(of, line_of_function)
 *
 *      //Declare a command "move line to" triggering line_to_function
 *      CREATE_LEAF(to, line_to_function)
 *
 *      //Finish the section of node "line"
 *      GO_UPPER()
 *
 *      //Finish the section of node "move"
 *      GO_UPPER()
 *
 *
 *      //Declare a command halt" triggering halt_to_function
 *      CREATE_LEAF(halt, halt_function)
 *
 */

#if defined(GO_LOWER) && defined(GO_UPPER) && defined(CREATE_LEAF)
//Write your commands here

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

#endif


