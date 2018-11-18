/*
  ascii_framer.h Part of TRACER

  Copyright (c) 2018 Raphaël Outhier

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


#ifndef TRACER_ASCII_FRAMER
#define TRACER_ASCII_FRAMER


#include "framer.h"


/*
 * The ascii framer delimits messages according to the nature of a character.
 *
 * 	This framer is unsafe (sending one frame can result in multiple frames read) and is for human if only.
 *
 * 	Decoding :
 *
 * 	Any character between ' ' (space, 0x20) and '~' (tild, 0x7E) is considered as a frame content, any other
 * 	character is considered as an end of frame;
 *
 * 	Encoding :
 *
 * 	The size of the frame is provided, all characters will be sent. The frame will be terminated by a "\n\r" EOF;
 */

//Create and initialise a human readable framer;
struct data_framer *ascii_framer_create();


#endif //TRACER_ASCII_FRAMER
