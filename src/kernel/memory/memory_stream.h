//
// Created by root on 7/17/18.
//

#ifndef TRACER_MEMORY_TRANSFER
#define TRACER_MEMORY_TRANSFER

#include <stdbool.h>

#include <stdint.h>

#include <stddef.h>
#include "memory_transfer.h"


//--------------------------------------------------- Stream trigger ---------------------------------------------------

/*
 * A stream trigger has one of the following types :
 */

enum stream_trigger_type_t {

	//The stream doesn't support a trigger;
		STREAM_NO_TRIGGER,

	//The watermark reach will call a transfer on its related memory stream;
		STREAM_TRANSFER_TRIGGER,

	//The watermark reach will trigger a DMA transfer;
		STREAM_DMA_TRANSFER

};


/*
 * A stream trigger represents a way to trigger the stream's transfer;
 */

struct stream_trigger_t {

	//The type of the watermark;
	enum stream_trigger_type_t type;

	//The function to set the triggering size; Asserts if the triggering size is valid;
	bool (*const set_watermark)(struct async_stream_t *, size_t triggering_size);

	//The function to enable or disable the watermark trigger;
	void (*const enable_trigger)(struct async_stream_t *, bool);

	//Fields that depend of the trigger type;
	union {



	};

	//------------------ DMA specific fields;

	//The eventual DMA channel that the watermark will trigger;
	uint8_t dma_channel;

	//The memory map for the DMA transfer;


};



//---------------------------------------------------- Stream memory ---------------------------------------------------

/*
 * Depending on the type of memory that the stream interfaces with, different type of operations can be done;
 * 	The memory state described authorised manipulations;
 */

enum stream_memory_type_t {

	//The stream's memory can be accessed and manipulated with DMA and standard memory transfers;
	STREAM_DMA_COMPATIBLE_MEMORY,

	//Programmatic memory transfer can't be done with standard memory transfer functions, but a DMA operation is
	// supported by the stream memory (case for some peripherals where reading a special register clears int flags);
	STREAM_DMA_COMPLIANT_MEMORY,

	//Programmatic memory transfer can't be done with standard memory transfer functions, and DMA access is not
	// supported;
	STREAM_NON_STANDARD_MEMORY,

};

//Predeclare the stream transfer structure;
struct stream_auto_transfer_t;


/*
 * The stream memory structure represents a virtual memory zone
 *
 */

struct stream_memory_t {

	//---------------- Fields ----------------

	//The type of memory;
	const enum stream_memory_type_t type;

	//The stream direction; Data can be written on an input stream, and read from an output stream;
	const bool input_stream;

	//The stream's trigger;
	const struct stream_trigger_t trigger;

	//The stream's transfer; null if no transfer registerd;
	struct stream_auto_transfer_t *transfer;


	//---------------- Stream lifecycle ----------------

	//Get a bloc descriptor representing the current memory state;
	void (*const get_bloc_descriptor)(struct stream_memory_t *stream, struct blocks_desc_t *desc);

	//Transfer data between with another memory zone, given the provided descriptor;
	size_t (*const transfer_blocs)(struct stream_memory_t *stream, const struct mem_map_t *mem,
									const struct blocks_desc_t *descriptor);

	//Update the memory after blocs have transferred
	void (*const discard_blocs)(struct stream_memory_t * memory, size_t nb_blocs);


	//---------------- Deletion ----------------

	//Call the owner of the stream memory for deletion;
	void (*const destructor)(struct stream_memory_t *memory);

};

//----------------------------------------------------- Init - Exit ----------------------------------------------------

//No initializer is required, streams implementations will do it by themselves;

//Stream destruction must be managed carefully, as transfers may be registered;
void stream_delete(struct stream_memory_t *stream_memory);


//---------------------------------------------------- Transfer ----------------------------------------------------

//Transfer blocs between the stream and another memory zone;
size_t stream_transfer(struct stream_memory_t *stream, struct mem_desc_t *dst);

//Read blocs from the stream memory; Returns the size of the read zone, zero if bad direction;
size_t stream_read(struct stream_memory_t *stream, struct mem_desc_t *dst);

//Copy blocs to the stream memory; Returns the size of the copied zone, zero if bad direction
size_t stream_write(struct stream_memory_t *stream, struct mem_desc_t *src);


//--------------------------------------------------- Stream automatic transfer --------------------------------------------------

/*
 * To transfer data automatically from one stream memory zone to another, the stream transfer structure is used;
 *
 * 	This struct is owned by its couple of memory streams, as soon as one is deleted, it is deleted too;
 */

struct stream_auto_transfer_t {

	//The couple of transfer streams; src is output, dst is input;
	struct stream_memory_t *src;
	struct stream_memory_t *dst;

	//The transfer's trigger; src or dst's trigger;
	struct stream_trigger_t *trigger;

	//Transfer data from src to dst; Transfer is not guaranteed to be done when the function returns;
	void (*const transfer)(struct stream_transfer_t *);

	//Stop the transfer. Will not return until the transfer is not stopped.
	// Will not re-happen until set_trigger is called;
	void (*const join)(struct stream_transfer_t *);

	//Delete the transfer;
	void (*const destructor)(struct stream_transfer_t *);

};



#endif