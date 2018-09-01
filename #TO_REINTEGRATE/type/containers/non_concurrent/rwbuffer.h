//
// Created by root on 7/15/18.
//

#ifndef TRACER_rwbuffer_H
#define TRACER_rwbuffer_H

/*
 * The rwbuffer is a contiguous buffer, that can store a determined number of bytes.
 *
 * 	Its primary goal is to store data received from an undefined transmitter that erratically inserts data, for an
 * 	undefined receiver that erratically reads data. Its secondary goal is to prevent memory leak in a badly
 * 	dimensioned interaction chain, by supporting data corruption;
 *
 * 	It is designed to be manipulable by DMA, and to support data overflow in its working principle :
 * 	when the buffer is full (data available = buffer_size), and the receiver tries to insert data, there will be a
 * 	data corruption at some point, because the interaction chain is not dimensioned well.
 *
 * 	It is not the rwbuffer to maintain data integrity, so it will always accept data. Data can be read the insertion
 * 	address doesn't equal the discarding address byte available. There has been a data corruption, and a message
 * 	fragment of buffer_size - 1 bytes has been lost, but no memory leak has	happened;
 */
#include <stdbool.h>

#include <stdint.h>

#include <stddef.h>



/*
 * The rwbuffer is composed of the following elements :
 */
typedef struct {

	//------------------------- Buffer constants -------------------------

	//The buffer start address, constant;
	void *const buffer_min;

	//The buffer's limit address, not allocated, constant;
	void *const buffer_max;

	//The buffer total size, constant;
	const size_t buffer_size;


	//------------------------- Access channels -------------------------

	//Input channel;
	void *input_address;

	//Output channel;
	const void *output_address;


} rwbuffer_t;


//----------------------------------------------------- Init - prempt ----------------------------------------------------

//Initialise the buffer with direct access on both sides;
rwbuffer_t *rwbuffer_initialise(size_t size);

//Delete a created buffer;
void rwbuffer_delete(rwbuffer_t *buffer);


//----------------------------------------------------- Manipulation ----------------------------------------------------

//If space is available, insert data into the buffer; Does nothing if no space;
void rwbuffer_insert(rwbuffer_t *buffer, uint8_t *data, size_t size);

//If data is available, remove and return data from the buffer. Returns 0 if no data;
size_t rwbuffer_remove(rwbuffer_t *buffer, uint8_t *dest, size_t size);


#endif //TRACER_rwbuffer_H
