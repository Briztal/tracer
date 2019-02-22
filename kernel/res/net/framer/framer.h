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


#ifndef TRACER_FRAMER_H
#define TRACER_FRAMER_H

#include <stdbool.h>

#include <stdint.h>

#include <stddef.h>


/*
 * A framer turns byte stream into framed messages, and framed messages into byte streams;
 *
 * 	Messages are stored in blocks, any attempt to use read or write when related blocks are not initialised will
 * 	throw an error;
 */

struct data_framer {

	/*The block where we save the result of the decoding;*/
	struct data_block *decoding_block;

	/*The block that contains frames to encode;*/
	struct data_block *encoding_block;


	/*Write a byte in the framer. Will assert if a frame has been entirely decoded;*/
	bool (*const decode)(struct data_framer *, uint8_t data);

	/*Read (and discard) a byte from the current frame. Will assert if the frame has been entirely encoded;*/
	bool (*const encode)(struct data_framer *, uint8_t *data);

	/*Delete the framer;*/
	void (*deleter)(struct data_framer *);

};

#endif /*TRACER_FRAMER_H*/
