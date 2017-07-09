/*
  parameters_indice.h - Part of TRACER

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


/* This file provides parameter indices for parameters you have set in parser_config. It gives you a "physical access"
 * in the code, instead of having to write an XMacro and to call parser_config.
 */

#ifndef CODE_PARAMETERS_INDICES_H
#define CODE_PARAMETERS_INDICES_H

#define GPARAMETER(i, j, k)\
    const char k##id = i;

//#include "parser_config.h"

#undef GPARAMETER

#endif //CODE_PARAMETERS_INDICES_H
