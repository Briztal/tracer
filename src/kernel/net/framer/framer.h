//
// Created by root on 8/8/18.
//

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

	//The block where we save the result of the decoding;
	struct data_block *decoding_block;

	//The block that contains frames to encode;
	struct data_block *encoding_block;


	//Write a byte in the framer. Will assert if a frame has been entirely decoded;
	bool (*const decode)(struct data_framer *, uint8_t data);

	//Read (and discard) a byte from the current frame. Will assert if the frame has been entirely encoded;
	bool (*const encode)(struct data_framer *, uint8_t *data);

	//Delete the framer;
	void (*deleter)(struct data_framer *);

};

#endif //TRACER_FRAMER_H
