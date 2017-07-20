/*
  BinaryActions.cpp - Part of TRACER

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

#include "BinaryActions.h"


#include "../hardware_language_abstraction.h"


#define BINARY(i, name, pin, ev) \
void BinaryActions::enable##i() {\
    digitalWriteFast(pin, ev);\
}\
void BinaryActions::disable##i() {\
    digitalWriteFast(pin, !ev);\
}


#include "../config.h"

#undef BINARY

