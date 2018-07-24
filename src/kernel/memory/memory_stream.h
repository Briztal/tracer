//
// Created by root on 7/17/18.
//

#ifndef TRACER_MEMORY_TRANSFER
#define TRACER_MEMORY_TRANSFER

#include <stdbool.h>

#include <stdint.h>

#include <stddef.h>


/*
 * A stream memory zone is a memory zone described
 */


/*
 * When executing a memory transfer between several memory zones the two following issues may be encountered :
 * 	- one of concerned memory zone is volatile, ie, may be deleted during the transfer, transfer has to be aborted or
 * 		completed before deletion is allowed;
 * 	- transfer must be triggered on an asynchronous base, and must be partially executed.
 *
 * This library defines the basis for memory transfers that may involve volatile unowned memory regions, or may be
 * 	triggered and executed asynchronously;
 *
 * A volatile memory zone is a contiguous memory bloc, granting access for transfer, whose ownership is undefined, and
 * 	that can be deleted at any time; When its data has been entirely transferred, it can be updated through the use of
 * 	a function pointer. It will eventually update the memory zone, and return true if transfer must continue.
 * 	If it returns false, no transfer can be done anymore.
 *
 * When a volatile memory zone must be deleted, it must notify its transfer process, reason why the memory zone also
 * 	owns a pointer to its transfer process;
 *
 * A memory transfer process will be defined as a set of volatile memory zones, that it can manipulate, and a transfer
 * 	medium. Each memory zone gives him access to the update function, that it will call for each memory zone each time
 * 	the transfer is complete, and eventually delete itself, as the next section describes.
 *
 * A memory transfer process has no defined owner; It can be seen as owned by the set of memory zones it concerns, as
 * it deletes itself as soon as one of them is unregistered from him;
 *
 * memory transfer lifecycle :
 *
 * 	- creation of a set of memory zones;
 * 	- initialisation of memory zones containment, and access size;
 * 	- creation of a transfer instance that supports the set of memory zone;
 * 	- registration of the set of memory zones to the transfer instance;
 * 	- iteration over all the communication chain, if chain there is;
 * 	- initialisation of all transfers of the chain;
 *
 * 	- L : transfer execution, several times;
 * 	- transfer completion, or interruption;
 * 	- transfer size calculation;
 * 	- memory zon-es update;
 * 	- computation of the new transfer, or invalidation of all memory zones and transfer destruction if a destruction
 * 		order is received through the zone update function;
 * 	- back to L if no termination order;
 *
 * 	When a memory zone must be deleted :
 * 	- critical section;
 * 	- if the memory is unregistered, go to end;
 * 	- cache the transfer pointer;
 * 	- un-registration of the memory zone;
 * 	- call of transfer_interrupt;
 * 	- transfer will be interrupted, memory zones will be updated. One memory zone will be unregistered.
 * 	- the transfer will un-register itself of both memory zones, delete itself, and return;
 * 	- memory zone is unregistered and transfer is terminated and deleted, memory won't be accessed anymore;
 * 	end :
 * 	- end of critical section;
 * 	- memory can be deleted;
 *
 * 	A memory zone is managed by a data structure, whose only constraints are :
 * 	- to have a defined owner;
 * 	- when it need to delete itself, it first deletes each memory zone, and deletes itself.
 * 	- when it finds one of its zones unregistered, it has the choice to delete all its zones, or to keep them,
 * 		but it must NOT delete itself, as its owner may still use it;
 *
 */

//---------------------------------------------------- Stream memory ---------------------------------------------------

/*
 * A stream memory zone is a succession of constantly distanced data blocs of the same size,
 * that can be manipulated by blocs;
 *
 * 	It includes a start address, from which available blocs can be read, an offset between blocs, and a bloc size;
 *
 * 	It allows sending/receiving of data from an unspecified source.
 *
 * 	available_blocs returns the number of manipulable blocs in the memory;
 *
 * 	discard_blocs updates the memory zone after a certain number of blocs have been read/written, to offer new
 * 		manipulable blocs; It will return false if the memory zone has become inactive.
 *
 */

struct stream_memory_t {

	//The first address of the memory zone;
	volatile uint8_t *start_address;

	//An eventual register to read before the last reading;
	volatile uint8_t *clear_register;

	//The size of a data bloc;
	const size_t bloc_size;

	//The size between two bloc's first address.
	//0 means bloc always at the same address, bloc_spacing < bloc_size means blocs overlap;
	const uint16_t bloc_spacing;

	//The direction of the bloc_spacing;
	const bool increment_positve;

	//The stream direction; Data can be written on an input stream, and read from an output stream;
	const bool input_stream;


	//The number of currently available blocs;
	size_t (*const available_blocs)(struct stream_memory_t *memory);

	//Update the memory after blocs have been manipulated;
	bool (*const discard_blocs)(struct stream_memory_t * memory, size_t nb_blocs);

	//Call the owner of the stream memory for deletion;
	void (*const destructor)(struct stream_memory_t *memory);

	//TODO INCLUDE A TRIGGER HERE ?

};


//----------------------------------------------------- Init - Exit ----------------------------------------------------

#define ACTIVE_STREAM(_start_address, _bloc_size, _increment, _increment_positive, \
						_input_stream, _available_blocs, _discard_blocs, _destructor)\
		{\
			.start_address = (_start_address),\
			.bloc_size = (_bloc_size),\
			.bloc_spacing = (_increment),\
			.increment_positve = (_increment_positive),\
			.input_stream = (_input_stream),\
			.available_blocs = (_available_blocs),\
			.discard_blocs = (_discard_blocs),\
			.destructor = (_destructor),\
		}


//The stream doesn't immediately require a destructor, since it does not own dynamic data; Though, stream memories are
//not owned by the caller, and so must not be deleted manually;
inline void stream_delete(struct stream_memory_t *memory) {

	//Call the destructor;
	(*(memory->destructor))(memory);

}

//----------------------------------------------------- Init - Exit ----------------------------------------------------

//Read blocs from the stream memory; Returns the size of the read zone, zero if bad direction;
size_t stream_read(struct stream_memory_t *stream, size_t nb_blocs, void *dst, size_t dst_size);

//Copy blocs to the stream memory; Returns the size of the copied zone, zero if bad direction
size_t stream_write(struct stream_memory_t *stream, size_t nb_blocs, const void *src, size_t dst_size);


#endif