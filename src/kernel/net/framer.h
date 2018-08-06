//
// Created by root on 8/6/18.
//

#ifndef TRACER_FRAMER_H
#define TRACER_FRAMER_H


#include <stdbool.h>

#include <stdint.h>
#include <stddef.h>

#include <data_structures/containers/non_concurrent/llist.h>

#include <types/list.h>


//----------------------------------------------------- Data block -----------------------------------------------------

/*
 * To store messages of a variable length, we will use data blocks; They reference a memory zone, and can be linked;
 */

struct data_block_t {

	//Link;
	struct list_head_t link;

	//The block's start address;
	void *address;

	//The block's current size;
	const size_t size;

	//The block's maximal size;
	const size_t max_size;

};


//------------------------------------------------------ IRQ FIFO ------------------------------------------------------

/*
 * The IRQ fifo shares its input and its output with two levels of execution;
 */

struct irq_fifo_t {

	//The couple of lists. Elements are appended to @pending, and then transferred into @available;
	struct list_head_t *pending, *available;

	//Operation locks;
	volatile bool push_lock, pull_lock;

};

//Asserts if no elements can be pulled of @fifo or if pull is locked;
void irq_fifo_empty(struct irq_fifo_t *fifo);

//Push @element in @fifo;
void irq_fifo_push(struct irq_fifo_t *fifo, struct list_head_t *element);

//Pull and return and element from @fifo. 0 returned if no element available;
struct list_head_t *irq_fifo_pull(struct irq_fifo_t *fifo);

//Concatenate pending data to the available queue, and reset the pending queue. If push or pull is locked, return 0;
bool irq_fifo_transfer(struct irq_fifo_t *fifo);


/*
 * ------------------------------------------- OSI layer 2 network interface -------------------------------------------
 */

/*
 * A layer 2 peripheral receives delimited frames.
 */

struct netf_2 {

	//The protocol interface;
	//TODO struct protocol_t protocol;

	//The quadruplet of irq fifos to transmit frame containers between hw and sw irq
	struct irq_fifo_t *rx_empty, rx_nonempty, tx_empty, tx_nonempty;

	//Enable hardware interrupts;
	void (*enable_rx_hw_irq)(struct net_interface_t *);
	void (*enable_tx_hw_irq)(struct net_interface_t *);

	//The destructor;
	void (*destructor)(struct netf_2 *);

};

//---------------------------------------------------- IRQ functions ---------------------------------------------------

//Push @block in rx_nonempty list of @iface, transfer in both rx lists, pull-return a block from rx_empty (can be 0);
struct data_block_t *netf2_get_new_rx_block(struct netf_2 *iface, struct data_block_t *block);

//Push @block in tx_empty list of @iface, transfer in both tx lists, pull-return a block from tx_nonempty (can be 0);
struct data_block_t *netf2_get_new_tx_block(struct netf_2 *iface, struct data_block_t *block);


//------------------------------------------------------- Polling ------------------------------------------------------

//Asserts if @iface has an available message in its rx_nonempty queue;
bool netf2_message_available(struct netf_2 *iface);

//Pulls a message, and if not null, copies its content in @block; Asserts if success;
bool netf2_get_message(struct netf_2 *iface, struct data_block_t *block);

//Attempts to pull an empty block, copies @block into it, and asserts if success;
bool netf2_send_message(struct netf_2 *iface, const struct data_block_t *block);


/*
 * ------------------------------------------- OSI layer 1 network interface -------------------------------------------
 */

/*
 * A framer turns byte stream into framed messages, and framed messages into byte streams;
 *
 * 	Messages are stored in blocks, any attempt to use read or write when related blocks are not initialised will
 * 	throw an error;
 */

struct framer_t {

	//The block where we save input data;
	struct data_block_t *input_block;

	//The block where we take output data;
	struct data_block_t *output_block;


	//Write a byte in the framer. Will assert if the message is complete;
	bool (*write_byte)(struct framer_t *, uint8_t data);

	//Read (and discard) a byte from the current message of the framer. Will assert if the message is complete;
	bool (*read_byte)(struct framer_t *, uint8_t *data);

};


/*
 * A layer 1 peripheral required a framer to comply with the netf2 requirement;
 */

struct netf_21 {

	//The layer 2 interface we adapt;
	struct netf_2 iface;

	//The framer we use;
	struct framer_t *framer;

};


//---------------------------------------------------- IRQ functions ---------------------------------------------------

//Receive a decoded byte; Asserts if more bytes can be written. If not, the procedure must stop;
bool netf_21_write_byte(struct netf_21 *iface, uint8_t data);

//Get an encoded byte. Asserts if more bytes can be read. If not, the procedure must stop;
bool netf_21_read_byte(struct framer_t *, uint8_t *data);

#endif //TRACER_FRAMER_H
