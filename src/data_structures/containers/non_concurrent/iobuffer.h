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
 * 	dimensioned communication chain, by supporting data corruption;
 *
 * 	It is designed to be manipulable by DMA, and to support data overflow in its working principle :
 * 	when the buffer is full (data available = buffer_size), and the receiver tries to insert data, there will be a
 * 	data corruption at some point, because the communication chain is not dimensioned well.
 *
 * 	It is not the rwbuffer to maintain data integrity, so it will always accept data. Data can be read the insertion
 * 	address doesn't equal the discarding address byte available. There has been a data corruption, and a message
 * 	fragment of buffer_size - 1 bytes has been lost, but no memory leak has	happened;
 */


#include <stdbool.h>

#include <stdint.h>

#include <stddef.h>

#include "instance.h"


struct iobuffer_t;

/*
 * The base iobuffer channel type. Every channel will be composed of this struct; It is composed of the following
 * 	elements :
 */

typedef struct {

	//The buffer the channel relates to; Pointer is volatile, also serves of registeration state;
	struct iobuffer_t *volatile buffer;


	//------------------------------------ Transfer fields ------------------------------------

	//The address where data starts;
	void *data_address;

	//The maximal size of the transfer; Must be updated to the effective transfer size before calling persist;
	size_t transfer_size;


	//------------------------------------ Function pointers ------------------------------------

	//A function pointer to update the channel, after transfer fields have been updated;
	void (*const update)(struct iobuffer_channel_t *channel);

	//A function pointer to reset the channel to a non-transferring state;
	void (*const reset)(struct iobuffer_channel_t *channel);

	//A function pointer to delete the channel, constant;
	void (*const destructor)(struct iobuffer_channel_t *channel);

} iochannel_t;


//Update shortcut;
inline void iochannel_update(iochannel_t *const channel) {
	(*(channel->update))(channel);
}

//Reset shortcut;
inline void iochannel_reset(iochannel_t *const channel) {
	(*(channel->reset))(channel);
}


/*
 * The rwbuffer is composed of the following elements :
 */

typedef struct iobuffer_t {

	//------------------------- Buffer constants -------------------------

	//The buffer start address, constant;
	void *const buffer_min;

	//The buffer's limit address, not allocated, constant;
	void *const buffer_max;


	//------------------------- Access channels -------------------------

	//Input channel; Stored by pointer for deletion;
	iochannel_t *input_channel;

	//Output channel; Stored by pointer for deletion;
	iochannel_t *output_channel;


} iobuffer_t;


//----------------------------------------------------- Init - exit ----------------------------------------------------

//Initialise the buffer with direct access on both sides;
void iobuffer_create(size_t size, iochannel_t *input_channel, iochannel_t *output_channel);


//Register a channel to a buffer;
void iobuffer_channel_register_buffer(iochannel_t *channel, iobuffer_t *iobuffer);


//Deletes the channel, eventually deletes the buffer and resets the buffer's other channel;
void iobuffer_channel_delete(iochannel_t *channel);


//------------------------------------------------ Transfer persistence ------------------------------------------------

//Notify the IOBuffer that one of its channel has completed a transfer, whose size is sored in the channel pointer;
void persist_transfer(iochannel_t *channel);



#endif //TRACER_rwbuffer_H
