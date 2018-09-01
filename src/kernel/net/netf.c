/*
  netf.c Part of TRACER

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

#include "netf.h"

#include <string.h>

#include <kernel/syscall.h>




//----------------------------------------------------- Data block -----------------------------------------------------


//Create a data block;TODO
struct data_block *data_block_create(size_t size) {

	//Create the rx block initializer;
	struct data_block tx_block_init = {

		//Head will be initialised after allocation;
		.head = {},

		//Allocate some memory for the block;
		.address = kernel_malloc(size),

		//Block is empty for instance;
		.size = 0,

		//Save the block size;
		.max_size = size,
	};

	//Allocate the block;
	struct data_block *block = kernel_malloc_copy(sizeof(struct data_block), &tx_block_init);

	//Initialise the block list;
	list_init((struct list_head *)block);

	//Return the initialised block;
	return block;

}


//Delete a data block;TODO
void data_block_delete(struct data_block *block) {

	//Free the content of the block,
	kernel_free(block->address);

	//Free the block itself;
	kernel_free(block);

}



/**
 * data_block_copy : copies the content of src into dst. Error if dst is not big enough;
 *
 * @param src : the block to copy;
 * @param dst : the block where to copy;
 */

void data_block_copy(const struct data_block *const src, struct data_block *const dst) {

	//Cache the size of src's content;
	size_t src_size = src->size;

	//If dst can't contain the content of src :
	if (dst->max_size < src_size) {

		//Error;
		kernel_error("netf.c : data_block_copy : dst is not big enough to contain src's content;");

	}

	//Copy the content of src to dst;
	memcpy(dst->address, src->address, src_size);

	//Update the size of dst's content;
	dst->size = src_size;

}


//----------------------------------------------------- Init - Exit ----------------------------------------------------

//Initalise a layer 2 interface : create and fill fifos, assign function pointers;
void netf2_init(
	struct netf2 *const iface,
	size_t nb_frames,
	const size_t frame_size,
	void (*const enable_rx_hw_irq)(struct netf2 *),
	void (*const enable_tx_hw_irq)(struct netf2 *),
	void (*const destructor)(struct netf2 *)
) {

	//Create the fifo initializer;
	struct shared_fifo fifo_init = {
		.list = 0,
	};

	//Create the ifaces initializer;
	struct netf2 iface_init = {

		//Allocate and initialise all lists;
		.rx_empty = kernel_malloc_copy(sizeof(struct shared_fifo), &fifo_init),
		.rx_nonempty = kernel_malloc_copy(sizeof(struct shared_fifo), &fifo_init),
		.tx_empty = kernel_malloc_copy(sizeof(struct shared_fifo), &fifo_init),
		.tx_nonempty = kernel_malloc_copy(sizeof(struct shared_fifo), &fifo_init),

		//Assign function pointers;
		.enable_rx_hw_irq = enable_rx_hw_irq,
		.enable_tx_hw_irq = enable_tx_hw_irq,
		.destructor = destructor,

	};

	/*
	 * Create data blocks;
	 */

	//For each block to create :
	while (nb_frames--) {

		//Create a block;
		struct data_block *block = data_block_create(frame_size);

		//Push the block in the rx_empty list;
		shared_fifo_push(iface_init.rx_empty, (struct list_head *) block);

		//Create another block;
		block = data_block_create(frame_size);

		//Push the block in the tx_empty list;
		shared_fifo_push(iface_init.tx_empty, (struct list_head *) block);

	}

	//Initialise the interface;
	memcpy(iface, &iface_init, sizeof(struct netf2));


}


//Destruct the interface : delete fifos and their content;TODO
void netf2_delete(struct netf2 *iface) {

	//Call the implementation's deleter;
	(*(iface->destructor))(iface);

	//Create a data block pointer;
	struct data_block *block;

	//Create a macro that will delete all block from the fifo and delete the fifo;
#define CLEAR_FIFO(fifo) {while((block = (struct data_block *)shared_fifo_pull((fifo)))) {data_block_delete(block);} kernel_free(fifo);}

	//Free all list and their content;
	CLEAR_FIFO(iface->rx_empty);
	CLEAR_FIFO(iface->tx_empty);
	CLEAR_FIFO(iface->rx_nonempty);
	CLEAR_FIFO(iface->tx_nonempty);

	//Undef the macro;
#undef CLEAR_FIFO

	//Free the interface;
	kernel_free(iface);

}
int suussuus;

//---------------------------------------------------- IRQ functions ---------------------------------------------------

/**
 * netf2_get_new_rx_block : Pushes @block in rx_nonempty list of @iface. Pulls and return a block from
 * rx_empty (can be 0);
 *
 * @param iface : the interface where to push and pull rx blocks;
 * @param block : the block containing the received frame;
 * @return a new block to store the next frame, or 0 if there is none;
 */

struct data_block *netf2_get_new_rx_block(struct netf2 *iface, struct data_block *block) {

	//Push @block in rx_nonempty;
	shared_fifo_push(iface->rx_nonempty, (struct list_head *) block);

	//Pull a block from rx_empty and return it;
	return (struct data_block *) shared_fifo_pull(iface->rx_empty);

}


/**
 * netf2_get_new_tx_block : Pushes @block in tx_empty list of @iface. Pulls and return a block from
 * tx_nonempty (can be 0);
 *
 * @param iface : the interface where to push and pull tx blocks;
 * @param block : the block containing a transmitted frame;
 * @return a new block containing a frame to transmit, or 0 if there is none;
 */

struct data_block *netf2_get_new_tx_block(struct netf2 *iface, struct data_block *block) {

	//Push @block in tx_empty;
	shared_fifo_push(iface->tx_empty, (struct list_head *) block);

	//Pull a block from tx_nonempty and return it;
	return (struct data_block *) shared_fifo_pull(iface->tx_nonempty);

}


//------------------------------------------------------- Polling ------------------------------------------------------

/**
 * netf2_get_message : attempts to get a received frame, and if not null, copies its content in @block;
 * 	Asserts if success;
 *
 * @param iface : the interface to pull from;
 * @param frame : the block where to copy the frame;
 * @return true if the frame was successfully copies, false if not;
 */

bool netf2_get_frame(struct netf2 *iface, struct data_block *frame) {

	//Pull a frame from rx_nonempty;
	const struct data_block *const ne_frame = (struct data_block *) shared_fifo_pull(iface->rx_nonempty);

	//If no frame was available :
	if (!ne_frame) {

		//Abort here;
		return false;

	}

	//Copy the content of @ne_frame in @frame;
	data_block_copy(ne_frame, frame);

	//Push ne_frame in rx_empty;
	shared_fifo_push(iface->rx_empty, (struct list_head *) ne_frame);

	//Enable the rx interrupt;
	(*(iface->enable_tx_hw_irq))(iface);

	//Complete;
	return true;

}


/**
 * netf2_send_message : attempts to get a frame container, copies @frame into it, and asserts if success;

 * @param iface : the interface that will send the frame;
 * @param frame : the frame to send;
 * @return true if the frame was successfully copied, false if not;
 */

bool netf2_send_frame(struct netf2 *iface, const struct data_block *const frame) {

	//Pull a frame from tx_empty;
	struct data_block *e_frame = (struct data_block *) shared_fifo_pull(iface->tx_empty);

	//If no frame was available :
	if (!e_frame) {

		//Abort here;
		return false;

	}

	//Copy the content of @frame in @e_frame;
	data_block_copy(frame, e_frame);

	//Push ne_frame in tx_nonempty;
	shared_fifo_push(iface->tx_nonempty, (struct list_head *) e_frame);

	//Enable the interrupt;
	(*(iface->enable_tx_hw_irq))(iface);

	//Complete;
	return true;

}



//--------------------------------------------- OSI layer 1 TODO NOOOOOOP functions ---------------------------------------------

//TODO REFACTOR AND CLARIFY LAYER 2


//----------------------------------------------------- Destructor -----------------------------------------------------

//Destructor. Deletes the framer; TODO
void netf21_destruct(struct netf21 *iface) {

	//Cache the framer;
	struct data_framer *framer = iface->framer;

	//Call the framer deleter;
	(*(framer->deleter))(framer);

}


/**
 * netf21_init_decoding : verifies that the framer contains a non null decoding block.
 * 	If not, attempts to get one, and if it fails, returns false.
 * 	If a non null block is present or provided, returns true;
 *
 * @param iface : the interface to init;
 * @return true if the decoding function can be called;
 */

bool netf21_init_decoding(struct netf21 *iface) {

	//Cache the framer;
	struct data_framer *framer = iface->framer;

	//If the decoding block exists, complete;
	if (framer->decoding_block) {
		return true;
	}

	//If null decoding block, attempt to get one from rx_empty;
	struct data_block *block = (struct data_block *) shared_fifo_pull(iface->iface.rx_empty);

	//If the block is null, fail;
	if (!block) {
		return false;
	}

	//If non null block, save it;
	framer->decoding_block = block;

	//Complete;
	return true;

}


/**
 * netf21_init_encoding : verifies that the framer contains a non null encoding block.
 * 	If not, attempts to get one, and if it fails, returns false.
 * 	If a non null block is present or provided, returns true;
 *
 * @param iface : the interface to init;
 * @return true if the encoding function can be called;
 */

bool netf21_init_encoding(struct netf21 *iface) {

	//Cache the framer;
	struct data_framer *framer = iface->framer;

	//If the encoding block exists, complete;
	if (framer->encoding_block) {
		return true;
	}

	//If null encoding block, attempt to get one from tx_nonempty;
	struct data_block *block = (struct data_block *) shared_fifo_pull(iface->iface.tx_nonempty);

	//If the block is null, fail;
	if (!block) {
		return false;
	}

	//If non null block, save it;
	framer->encoding_block = block;

	//Complete;
	return true;

}


//---------------------------------------------- OSI layer 1 IRQ functions ---------------------------------------------

/**
 * netf_21_transmit_byte : transmits a byte to @iface for decoding; Asserts if more bytes can be written.
 * If not, the transmission must stop;
 *
 * @param iface : the interface that must receive the byte;
 * @param data : the byte to decode;
 * @return true if there is space for data to be received;
 */

bool netf_21_decode_byte(struct netf21 *iface, uint8_t data) {

	//Cache the framer;
	struct data_framer *framer = iface->framer;

	//If the framer has no block to receive data :
	if (!framer->decoding_block) {

		/*
		 * This should not have happened, and witnesses a flaw in the program that uses the interface.
		 */

		//Return false, to disable future transmission; byte will be lost;
		return false;

	}


	//Transmit the byte to the framer;
	bool frame_complete = (*(framer->decode))(framer, data);

	//If the frame is complete :
	if (frame_complete) {

		//Send the block in the net2 for storage and get another; Cache it and assign it;
		struct data_block *new_block =
			framer->decoding_block = netf2_get_new_rx_block(&iface->iface, framer->decoding_block);

		//Assert if the provided block exists;
		return new_block != 0;

	}

	//If the frame is not complete, assert to enable further readings;
	return true;

}


/**
 * netf_21_receive_byte : receive an encoded byte from @iface. Asserts if more bytes can be read.
 * If not, the procedure must stop;
 *
 * @param iface : the interface that must provide the byte;
 * @param data
 * @return true if there are more bytes to receive.
 */

bool netf_21_get_encoded_byte(struct netf21 *iface, uint8_t *data) {

	//Cache the framer;
	struct data_framer *framer = iface->framer;

	//If the framer has no block to receive data :
	if (!framer->encoding_block) {

		/*
		 * This should not have happened, and witnesses a flaw in the program that uses the interface.
		 */

		//Return false, to disable future transmission; byte will be lost;
		return false;

	}

	//Transmit the byte to the framer;
	bool frame_complete = (*(framer->encode))(framer, data);

	//If the frame is complete :
	if (frame_complete) {

		//Send the block in the net2 for storage and get another; Cache it and assign it;
		struct data_block *new_block = netf2_get_new_tx_block(&iface->iface, framer->encoding_block);

		//Update the new block;
		framer->encoding_block = new_block;

		//Assert if the provided block exists;
		return new_block != 0;

	}

	//If the frame is not complete, assert to enable further readings;
	return true;


}
