//
// Created by root on 7/17/18.
//

#ifndef TRACER_MEMORY_TRANSFER
#define TRACER_MEMORY_TRANSFER

#include <stdbool.h>

#include <stdint.h>

#include <stddef.h>
#include "memory_transfer.h"


/*
 * A stream memory zone is a memory zone described
 */

//---------------------------------------------------- Stream memory ---------------------------------------------------

/*
 * A stream memory structure references a memory map, indexing blocs of a fixed size, but whose numbers varies in time;
 *
 * 	It allows sending/receiving of data from an unspecified source.
 *
 * 	update_bloc_descriptor returns the number of manipulable blocs in the memory;
 *
 * 	discard_blocs updates the memory zone after a certain number of blocs have been read/written, to offer new
 * 		manipulable blocs; It will return false if the memory zone has become inactive.
 *
 */

struct stream_memory_t {

	//---------------- Fields ----------------

	//The stream direction; Data can be written on an input stream, and read from an output stream;
	const bool input_stream;


	//---------------- Stream lifecycle ----------------

	//Update the number of available blocs, in the blocks_descriptor field;
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

//The stream doesn't immediately require a destructor, since it does not own dynamic data; Though, stream memories are
//not owned by the caller, and so must not be deleted manually;
inline void stream_delete(struct stream_memory_t *stream_memory) {

	//Call the destructor;
	(*(stream_memory->destructor))(stream_memory);

}

//-------------------------------------------------- Default transfers -------------------------------------------------

//Call to memtransfer;
size_t stream_default_reader(struct stream_memory_t *stream, struct mem_desc_t *dst);

//Call to memtransfer;
size_t stream_default_writer(struct stream_memory_t *stream, struct mem_desc_t *src);


//---------------------------------------------------- Transfer ----------------------------------------------------

//Transfer blocs between the stream and another memory zone;
size_t stream_transfer(struct stream_memory_t *stream, struct mem_desc_t *dst);

//Read blocs from the stream memory; Returns the size of the read zone, zero if bad direction;
size_t stream_read(struct stream_memory_t *stream, struct mem_desc_t *dst);

//Copy blocs to the stream memory; Returns the size of the copied zone, zero if bad direction
size_t stream_write(struct stream_memory_t *stream, struct mem_desc_t *src);



#endif