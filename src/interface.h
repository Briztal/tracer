/*
  config.h - Part of TRACER

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

#include "config.h"


#ifdef MAIN_CI_GCODE
#ifdef ENABLE_GCODE_INTERFACE
#define CI GI
#else
#error "Your main interface (the GCodeInterface) is not enabled."
#endif
#endif

#ifdef MAIN_CI_TREE
#ifdef ENABLE_TREE_INTERFACE
#define CI TI
#else
#error "Your main interface (the TreeInterface) is not enabled."
#endif
#endif

#include "Interfaces/GCodeInterface/GCodeInterface.h"
#include "Interfaces/TreeInterface/TreeInterface.h"
