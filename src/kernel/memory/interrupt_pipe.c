//
// Created by root on 7/27/18.
//

#include <kernel/kernel.h>
#include <string.h>
#include "interrupt_pipe.h"

//--------------------------------------------------- Private Headers --------------------------------------------------

//Accepts the provided client; Used for implementations to update transfer data according to the client's type;
void interrupt_pipe_accept_client(struct interrupt_pipe_t *pipe);

//Prepares the transfer, and enables the trigger;
void interrupt_pipe_prepare_transfer(struct interrupt_pipe_t *pipe);


//Attempts to close the pipe. If not stoppable, returns false;
bool interrupt_pipe_close(struct interrupt_pipe_t *pipe);


//Delete the pipe's dynamic data; Called by owner stream only;
void interrupt_pipe_destructor(struct interrupt_pipe_t *pipe);

//Transfer data between two DMA compatible zones;
void interrupt_pipe_fast_transfer(struct interrupt_pipe_t *pipe, struct stream_t *src, struct stream_t *dst,
								  size_t nb_blocks);

//Transfer data between two zones that are not DMA compatible;
void interrupt_pipe_slow_transfer(struct interrupt_pipe_t *pipe, struct stream_t *src, struct stream_t *dst,
								  size_t nb_blocs);

//--------------------------------------------------- Private Headers --------------------------------------------------

//Initialise an interrupt pipe, providing the owner and the
void interrupt_pipe_initialise(struct interrupt_pipe_t *pipe,
							   struct stream_t *owner,
							   void (*update_watermark)(struct interrupt_pipe_t *, size_t),
							   void (*enable_interupt)(struct interrupt_pipe_t *),
							   void (*disable_interupt)(struct interrupt_pipe_t *),
							   void (*destructor)(struct interrupt_pipe_t *)) {

	//If null functions were provided for interrupt management :
	if ((!enable_interupt) || (!disable_interupt) || (!update_watermark)) {

		//Error, interrupt management function are mandatory;
		kernel_error("interrupt_pipe.c : interrupt_pipe_initialise : interrupt management functions can't be null;");

	}

	struct interrupt_pipe_t init = {

		//Initialise all stream data;
		.pipe = {

			//Closed at init;
			.state = STREAM_PIPE_CLOSE,

			//Provide the owner's reference;
			.owner_stream = owner,

			//Not registered;
			.client_stream = 0,

			//Size not up to date for instance;
			.transfer_size = 0,

			//Provide interrupt pipe functions;
			.open = (void (*)(struct stream_pipe_t *)) interrupt_pipe_accept_client,
			.update_watermark = (void (*)(struct stream_pipe_t *, size_t)) update_watermark,
			.prepare_transfer = (void (*)(struct stream_pipe_t *)) interrupt_pipe_prepare_transfer,
			.enable_trigger = (void (*)(struct stream_pipe_t *)) enable_interupt,
			.disable_trigger = (void (*)(struct stream_pipe_t *)) disable_interupt,
			.close = (bool (*)(struct stream_pipe_t *)) interrupt_pipe_close,
			.destructor = (void (*)(struct stream_pipe_t *)) interrupt_pipe_destructor,

		},

		//Transfer inactive at init;
		.transfer_active = false,

		//Save the destructor;
		.destructor = destructor,

	};

	//Initialise the pipe;
	memcpy(pipe, &init, sizeof(struct interrupt_pipe_t));

}


/*
 * interrupt_pipe_transfer : executes the pipe transfer in the correct order, if the pipe is started;
 */

void interrupt_pipe_transfer(struct interrupt_pipe_t *pipe) {

	//Mark the transfer active;
	pipe->transfer_active = true;

	//If the pipe is not started :
	if (pipe->pipe.state != STREAM_PIPE_STARTED) {

		//Mark the transfer inactive;
		pipe->transfer_active = false;

		//Error;
		kernel_error("interrupt_pipe.c : interrupt_pipe_transfer : attempted to transfer on a non started pipe;");

		//Do nothing;
		return;

	}

	//Cache the owner;
	struct stream_t *const owner = pipe->pipe.owner_stream;

	//Declare references to src and dst streams;
	struct stream_t *src, *dst;

	//If the owner is an input stream (dst);
	if (owner->input_stream) {

		//Initialise dst as the owner and src as the client;
		dst = owner, src = pipe->pipe.client_stream;

	} else {

		//If not, do the opposite;
		src = owner, dst = pipe->pipe.client_stream;

	}

	//Create the stream's block descriptor;
	const size_t src_nb = (*(src->get_available_blocks))(src);
	const size_t dst_nb = (*(dst->get_available_blocks))(dst);

	//Cache the minimal number of blocks;
	size_t transfer_size = (src_nb < dst_nb) ? src_nb : dst_nb;

	//If no blocks can be transferred :
	if (!transfer_size) {

		//Stop here;
		return;//TODO ERROR ?

	}

	//Execute the transfer and cache the transfer size;
	(*(pipe->transfer_function))(pipe, src, dst, transfer_size);

	//Confirm the transfer;TODO IMPROVEMENT : CONFIRM THE TRANSFER WHEN ALL SPACE HAS BEEN USED; MEMORY MAPS AND BLOC DESCCRIPTORS WILL BE QUERIED ONLY ONCE;
	stream_pipe_confirm_transfer((struct stream_pipe_t *) pipe, transfer_size);

	//Mark the transfer inactive;
	pipe->transfer_active = false;

}


//------------------------------------------------- Pipe Implementation ------------------------------------------------

/*
 * interrupt_pipe_accept_client : update the transfer function according to both streams types;
 */

void interrupt_pipe_accept_client(struct interrupt_pipe_t *pipe) {

	//Declare the function pointer;
	interrupt_pipe_transfert_t transfer_function;

	//If both zones are DMA compatible :
	if ((pipe->pipe.owner_stream->type == STREAM_DMA_COMPLIANT_MEMORY) &&
		(pipe->pipe.client_stream->type == STREAM_DMA_COMPLIANT_MEMORY)) {

		//Select the DMA like transfer function;
		transfer_function = &interrupt_pipe_fast_transfer;

		//Cache src's block size (equal to dst, guaranteed by pipe_t;
		size_t block_size = pipe->pipe.owner_stream->block_size;

		//Declare the pointer to the mmap transfer function;
		void (*mmap_f)(struct mem_map_t, struct mem_map_t, size_t, size_t);

		//If the block size is pair :
		if (block_size & 1) {

			//Transfer will be made in a byte base;
			mmap_f = &mmap_transfer_8;

		} else if (block_size & 3) {
			//If the block size is multiple of 2

			//Transfer will be made in a 2-byte base;
			mmap_f = &mmap_transfer_16;

		} else {
			//If the block size is multiple of 4

			//Transfer will be made in a 4-byte base;
			mmap_f = &mmap_transfer_32;
		}

		//Save the mmap function;
		pipe->mmap_transfer = mmap_f;


	} else {

		//Use both transfer implementations for transfer;
		transfer_function = &interrupt_pipe_slow_transfer;

	}

	//Update the transfer function;
	pipe->transfer_function = transfer_function;

}




/*
 * interrupt_pipe_prepare_transfer : Nothing to do, trigger will be enabled by the pipe general library,
 * 	size is already checked;
 */

void interrupt_pipe_prepare_transfer(struct interrupt_pipe_t *pipe) {

	//TODO IMPROVEMENT : CACHE MEMORY MAPS AND BLOC DESCRIPTOR, DETERMINE THE NUMBER OF TRANSFER TO DO UNTIL COMPLETION,
	//TODO

}


/*
 * interrupt_pipe_close : asserts if the transfer is inactive;
 */

bool interrupt_pipe_close(struct interrupt_pipe_t *pipe) {

	//If the transfer is currently active :
	return !(pipe->transfer_active);

}


//Delete the pipe's dynamic data; Called by owner stream only;
void interrupt_pipe_destructor(struct interrupt_pipe_t *pipe) {

	//No dynamic data owned, just call the implementation's destructor;
	(*(pipe->destructor))(pipe);

}


//------------------------------------------------- Transfer functions -------------------------------------------------

/*
 * interrupt_pipe_fast_transfer : this function is called only when both streams are DMA compatible;
 *
 * 	It determines the direction and then executes a direct bloc copy, with the optimal function;
 */

void interrupt_pipe_fast_transfer(struct interrupt_pipe_t *const pipe, struct stream_t *const src,
								  struct stream_t *const dst, const size_t nb_blocks) {


	//We will use the two memory maps, legit because both zones are DMA compatible;
	struct mem_map_t src_map, dst_map;

	//Initialise both maps;
	src->get_memory_map(src, &src_map);
	dst->get_memory_map(dst, &dst_map);

	//Transfer blocks, and return the size of the copy;
	(*(pipe->mmap_transfer))(src_map, dst_map, src->block_size, nb_blocks);

}


void interrupt_pipe_slow_transfer(struct interrupt_pipe_t *const pipe, struct stream_t *const src,
								  struct stream_t *const dst, const size_t nb_blocs) {

	//Cache the common blocks size;
	size_t blocks_size = src->block_size;

	//Create an array that will contain temporary data. Covers two blocs more, to support alignment adjustment;
	uint8_t array[(nb_blocs + (size_t) 2) * blocks_size];

	//Go to the first bloc, take the closest aligned pointer;
	void *ptr = (void *) ((size_t) (array + blocks_size) & ~(size_t) 3);

	//Declare the array's memory map;
	const struct mem_map_t map = {

		//Start at closest aligned address;
		.start_address = ptr,

		//Contiguous memory;
		.block_spacing = blocks_size,

	};

	//Create the transfer's memory descriptor;
	const struct blocks_desc_t transfer_descriptor = {
		.nb_blocks = nb_blocs,
		.block_size = blocks_size,
	};

	//Copy data from src to the array;
	(*(src->transfer_blocks))(src, &map, &transfer_descriptor);

	//Copy data from the array to dst;
	(*(dst->transfer_blocks))(dst, &map, &transfer_descriptor);

}


