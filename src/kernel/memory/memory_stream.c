//
// Created by root on 7/17/18.
//

#include "memory_stream.h"

#include <string.h>
#include <kernel/arch/arch.h>


inline void stream_discard(struct stream_memory_t *const stream, const size_t nb_transferred_blocs) {

	//If the transfer has happened and the function is not null : 
	if (nb_transferred_blocs && stream->discard_blocs) {

		//Call the function;
		(*(stream->discard_blocs))(stream, nb_transferred_blocs);

	}

}


//---------------------------------------------------- Read - write ----------------------------------------------------

/*
 * transfer_blocs : setups the transfer and calls the stream transfer function;
 */

size_t stream_transfer(struct stream_memory_t *const stream, struct mem_desc_t *const mem) {

	//Create the stream's block descriptor;
	struct blocks_desc_t stream_descriptor;

	//Get the stream's bloc descriptor;
	(*(stream->get_bloc_descriptor))(stream, &stream_descriptor);

	//Create the transfer's block descriptor;
	struct blocks_desc_t descriptor;

	//Merge the couple of bloc descriptors; Will assert if a transfer can happen;
	bool transferable = block_desc_merge(&stream_descriptor, &mem->bloc_desc, &descriptor);

	//If no transfer can happen, nothing to do;
	if (!transferable) {
		return 0;
	}

	//Transfer blocs, and return the size of the copy;
	(*(stream->transfer_blocs))(stream, &mem->memory_map, &descriptor);

	//Discard some blocks;
	stream_discard(stream, descriptor.nb_blocks);

	//Return the number of read blocks;
	return descriptor.nb_blocks;

}


/*
 * stream_read : executes the transfer if the stream is output;
 */

size_t stream_read(struct stream_memory_t *const stream, struct mem_desc_t *const dst) {

	//If the stream is an input stream :
	if (stream->input_stream) {

		//Error, wrong direction. Do nothing;
		return 0;

	}

	//Execute the transfer;
	return stream_transfer(stream, dst);
	
}


/*
 * stream_write : executes the transfer if the stream is input;
 */

size_t stream_write(struct stream_memory_t *const stream, struct mem_desc_t *const src) {

	//If the stream is an output stream :
	if (!stream->input_stream) {

		//Error, wrong direction. Do nothing;
		return 0;


	}

	//Execute the transfer;
	return stream_transfer(stream, src);

}
