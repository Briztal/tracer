/*
  ascii_framer.c Part of TRACER

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

#include <net/netf.h>
#include "../../../std/syscall.h"
#include "framer.h"

/*
 * The ascii framer is composed of a data framer, and TODO
 */

struct ascii_framer {

	//The data framer base;
	struct data_framer framer;


	//The index of the current insertion position in the decoding buffer;
	size_t insertion_index;

	//A flag, set when the current frame is too long for the buffer;
	bool decoding_unsafe;


	//The index where to read bytes to encode;
	size_t read_index;

	//Has the EOF line feed and carriage return been sent ?
	bool EOF_lf_sent;


};


//Private headers;
void ascii_framer_deleter(struct data_framer *framer) {};

bool ascii_framer_decode(struct ascii_framer *framer, uint8_t data);

bool ascii_framer_get_encoded_byte(struct ascii_framer *framer, uint8_t *data);


/**
 * ascii_framer_create : creates and initialises an ascii framer, and return its framer casted version;
 * @return the casted version of the framer;
 */

struct data_framer *ascii_framer_create() {

	struct ascii_framer init = {

		.framer = {

			//Blocks not initialised for instance;
			.encoding_block = 0,
			.decoding_block = 0,

			.encode = (bool (*)(struct data_framer *, uint8_t *)) &ascii_framer_get_encoded_byte,
			.decode = (bool (*)(struct data_framer *, uint8_t)) &ascii_framer_decode,
			.deleter = &ascii_framer_deleter,
		},

		//Decoding safe;
		.insertion_index = 0,
		.decoding_unsafe = false,

		//Encoding safe;
		.read_index = 0,
		.EOF_lf_sent = false,

	};

	//Allocate, initialise and return the up-casted ascii framer;
	return (struct data_framer *) kernel_malloc_copy(sizeof(struct ascii_framer), &init);

}


/**
 * ascii_framer_decode : transmits @data to @framer for decoding;
 *
 * 	If @data is an EOF, update the block and assert if the frame is not null;
 *
 * 	If @data is a character, attempt to insert it.
 *
 * 	If the frame we are decoding is too long for the decoding block, it is discarded, and no data is inserted until
 * 	the next EOF
 *
 * @param framer : the framer that must receive the data byte;
 * @param data : the data byte to transmit;
 * @return true if a block update is required;
 */

bool ascii_framer_decode(struct ascii_framer *const framer, const uint8_t data) {

	//Cache the insertion index;
	size_t insertion_index = framer->insertion_index;

	//Cache the decoding block;
	struct data_block *decoding_block = framer->framer.decoding_block;

	//If the byte is an end of frame :
	if ((data > '~') || (data < ' ')) {

		//An end of frame as been received. We can go back in safe state;
		framer->decoding_unsafe = false;;
		//Reset the insertion index for future insertion;
		framer->insertion_index = 0;

		//If the insertion index is non null (non-empty frame) :
		if (insertion_index) {

			//Update the data block;
			decoding_block->size = insertion_index;

			//The frame is complete, a block update is required;
			return true;

		} else {

			//The frame is empty, no block update required;
			return false;

		}

	} else {

		//The byte is a frame character;

		//If we are in an unsafe state :
		if (framer->decoding_unsafe) {

			//Do nothing, no block update required;
			return false;

		}

		//If the insertion would overflow the data block :
		if (insertion_index >= decoding_block->max_size) {

			//Go in the unsafe state. No data will be inserted until the next EOF;
			framer->decoding_unsafe = true;

		} else {

			//The byte can be inserted;

			//Insert it;
			*((uint8_t *) decoding_block->address + insertion_index) = data;

			//Increment the insertion index;
			framer->insertion_index = insertion_index + 1;

		}


		//No block update is required;
		return false;

	}

}


/**
 * ascii_framer_get_encoded_byte : encodes @framer's and store one byte of the resulting stream in @data;
 *
 * 	If no byte can be found (empty block), 0 is returned;
 *
 * 	If the provided byte completes the encoding, true is returned, for a block update;
 *
 * @param framer : the framer that must provide the encoded byte;
 * @param data : the location where the byte must be stored;
 * @return true if the frame must be updated;
 */

bool ascii_framer_get_encoded_byte(struct ascii_framer *const framer, uint8_t *const data) {

	//Cache the reading index;
	size_t read_index = framer->read_index;

	//Cache the encoding block;
	struct data_block *encoding_block = framer->framer.encoding_block;

	//If we reached the end of the encoding block :
	if (read_index == encoding_block->size) {

		//If the EOF line feed has not been sent :
		if (!framer->EOF_lf_sent) {

			//If line feed not sent, send one;
			*data = '\n';

			//Mark line feed sent;
			framer->EOF_lf_sent = true;

			//EOF carriage return not sent, no block update for instance;
			return false;

		} else {

			//Send a carriage return;
			*data = '\r';

			//Reset the read index, and the EOF flag;
			framer->read_index = 0;
			framer->EOF_lf_sent = false;

			//Require an encoding block update;
			return true;

		}

		//Never reached;

	}

	//Block still contains data to be sent;

	//Send the byte at the reading position;
	*data = *((uint8_t *) encoding_block->address + read_index);

	//Update the read index;
	framer->read_index = read_index + 1;

	//Block end not reached for instance, no block update required;
	return false;

}
