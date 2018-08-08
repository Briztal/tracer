//
// Created by root on 8/8/18.
//


#ifndef TRACER_ASCII_FRAMER
#define TRACER_ASCII_FRAMER


#include "framer.h"


/*
 * The ascii framer delimits messages according to the nature of a character.
 *
 * 	This framer is unsafe (sending one frame can result in multiple frames read) and is for human interface only.
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
