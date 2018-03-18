
/*
  Terminal.cpp - Part of TRACER

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

#include <stdint.h>

#include "TerminalLanguage.h"

/*
 * Constructor : initialises the two parent superclasses;
 */

TerminalLanguage::TerminalLanguage(uint8_t max_children_nb, uint8_t max_word_size, uint8_t max_arguments_nb) :
        StringTreeLanguage(max_word_size, max_children_nb), CharArgumentLanguage(max_arguments_nb) {}



void TerminalLanguage::encode(tstring &s, uint8_t type) {

}
