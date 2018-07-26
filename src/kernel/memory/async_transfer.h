//
// Created by root on 7/26/18.
//

#ifndef TRACER_ASYNC_TRANSFER_H
#define TRACER_ASYNC_TRANSFER_H


#include "memory_stream.h"

//Predeclare struct stream_transfer_t
struct stream_transfer_t;


/*
 * A stream trigger has one of the following types :
 */
enum stream_trigger_type {

	//The watermark reach will call a transfer on its related memory stream;
	STREAM_TRANSFER_TRIGGER,

	//The watermark reach will trigger a DMA transfer;
	STREAM_DMA_TRANSFER

};


struct stream_trigger {

	//The type of the watermark;
	enum stream_trigger_type type;

	//The function to set the triggering size; Asserts if the triggering size is valid;
	bool (*const set_watermark)(struct async_stream_t *, size_t triggering_size);

	//The function to enable or disable the watermark trigger;
	void (*const enable_trigger)(struct async_stream_t *, bool);


	//------------------ DMA specific fields;

	//The eventual DMA channel that the watermark will trigger;
	uint8_t dma_channel;

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
