//
// Created by root on 7/17/18.
//

#ifndef TRACER_MEMORY_TRANSFER
#define TRACER_MEMORY_TRANSFER

#include <stdbool.h>

#include <stdint.h>

#include <stddef.h>
#include "memory_transfer.h"


//---------------------------------------------------- stream ---------------------------------------------------

/*
 * The stream structure represents a directed memory zone of a determined type that is accessible for read/write.
 *
 *
 * 	The stream memory is a kernel file, and so, can only be created / deleted by the kernel;
 * 	The stream memory can be accessed directly via the kernel file system, or indirectly via a stream pipe. 
 *
 *
 * 	A stream pipe automatically (via interrupts of DMA) transfers data form an output stream to an input stream;
 *  A stream pipe is owned by either its input or output stream, and share the lifetime of its owner.
	When a pipe is opened between two streams, both corresponding files will be opened, and file descriptors will be
 * 	owned by the pipe;
 * 	No stream can be closed until its registered pipe is closed, and closes the stream file;
 *
 * 	When it is opened, the stream pipe is registered in the kernel file system, allowing to close the pipe;
 *
 * 	A stream can't be deleted until its registered pipe is closed; If the user attempts to delete a stream file, with
 * 	an opened pipe, the stream file will be unregistered from the file system, and will be marked orphan;
 *
 * 	As soon as the related pipe is closed, both stream files will be closed, and deleted, because marked orphans;
 * //TODO DO NOT CLOSE THE OTHER FILE, LET THE TRANSFER CLOSE IT, AS IT CAN CALL THE KERNEL;
 * 
 */

/*
 * Depending on the type of memory that the stream interfaces with, different type of operations can be done;
 * 	The memory state described authorised manipulations;
 */

enum stream_memory_type_t {

	//The stream's memory can be accessed and manipulated with DMA and standard memory transfers;
		STREAM_DMA_COMPATIBLE_MEMORY,

	//Programmatic memory transfer can't be done with standard memory transfer functions, but a DMA operation is
	// supported by the stream (case for some peripherals where reading a special register clears int flags);
		STREAM_DMA_COMPLIANT_MEMORY,

	//Programmatic memory transfer can't be done with standard memory transfer functions, and DMA access is not
	// supported;
		STREAM_NON_STANDARD_MEMORY,

};

//Predeclare the stream transfer structure;
struct stream_pipe_t;


/*
 * The stream structure is composed of the following elements :
 */

struct stream_t {

	//---------------- Fields ----------------

	//The type of memory, constant;
	const enum stream_memory_type_t type;

	//The stream direction; Data can be written on an input stream, and read from an output stream, constant;
	const bool input_stream;

	//The transfer owned by the stream, constant;
	struct stream_pipe_t *const owned_pipe;

	//The transfer registered to the stream, mutable;
	struct stream_pipe_t *registered_pipe;


	//---------------- Stream lifecycle ----------------

	//Get a bloc descriptor representing the current memory state, constant function pointer;
	void (*const get_bloc_descriptor)(struct stream_t *stream, struct blocks_desc_t *desc);

	//Transfer data between with another memory zone, given the provided descriptor, constant function pointer;
	size_t (*const transfer_blocks)(struct stream_t *stream, const struct mem_map_t *mem,
								   const struct blocks_desc_t *descriptor);

	//Update the memory after blocks have transferred, constant function pointer;
	void (*const discard_blocks)(struct stream_t *memory, size_t nb_blocks);


	//---------------- Deletion ----------------

	//Call the owner of the stream for deletion; must also free the stream struct; constant function pointer;
	void (*const deleter)(struct stream_t *memory);

};

//----------------------------------------------------- Init - Exit ----------------------------------------------------

//No initializer is required, streams implementations will do it by themselves;

/*
 * Stream destruction must be managed carefully, as transfers may be registered;
 * 
 * 	A stream can only be deleted only when it has no pipe registered.
 * 	
 * 	If it has a pipe registered, attempting to delete it will trigger an error;
 */

void stream_delete(struct stream_t *stream);


//---------------------------------------------------- Transfer ----------------------------------------------------

//Transfer blocks between the stream and another memory zone;
size_t stream_transfer(struct stream_t *stream, struct mem_desc_t *dst);

//Read blocks from the stream; Returns the size of the read zone, zero if bad direction;
size_t stream_read(struct stream_t *stream, struct mem_desc_t *dst);

//Copy blocks to the stream; Returns the size of the copied zone, zero if bad direction
size_t stream_write(struct stream_t *stream, struct mem_desc_t *src);


//---------------------------------------------------- Stream Pipes ----------------------------------------------------


/*
 * To transfer data automatically from one stream zone to another, the stream pipe structure is used;
 *
 * A pipe is owned by a stream, and share its lifetime;
 *
 * 	Lifecycle :
 * 	
 * 	KERNEL SPACE, high priority;
 * 	
 * 		- creation, in kernel space, owner provided;
 * 		- opening, client provided; - owner and client registration, and start called;
 * 		- start : transfer is prepared and trigger is enabled if client stream has enough space/data. If not, stop;
 * 		
 * 		
 * 		- When close is called :
 * 		- attempt to abort the current transfer;
 * 		- if success, cleanup;
 * 		- if not, set close_required state, cleanup will be called;
 * 		
 * 		
 * 	INT / lower prio than kernel space;
 * 	
 * 		- transfer;
 * 		- cleanup if close_required;
 * 		- if not :
 * 		- discard on both client and owner:
 * 		- start again if streams content allow it;
 *
 * 		- when client stream is updated, start is called;
 *
 */


/*
 * The automatic stream transfer has one of the following states :
 */

enum stream_pipe_state_t {

	//Client not registered;
		STREAM_PIPE_CLOSE,

	//Client not registered, waiting for transfer end to close;
		STREAM_PIPE_CLOSE_REQUIRED,

	//Client registered, transfer stopped;
		STREAM_PIPE_OPEN,

	//Client registered, transfer started;
		STREAM_PIPE_STARTED,

};


/*
 * The stream transfer is composed of the following elements :
 */

struct stream_pipe_t {

	//The pipe's state;
	volatile enum stream_pipe_state_t state;

	//The owner stream;
	struct stream_t *const owner_stream;

	//The client stream;
	struct stream_t *client_stream;

	//Accepts the provided client; Used for implementations to update transfer data according to the client's type;
	void (*const accept_client)(struct stream_pipe_t *);

	//Prepares the transfer, and enables the trigger;
	void (*const prepare_transfer)(struct stream_pipe_t *);

	//Attempts to close the pipe. If not stoppable, returns false;
	bool (*const close)(struct stream_pipe_t *);

	//Delete the pipe's dynamic data; Called by owner stream only;
	void (*const destructor)(struct stream_pipe_t *);

};


//Activate a stream pipe, given the owner stream and the client stream; Both streams will be opened by the pipe;
//TODO FILENAMES + PIPE FILENAME
void stream_pipe_open(struct stream_t *owner, struct stream_t *client);

//Attempts to close a pipe. If possible, un-registers itself and close both owned streams;
// If not possible, schedules its closing, will be done at the current transfer's confirmation;
//TODO PIPE_FILENAME
void stream_pipe_close(struct stream_pipe_t *pipe);

//Prepare the transfer;
void stream_pipe_prepare_transfer(struct stream_pipe_t *pipe);

//Confirm transfer;
void stream_pipe_confirm_transfer(struct stream_pipe_t *pipe, size_t nb_transferred_blocks);




#endif