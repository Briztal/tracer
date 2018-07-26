//
// Created by root on 7/26/18.
//

#ifndef TRACER_ASYNC_TRANSFER_H
#define TRACER_ASYNC_TRANSFER_H


#include "memory_stream.h"

//Predeclare struct stream_transfer_t
struct stream_transfer_t;


//
enum watermark_type_t {

	//The watermark reach will call a transfer on its related memory stream;
	TRANSFER_CALL,

	//The watermark reach will trigger a DMA transfer;
	DMA_TRANSFER

};


struct stream_watermark_t {

	//The type of the watermark;
	enum watermark_type_t type;

	//The function to set the triggering size; Asserts if the triggering size is valid;
	bool (*const set_watermark)(struct async_stream_t *, size_t triggering_size);

	//The function to enable or disable the watermark trigger;
	void (*const enable_trigger)(struct async_stream_t *, bool);


	//------------------ DMA specific fields;

	//The eventual DMA channel that the watermark will trigger;
	uint8_t dma_channel;

};


/*
 * The trigger stream is a stream that references an async transfer, that can be automatically triggered when a
 * 	determined amount of data is available for transfer;
 */

struct watermark_stream_t {

	//The stream base;
	struct stream_memory_t stream;

	//The registered stream_transfer;
	struct stream_transfer_t *transfer;

};


/*
 * Sometimes it is appropriate to transfer data on an asynchronous base, when interacting with a
 */

struct stream_transfer_t {

	//The couple of transfer streams;
	struct trigger_stream_t *src;
	struct trigger_stream_t *dst;

	//Stop the transfer. Will not return until the transfer is not stopped.
	// Will not re-happen until set_trigger is called;
	void (*const join)(struct stream_transfer_t *);

	//Delete the transfer;
	void (*const destructor)(struct stream_transfer_t *);

};

#endif //TRACER_ASYNC_TRANSFER_H
