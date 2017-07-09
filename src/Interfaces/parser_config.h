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

//TODO DOC

#ifdef GCOMMAND

/*
    GCode Gcommands :

    Rules to follow :
    - All indices must be consecutives;
    - the function to call mest be a public non static member function of ActionController, taking and returning void;
        parameters can only be transmitted by current_gcode parameters array, which rules are defined in section below;

*/

GCOMMAND(0, homeMachine)
GCOMMAND(1, absoluteMove)
GCOMMAND(2, ellipticMove)
GCOMMAND(3, bezierMove)

#endif

#ifdef MCOMMAND

/*
    GCode Mcommands :

    Rules to follow :
    - All indices must be consecutives;
    - the function to call mest be a public non static member function of ActionController, taking and returning void;
        parameters can only be transmitted by current_gcode parameters array, which rules are defined in section below;

*/


MCOMMAND(1, open)
MCOMMAND(2, close)
MCOMMAND(3, set)
MCOMMAND(4, clear)
MCOMMAND(5, adjustPower)
MCOMMAND(6, adjustLinearPower)
MCOMMAND(7, adjustSpeedPower)


#endif


#ifndef PARAMETERS_NB
#define PARAMETERS_NB 9
#endif

#ifdef GPARAMETER

/*
    GCode Parameters :
    place here all parameters you need in your commands, with this parameter scheme :
    GCODE_PARAMETER_STATEMENT(indice, 'matching letter', matching letter)

    Rules to follow :
    - All indices must be consecutives;
    - Matching letter mustn't be a number, who are reserved for implicit coordinates;
*/

#undef F;

GPARAMETER(0, 'V', V)
GPARAMETER(1, 'A', A)
GPARAMETER(2, 'B', B)
GPARAMETER(3, 'S', S)
GPARAMETER(4, 'R', R)
GPARAMETER(5, 'U', U)
GPARAMETER(6, 'C', C)
GPARAMETER(7, 'E', E)
GPARAMETER(8, 'L', L)

#endif

