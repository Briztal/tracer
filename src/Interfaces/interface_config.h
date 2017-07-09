/*
  interface_config.h - Part of TRACER

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
 *
 * //TODO DOC ABOUT ARGS REQUIREMENT
 *
 * The syntax of the command interface :
 *
 * A command is a sequence of byte.
 * - the first part determines the command;
 * - the other is composed of the argument sequence : size - [size] bytes of data;
 * - 0 to end;
 *
 * Example : 1 : 20 : 254 : 1 : 7 : 0 : 0 : 0 : 0 : 0 : 0 : 0
 *         :    command id    :             args              :
 *
 * For the example, all data bytes are 0.
 *
 * The command id has a variable size, determined by a command tree, which is defined by you.
 *
 * For example, the upper command id matches the function 3 in the command tree below :
 *
 * - root
 *      - 1
 *          - 1 -> function 0;
 *          - 20
 *              - 69 -> function_1;
 *              - 120 -> function_2;
 *              - 254 -> function_3;
 *      - 200 -> function_4;
 *
 * (In this type of tree, leafs are commands, and non leaf nodes contain multiple commands);
 *
 * -----------------------------------------------------------------------------------------------------------------
 *
 * Depending on which modules you have enabled, you can activate system canals.
 *
 * These canals are automatically detected by the high level, and used by it to monitor specific functions of the machine.
 *
 * Those canals are :
 *  - asserv : the canal to monitor Control loops,
 *  - steppers
 *
 *
 * -----------------------------------------------------------------------------------------------------------------
 *
 * To define your command tree, you have access to the following commands
 *
 * GO_LOWER(char c) :
 *      creates a non-leaf node, labeled by c, under the actual node, and goes its level;
 *
 * GO_UPPER() :
 *      returns to the parent node;
 *
 * CREATE_LEAF(char c, void (*f)(void)) :
 *      creates a leaf node, labeled by c, pointing to the function f.
 *
 * System leafs creation :
 * You can notify the higher level that a particular leaf leads to a system function.
 *
 * CREATE_ASSERV_LEAF(char c) : creates the asserv canal, with the name defined below
 *
 * CREATE_STEPPERS_LEAF(char c) : creates the steppers canal, with the name defined below
 *
 * //TODO EXAMPLES WITH SYSTEM CANALS
 *
 * To keep the upper example, to define the upper command tree, you may write the following code :
 *
 * GO_LOWER(1)
 * CREATE_LEAF(1, function_0)
 * GO_LOWER(20)
 * CREATE_CALLABLE_LEAF(69, function_1)
 * CREATE_LEAF(120, function_2)
 * CREATE_LEAF(254, function_3)
 * CREATE_ASSERV_LEAF(254, function_3)
 * GO_UPPER
 * GO_UPPER
 * CREATE_STEPPERS_LEAF(200, function_4)
 *
 *  -----------------------------------------------------------------------------------------------------------------
 *
 * Once you have used one of commands CREATE_LEAF and CREATE_CALLABLE_LEAF,
 * you also have access to  documentation commands :
 * REQUIRE_ARG(type, name) : tells the high level that an argument of type 'type', named 'name' is required by
 *      the last defined function.
 *
 *      Types are :
 *      - 0 : bool;
 *      - 1 : char;
 *      - 2 : int;
 *      - 3 : long;
 *      - 4 : float;
 *
 *      Remark : when you send a string, you must send_packet its size at its beginning;
 *
 * ALLOW_MULTIPLICITY(count, name) : tells the high level that last 'count' arguments (declared using REQUIRE_ARGS)
 *      can be sent multiple times (ex : to move on several axis, [AXIS_ID, VALUE] args will be sent for each concerned axis.
 *
 *  -----------------------------------------------------------------------------------------------------------------
 *
 * //TODO CHECK THE FUNCTION SIGNATURE UPPER
 * //TODO IMPLEMENT MORE COMMANDS : CREATE_LOWER_BINARY : cree un node binaire fils
 */

#define MONITOR_CANAL

#define MAX_DEPTH 3


#if defined(GO_LOWER) && defined(GO_UPPER) && defined(CREATE_LEAF)

#ifndef REQUIRE_ARG
#define REQUIRE_ARG(i, name)
#endif

#ifndef ALLOW_MULTIPLICITY
#define ALLOW_MULTIPLICITY(c, name)
#endif

//Insert your tree here

CREATE_LEAF(1, action)
CREATE_LEAF(2, home)
GO_LOWER(3, motion)
GO_UPPER
GO_LOWER(4, preparation)
GO_UPPER

#undef GO_LOWER
#undef GO_UPPER
#undef CREATE_LEAF
#undef CREATE_CALLABLE_LEAF
#undef REQUIRE_ARG
#undef ALLOW_MULTIPLICITY

#endif

//The baudrate of the serial link
#define BAUDRATE 115200

//The maximum size of a data in one message
#define PACKET_SIZE 200

