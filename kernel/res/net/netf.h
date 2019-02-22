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

#ifndef TRACER_NETF_H
#define TRACER_NETF_H


#include <stdbool.h>

#include <stdint.h>

#include <stddef.h>

#include <data_structures/containers/concurrent/shared_fifo.h>
#include <kernel/res/net/framer/framer.h>



/*----------------------------------------------------- Data block -----------------------------------------------------*/

/*
 * To store messages of a variable length, we will use data blocks; They reference a memory zone, and can be linked;
 */

struct data_block {

	/*Link, mutable;*/
	struct list_head head;

	/*The block's start address, constant, references a precise location;*/
	void *const address;

	/*The block's current size, mutable*/
	size_t size;

	/*The block's maximal size, constant;*/
	const size_t max_size;

};

/*Create a data block;*/
struct data_block *data_block_create(size_t size);

/*Delete a data block;*/
void data_block_delete(struct data_block *block);

/*Copy the content of @src into @dst. Error if dst is not big enough;*/
void data_block_copy(const struct data_block *src, struct data_block *dst);

/*
 * ------------------------------------------- OSI layer 2 network if -------------------------------------------
 */

/*
 * A layer 2 peripheral receives delimited frames.
 */

struct netf2 {

	/*The protocol if;*/
	/*TODO struct protocol_t protocol;*/

	/*The quadruplet of shared fifos, to transmit frame containers between hw_specs and sw irq*/
	struct shared_fifo *const rx_empty, *const rx_nonempty, *const tx_empty, *const tx_nonempty;

	/*Enable hardware interrupts;*/
	void (*const enable_rx_hw_irq)(struct netf2 *);
	void (*const enable_tx_hw_irq)(struct netf2 *);

	/*The implementation deleter;*/
	void (*const destructor)(struct netf2 *);

};

/*----------------------------------------------------- Init - Exit ----------------------------------------------------*/

/*Initalise a layer 2 if : create and fill fifos, assign function pointers;*/
void netf2_init(
	struct netf2 *iface,
	size_t nb_frames,
	size_t frame_size,
	void (*enable_rx_hw_irq)(struct netf2 *),
	void (*enable_tx_hw_irq)(struct netf2 *),
	void (*destructor)(struct netf2 *)
);


/*Destruct the if : delete fifos and their content;*/
void netf2_delete(struct netf2 *iface);


/*---------------------------------------------------- IRQ functions ---------------------------------------------------*/

/*Push @block in rx_nonempty list of @interfaces, transfer in both rx lists, pull-return a block from rx_empty (can be 0);*/
struct data_block *netf2_get_new_rx_block(struct netf2 *iface, struct data_block *block);

/*Push @block in tx_empty list of @interfaces, transfer in both tx lists, pull-return a block from tx_nonempty (can be 0);*/
struct data_block *netf2_get_new_tx_block(struct netf2 *iface, struct data_block *block);


/*------------------------------------------------------- Polling ------------------------------------------------------*/

/*Pulls a message, and if not null, copies its content in @block; Asserts if success;*/
bool netf2_get_frame(struct netf2 *iface, struct data_block *frame);

/*Attempts to pull an empty block, copies @block into it, and asserts if success;*/
bool netf2_send_frame(struct netf2 *iface, const struct data_block *block);


/**
 * netf2_message_available : asserts if messages can be polled from @iface;
 *
 * @param iface : the interface to examine;
 */

static inline bool netf2_message_available(struct netf2 *iface) {

	/*Assert if rx_nonempty contains messages;*/
	return !shared_fifo_empty(iface->rx_nonempty);

}

/*
 * ------------------------------------------- OSI layer 1 TODO NOP, LAYER 2 BASIC network if -------------------------------------------
 */

/*
 * A layer 1 peripheral required a framer to comply with the netf2 requirement;
 */

struct netf21 {

	/*The layer 2 if we adapt;*/
	struct netf2 iface;

	/*The framer we use;*/
	struct data_framer *framer;

};


/*----------------------------------------------------- Destructor -----------------------------------------------------*/

/*Destructor. Deletes the framer;*/
void netf21_destruct(struct netf21 *iface);


/*--------------------------------------------------- Init functions ---------------------------------------------------*/

/*Initialise the decoding structure, assert if decoding can happen;*/
bool netf21_init_decoding(struct netf21 *iface);

/*Initialise the encoding structure, assert if encoding can happen;*/
bool netf21_init_encoding(struct netf21 *iface);


/*---------------------------------------------------- IRQ functions ---------------------------------------------------*/

/*Receive a decoded byte; Asserts if more bytes can be written. If not, the procedure must stop;*/
bool netf_21_decode_byte(struct netf21 *iface, uint8_t data);

/*Get an encoded byte. Asserts if more bytes can be read. If not, the procedure must stop;*/
bool netf_21_get_encoded_byte(struct netf21 *iface, uint8_t *data);

#endif /*TRACER_NETF_H*/
