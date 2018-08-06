//
// Created by root on 8/1/18.
//

#ifndef TRACER_PACKET_SEGMENTER_H
#define TRACER_PACKET_SEGMENTER_H

#include <stdint.h>

#include <stddef.h>

/*
 * A packet framer has to functions :
 * 	- encoding : converting messages into formatted packets;
 * 	- decoding : receiving byte streams, and decoding formatted packets in real time;
 *
 * 	The encoding function always succeeds, and returns the size of the encoded message;
 *
 * 	The decoding function executes a real time processing, and is not sure to succeed. It will return a state that
 * 	describes the result of the processing;
 * 	It takes in argument the byte to decode, the address of the first byte of the dst array, and the position at which
 * 	it should insert data in the array. If insertion there is, this position will be updated;
 */

enum framer_decoding_status_t {

	//The decoding is not complete;
		FRAMER_DECODING_INCOMPLETE,

	//The decoding failed,
		FRAMER_DECODING_COMPLETE,

	//The decoding function encountered an error;
		FRAMER_DECODING_FAILED,

};


struct framer_t {

	//Encode a message;
	size_t (*const encode)(struct framer_t *framer, const void *src, size_t src_size, void *dst, size_t dst_size);

	//Receive a byte of a message, decode in real time, and eventually insert data in the dst array;
	enum framer_decoding_status_t (*const decode)(struct framer_t *framer, uint8_t byte, void *dst_init,
												  size_t *current_position);

	//Destructor;
	void (*const destructor)(struct framer_t *);

};



//Inline shortcuts;

inline size_t framer_encode(struct framer_t *framer, void *src, size_t src_size, void *dst, size_t dst_size) {

	(*(framer->encode))(framer, src, src_size, dst, dst_size);

}

enum framer_decoding_status_t framer_decode(struct framer_t *framer, uint8_t byte, void *dst_init,
											size_t *current_position) {

	(*(framer->decode))(framer, byte, dst_init, current_position);
}

void framer_destruct(struct framer_t *framer) {

	(*(framer->destructor))(framer);
}


#endif //TRACER_PACKET_SEGMENTER_H
