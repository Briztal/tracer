/*
  heap.c Part of TRACER

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


#include <stdbool.h>

#include <stdint.h>

#include <string.h>

#include "heap.h"
#include "core/core.h"

#include <util/type.h>
#include <core/debug.h>


#define alignment_size (sizeof(size_t))

/**
 * correct_alignment_size : Will return the size value that is a direct multiple of the primitive type size, size_t;
 *
 * 	The resulting size will maintain the most strict alignment when added or subtracted to aligned addresses;
 *
 * @param size : the size to correct;
 * @return the closest superior multiple of alignment of size;
 */

static inline size_t __correct_alignment_size(size_t size) {

	//If size is not a multiple of alignment_size(that is a power of 2);
	if (size & (alignment_size - 1)) {

		//Add alignment_word to be superior to @size, and truncate to the closest inferior multiple of alignment_size;
		return ((size + alignment_size) & ~(alignment_size - 1));

	} else {

		//If size if a multiple of alignment_size, no modifications to do;
		return size;

	}

}


//For speed purposes, the following macro updates the variable. No call required, easier to optimise;
#define correct_alignment_size(size) {\
        if ((size) & (alignment_size- 1)) {\
            (size) =  (((size) + alignment_size) & ~(alignment_size- 1));\
        }\
    }


#define correct_address_size_alignment(address, size) {\
        (size) = (size) & ~(alignment_size- 1); \
        if (((size_t)(address)) & ~(alignment_size- 1)) {\
            (address) =  (void *)(((size_t)(address) + alignment_size) & ~(alignment_size- 1));\
        }\
    }


//----------------------------------------------------- Heap block -----------------------------------------------------

/*
 * As there is space for a flag in the heap block header, we will use it as an indicator for double free of free to
 * 	unallocated; This will not work all times but will offer a usefull safeguard;
 */

#define HEAP_ALLOCATED_STATUS   ((uint32_t)0xDEADBEEF)
#define HEAP_FREE_STATUS        ((uint32_t)0xFEEBDAED)
#define HEAP_RESET_STATUS		((uint32_t)0xEDDAEBEF)
//TODO HEAP_INVALIDATED_STATUS

/*
 * A heap block is a header contains all data required to describe properly a memory block;
 */

struct heap_block {

	//Blocks are linked contiguously;
	struct list_head head;

	//Available blocks are linked non-contiguously, and possibly sorted;
	struct list_head available_head;

	//The status of the memory;
	uint32_t status;

	//The size of the memory block, data and header comprised:
	size_t block_size;

};


//The offset required to convert a heap block pointer to the pointer to the data it references;
#define HEAP_BLOCK_OFFSET sizeof(struct heap_block);


void heap_print(struct heap_head *head) {

	core_log("heap print : ");
	core_log_int(sizeof(struct heap_head));
	core_log_int(sizeof(struct heap_block));

	core_log("blocks print : ");

	{
		//Cache the first block;
		struct heap_block *const blk = head->first_block, *b = blk;

		uint32_t i = 0;

		do {

			core_log_int(i++);
			core_log_int((uint32_t) b);

			if (b->status == HEAP_FREE_STATUS) {
				core_log("\tfree");
			} else if (b->status == HEAP_ALLOCATED_STATUS) {
				core_log("\tallocated;");
			} else {
				core_log("\tinvalid;");
			}

			core_log("block size : ");
			core_log_int(b->block_size);

			core_log("\n");

			//Update the next block
			b = b->head.next;

		} while (b != blk);
	}
	{

		core_log("av blocks print : ");

		//Cache the first block;
		struct heap_block *const blk = head->first_available_block, *b = blk;

		if (!blk) {
			core_log("No  available blocks");
			return;
		}

		uint32_t i = 0;

		do {

			core_log_int(i++);
			core_log_int((uint32_t) b);

			if (b->status == HEAP_FREE_STATUS) {
				core_log("\tfree");
			} else if (b->status == HEAP_ALLOCATED_STATUS) {
				core_log("\tallocated;");
			} else {
				core_log("\tinvalid;");
			}

			core_log("block size : ");
			core_log_int(b->block_size);

			core_log("\n");

			//Update the next block
			b = b->available_head.next;


		} while (b != blk);

	}

}


//-------------------------------------------- Available list manipulation ---------------------------------------------

/**
 * heap_insert_available_block : sub-call to the insertion function;
 */

static inline void heap_insert_available_block(struct heap_head *heap, struct heap_block *block) {

	//Cache the first block on the list;
	struct heap_block **const first_block = &heap->first_available_block;

	//If the heap's available list is empty :
	if (!(*first_block)) {

		//Just save the first block;
		*first_block = block;

	} else {

		//If the list is not empty :

		//Call the insertion function;
		(*(heap->insertion_f))(first_block, block);
	}

}

/**
 * heap_remove_available_block : sub-call to the insertion function;
 */

static void heap_remove_available_block(struct heap_head *heap, struct heap_block *block) {

	//If the block is the first element of the available blocks list :
	if (heap->first_available_block == block) {

		struct heap_block *next = block->available_head.next;

		//If @block is the only element of the list :
		if (block == next) {

			//Mark the list empty;
			heap->first_available_block = 0;

		} else {
			//If @block has a successor :

			//Mark the successor as the first available block;
			heap->first_available_block = next;

		}

	}

	//Remove @block from its list;
	elist_remove(struct heap_block, available_head, block);

}



//------------------------------------------------- Block sanity check -------------------------------------------------

/**
 * heap_check_free_block : checks if the block is FREE, returns if so. If not, calls the error function;
 *
 * @param block : the block to check, supposedly in the FREE status;
 */

static void heap_check_free_block(const struct heap_head *const heap, const struct heap_block *const block) {

	//Cache the block's status;
	uint32_t status = block->status;


	/*
	 * Verify that the block is valid. A valid memory block will always be in status FREE;
	 * 	Though, an invalid memory block may be in status FREE; This method is an indicator, it will not
	 * 	necessarily detect all memory leaks;
	 */

	//If the block's status is not FREE, a memory leak has occurred :
	if (status != HEAP_FREE_STATUS) {

		//If the block's status is allocated, log and quit;
		if (status == HEAP_ALLOCATED_STATUS) {

			//Log and quit;
			core_error("heap.c : heap_malloc : block is marked allocated, memory leak;");

		} else {

			//Log and quit;
			core_error("heap.c : heap_malloc : block status invalid, memory leak.");

		}

	}
}


/**
 * heap_check_allocated_block : checks if the block is ALLOCATED, returns if so. If not, calls the error function;
 *
 * @param block : the block to check, supposedly in the ALLOCATED status;
 */

static void heap_check_allocated_block(const struct heap_head *const heap, const struct heap_block *const block) {

	//Cache the block status;
	uint32_t status = block->status;

	/*
	 * Verify that the block is valid. A valid memory block will always be in status HEAP_ALLOCATED;
	 * 	Though, an invalid memory block may be in status HEAP_ALLOCATED; This method is an indicator, it will not
	 * 	necessarily detect all double/invalid free call;
	 */

	if (status != HEAP_ALLOCATED_STATUS) {

		//If the block is in FREE status, a double free may have been called;
		if (status == HEAP_FREE_STATUS) {

			//Log and quit;
			core_error("heap.c : heap_free : block marked free; double free - invalid free and status collision"
						   " - memory leak.");

		} else {

			//Log and quit;
			core_error("heap.c : heap_free : block status invalid. memory leak - invalid free");

		}

	}

}


/**
 * heap_check_allocated_block : checks if the block is ALLOCATED, returns if so. If not, calls the error function;
 *
 * @param block : the block to check, supposedly in the ALLOCATED status;
 */

static void heap_check_block(const struct heap_head *const heap, const struct heap_block *const block) {

	//Cache the block status;
	uint32_t status = block->status;

	/*
	 * Verify that the block has a valid status;
	 */

	if ((status != HEAP_ALLOCATED_STATUS) && (status != HEAP_FREE_STATUS)) {

		//Log and quit;
		core_error("heap.c : heap_free : block status invalid. memory leak;");

	}

}


//------------------------------------------------------ Creation ------------------------------------------------------

/**
 * heap_create : initialises a heap container in the provided memory block;
 * 	If the block is too small, it will return 0;
 *
 * 	If the address or the size are not alignable, the size will be decreased, and the address increased, in order to
 * 		meet alignment requirements, while still fitting in the provided memory block;
 *
 * @param start_address : the lowest bound of the block;
 * @param size : the block size;
 * @return : 0 if init failed, the address of the heap head if it succeeded;
 */

struct heap_head *heap_create(void *start_address, size_t size, heap_insertion_f insertion_f) {

	/*
	 * Size and address correction;
	 */

	//Save the creation address;
	void *const creation_address = start_address;

	//If the size is not alignable, decrease it to meet alignment requirement;
	size = size & ~(alignment_size - 1);

	//If the start address is not aligned properly : 
	if (((size_t) start_address) & (alignment_size - 1)) {

		//Increase the start address to meet alignment requirements;
		start_address = (void *) (((size_t) start_address + alignment_size) & ~(alignment_size - 1));

		//Decrease the size of one alignment word;
		size -= alignment_size;

	}

	//If the block is too small to contain a heap head :
	if (size < sizeof(struct heap_head)) {

		//Fail, not enough space;
		return 0;

	}


	/*
	 * Initialisation
	 */

	//Set the heap head at the beginning of the block;
	struct heap_head *heap = start_address;

	//Update @start_address to reflect the address right after the heap;
	start_address = (uint8_t *) start_address + sizeof(struct heap_head);

	//Determine the first block size;
	size_t init_block_size = size - sizeof(struct heap_head);

	//If the block is too small to contain a heap block :
	if (init_block_size < sizeof(struct heap_block)) {

		//Fail, not enough space;
		return 0;

	}

	//Set the first heap block right after the heap head;
	struct heap_block *first_block = start_address;


	//Create the first block initializer;
	struct heap_block first_block_init = {

		//The block is the first in the heap for instance, no linking;
		.head = {
			.next = first_block,
			.prev = first_block,
		},

		//The block is the first available in the heap;
		.available_head = {
			.next = first_block,
			.prev = first_block,
		},

		//The space is free, set in the heap fred status;
		.status = HEAP_FREE_STATUS,

		//All remaining space belongs to the block;
		.block_size = init_block_size,

	};

	//Initialise the first block;
	memcpy(first_block, &first_block_init, sizeof(struct heap_block));

	//Create the initializer for the heap head;
	struct heap_head head_init = {
		.creation_address = creation_address,
		.init_block_size = init_block_size,
		.first_block = first_block,
		.first_available_block = first_block,
		.insertion_f = insertion_f,
	};

	//Initialise the heap head;
	memcpy(heap, &head_init, sizeof(struct heap_head));

	//Return the initialised heap head reference;
	return heap;

}


/**
 * heap_reset : resets the heap. All blocks will be marked reset to invalid frees, and the first block will be linked
 * 	to itself;
 *
 * @param heap : the heap to reset;
 */

void heap_reset(struct heap_head *heap) {

	//Cache the first heap block;
	struct heap_block *block = heap->first_block, *const first_block = block;


	//Reset all blocks except the first;
	while (true) {

		//Go to the next block. First will not be reset;
		block = block->head.next;

		//If we are back to the first block, stop;
		if (block == first_block)
			break;

		//Reset the block;
		block->status = HEAP_RESET_STATUS;

	}

	//Create the first block initializer;
	struct heap_block first_block_init = {

		//The block is the first in the heap for instance, no linking;
		.head = {
			.next = first_block,
			.prev = first_block,
		},

		//The block is the first available in the heap;
		.available_head = {
			.next = first_block,
			.prev = first_block,
		},

		//The space is free, set in the heap fred status;
		.status = HEAP_FREE_STATUS,

		//All remaining space belongs to the block;
		.block_size = heap->init_block_size,

	};

	//Initialise the first block;
	memcpy(first_block, &first_block_init, sizeof(struct heap_block));

	//Reset the heap's first available block;
	heap->first_available_block = first_block;

}


//--------------------------------------------------- Split - Merge ----------------------------------------------------

/**
 * heap_split_block : splits the required block into two blocks, the first containing @data_size bytes, and the other
 * 	containing the rest;
 *
 * @param head : the heap where @block belongs;
 * @param block : the block to split;
 * @param block_size : the required new size of the block, header comprised;
 */

static void heap_split_block(struct heap_head *const head, struct heap_block *const block, const size_t block_size) {

	//If the block can't contain bytes, and another non-empty block :
	if (block->block_size <= block_size + sizeof(struct heap_block)) {

		//Nothing to do, the block can't be split;
		return;

	}


	//The new block will be located after the current block, block head and data comprised;
	struct heap_block *new_block = block + block_size;

	//Create the new block's initializer;
	struct heap_block new_block_init = {

		//Link the block to itself;
		.head = {
			.prev = new_block,
			.next = new_block,

		},

		//Link the block to itself;
		.available_head = {
			.prev = new_block,
			.next = new_block,
		},

		//The space is free, set in the heap fred status;
		.status = HEAP_FREE_STATUS,

		//The new block is directly after @block's end
		//All sizes and pointers are properly aligned, no correction required;
		.block_size = block->block_size - block_size,

	};

	//Update the previous block's size;
	block->block_size = block_size;

	//Initialise the new block;
	memcpy(new_block, &new_block_init, sizeof(struct heap_block));

	//Insert the new block after the provided one;
	list_add((struct list_head *) block, (struct list_head *) new_block);

	//Insert the new block in the available list;
	heap_insert_available_block(head, new_block);

}


/**
 * heap_merge_blocks : merges the provided block and its successor, only if both are free, and if the provided block
 * 	is not the last of the memory zone;
 *
 * @param block : the block from which to get the successor;
 */

static bool heap_merge_blocks(struct heap_head *heap, struct heap_block *block) {

	//Check the block.
	heap_check_block(heap, block);

	//If the block is free :
	if (block->status != HEAP_FREE_STATUS) {

		//No merge to do, no data has been modified;
		return false;

	}

	//Cache the block's direct successor;
	struct heap_block *next = block->head.next;

	//Check the successor;
	heap_check_block(heap, block);

	//If the successor is free :
	if (next->status != HEAP_FREE_STATUS) {

		//No merge to do, no data has been modified;
		return false;

	}

	//If the successor is the first block on the list (linking goes in increasing addresses) :
	if (next <= block) {

		//No merge to do, can only merge contiguous blocks; no data has been modified;
		return false;

	}

	//Remove both blocks from their available lists;
	heap_remove_available_block(heap, block);
	heap_remove_available_block(heap, next);

	//Remove the successor from the blocks list; @block's head will be updated;
	list_remove((struct list_head *) next);

	//Update @block 's data length, to contain @next;
	block->block_size += next->block_size;

	//Data has been modified, @block must be re-inserted manually in the available list;
	return true;


}


//---------------------------------------------------- Manipulation ----------------------------------------------------

/**
 * Allocate memory block;
 *
 * Allocates a block of size bytes of memory, returning a pointer to the beginning of the block.
 *
 * The content of the newly allocated block of memory is not initialized, remaining with indeterminate values.
 *
 * If size is zero, a null pointer is provided;
 *
 * If the required size can't be aligned (not a multiple of @alignment_word) the size will be increased in order to
 * 	provide an alignable block, that still can contain the required size;
 *
 * @param heap : the heap memory zone where to allocate the block;
 * @param size : the required size of the memory block;
 *
 * @return a pointer to the required memory block, or 0 if error/null size;
 */

void *heap_malloc(struct heap_head *heap, size_t size) {

	//If a null size was provided :
	if (!size) {

		//Return a null pointer;
		return 0;

	}

	//First, correct the size, so that it maintains alignment during pointer arithmetic;
	correct_alignment_size(size);

	//Add the block header to the size;
	size += sizeof(struct heap_block);

	//Cache the first available block;
	struct heap_block *const first_block = heap->first_available_block;

	//If there is no available block :
	if (!first_block) {

		//Fail, no space left;
		core_error("heap.c : heap_malloc : no space left");

		//Never reached;
		return 0;

	}

	//Create a temp for the first block;
	struct heap_block *block = first_block;

	//For each block in the list :
	do {

		//TODO DISABLE CHECKING

		heap_check_free_block(heap, block);

		//If the block can contain the required size :
		if (block->block_size >= size) {

			//We can split the block in two blocks, the first being of the required size;
			heap_split_block(heap, block, size);

			//Set to allocated;
			block->status = HEAP_ALLOCATED_STATUS;

			//Remove the block from the available list;
			heap_remove_available_block(heap, block);

			//Return the lowest address of the block's data zone;
			return (void *) ((uint8_t *) block + sizeof(struct heap_block));

		}

		//Re-iterate for the next available block;
		block = block->available_head.next;


	} while (block != first_block);


	//If we reached the end of the block, error;
	core_error("heap.c : heap_malloc : no memory block large enough;");

	//Never reached;
	return 0;

}


/**
 * heap_calloc : Allocate and zero-initialize block;
 *
 * Allocates a block of memory of @size bytes, and initializes it to zero.
 *
 * The effective result is the allocation of a zero-initialized memory block of @size bytes.
 *
 * If @size is zero, 0 will be returned;
 *
 * @param heap : the heap where to allocate the memory block;
 * @param size : the required size of the memory block;
 * @return the lowest data address of the allocated block;
 */

void *heap_calloc(struct heap_head *heap, size_t size) {

	//If the size is null, return 0;
	if (!size) {
		return 0;
	}

	//Allocate a block of the required size;
	void *ptr = heap_malloc(heap, size);

	//Set all bytes to 0;
	memset(ptr, 0, size);

	//Return the null-initialised block;
	return ptr;

}


/**
 * heap_ialloc : Allocate and initialize block;
 *
 * Allocates a block of memory of @size bytes, and copies @init, supposedly of @size bytes too,  in it;
 *
 * The effective result is the allocation of an initialized memory block of @size bytes.
 *
 * If @size is zero, 0 will be returned;
 *
 * @param heap : the heap where to allocate the memory block;
 * @param size : the required size of the memory block;
 * @param initializer : the initializer of the memory block;
 * @return the lowest data address of the allocated block;
 */

void *heap_ialloc(struct heap_head *heap, size_t size, const void *initializer) {

	//If the size is null, return 0;
	if (!size) {
		return 0;
	}

	//Allocate a block of the required size;
	void *ptr = heap_malloc(heap, size);


	//Initialise the block;
	memcpy(ptr, initializer, size);

	//Return the null-initialised block;
	return ptr;

}


/**
 * Deallocate memory block;
 *
 * A block of memory previously allocated by a call to malloc is deallocated, making it available again for further
 * 	allocations.
 *
 * If @ptr does not point to a block of memory allocated with the above functions, it causes undefined behavior.
 * 	The function will attempt to catch such behaviour and to throw an error, but it is not guaranteed;
 *
 * If @ptr is a null pointer, the function does nothing.
 *
 * Notice that this function does not change the value of @ptr itself, hence it still points to the same (now invalid)
 * location;
 *
 * @param heap : the heap memory zone that contains @ptr;
 * @param ptr : the data pointer that identifies a heap block to free;
 */

void heap_free(struct heap_head *heap, void *ptr) {

	//If a null pointer was provided :
	if (!ptr) {

		//Nothing to do;
		return;

	}

	//Determine the address of the related heap block;
	struct heap_block *block = (struct heap_block *) ((uint8_t *) ptr - sizeof(struct heap_block));

	//TODO ERROR MESSAGE
	//TODO DISABLE CHECKING
	heap_check_allocated_block(heap, block);

	//Mark the memory block available;
	block->status = HEAP_FREE_STATUS;

	//Merge @block and its successor;
	heap_merge_blocks(heap, block);

	//Cache the block's predecessor;
	struct heap_block *prev = block->head.prev;

	//Merge block and its predecessor;
	bool prev_to_reinsert = heap_merge_blocks(heap, prev);

	//If the previous block must be re-inserted :
	if (prev_to_reinsert) {

		//Re-insert the previous block in the list;
		heap_insert_available_block(heap, prev);

	} else {
		//If the fred block must be reinserted;

		//Insert the fred block in the list;
		heap_insert_available_block(heap, block);

	}

}


/**
 * Reallocate memory block;
 *
 * Changes the size of the memory block pointed to by @ptr.
 *
 * The function may move the memory block to a new location (whose address is returned by the function).
 *
 * The content of the memory block is preserved up to the lesser of the new and old sizes, even if the block is moved
 * to a new location. If the new size is larger, the value of the newly allocated portion is indeterminate.
 *
 * In case that @ptr is a null pointer, the function behaves like malloc, assigning a new block of @size bytes and
 * returning a pointer to its beginning.
 *
 * @param heap : the heap memory zone that covers @data;
 * @param ptr : the data pointer that identifies a heap block to free;
 * @param data_size : the new required size;
 *
 * @return : a pointer to the new position;
 */

void *heap_realloc(struct heap_head *heap, void *ptr, size_t data_size) {

	//If a null pointer was provided :
	if (!ptr) {

		//Allocate some data;
		return heap_malloc(heap, data_size);

	}

	//If the size is null :
	if (!data_size) {

		//Return a null pointer;
		return 0;

	}

	//Determine the address of the related heap block;
	struct heap_block *block = (struct heap_block *) ((uint8_t *) ptr - sizeof(struct heap_block));

	//TODO ERROR MESSAGE
	//TODO DISABLE CHECKING
	heap_check_allocated_block(heap, block);

	//Cache the block data size;
	size_t block_size = block->block_size;
	size_t block_data_size = block_size  - sizeof(struct heap_block);

	//If both sizes are equal :
	if (data_size == block_data_size) {

		//Nothing to do;
		return ptr;

	} else if (data_size < block_data_size) {
		//If the block must be reduced :

		//We can split the block in two blocks, the first being of the required size;
		heap_split_block(heap, block, block_size);

		//If the block can't be split, nothing to do;
		return ptr;

	} else {
		//If the block must be enlarged :

		//Allocate another block somewhere else in the heap with the correct size;
		void *new_ptr = heap_malloc(heap, data_size);

		//Copy the previous data block at the beginning of the new one;
		memcpy(new_ptr, ptr, data_size);

		//Free the old block;
		heap_free(heap, ptr);

		//Return the new pointer;
		return new_ptr;

	}

}


//----------------------------------------------------- Insertion ------------------------------------------------------

//The available blocks list is a fifo;
void heap_fifo_insertion(struct heap_block **first_block, struct heap_block *new_block) {

	//Cache the first block;
	struct heap_block *f = *first_block;

	//Add the new block before the first block;
	elist_concat(struct heap_block, available_head, new_block, f);

	//Set the new block to be the first block;
	*first_block = new_block;

}

//The available fifo is sorted, on a data size base;
void heap_size_sort_insertion(struct heap_block **first_block_p, struct heap_block *new_block) {


	//Cache the first block;
	struct heap_block *const first_block = *first_block_p;
	struct heap_block *block = first_block;

	//A flag set if new_block must be the new first block of the list;
	bool first = true;

	//For each block of the list
	do {

		//If the block's size is greater than the new block's size, stop here;
		if (block->block_size > new_block->block_size) {
			break;
		}

		//@new_block has not the lowest size of the list, @first_block will stay in place in @first_block_p
		first = false;

		//Cache the next block;
		block = block->available_head.next;


	} while (block != first_block);

	//Bloc is the first greater than new_block;

	//Add the new block before @block;
	elist_concat(struct heap_block, available_head, new_block, block);

	//If new_block has the lowest size of the list :
	if (first) {

		//Set the new block to be the first block of the list;
		*first_block_p = new_block;

	}


}



