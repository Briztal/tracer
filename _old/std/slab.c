//
// Created by root on 12/28/18.
//

#include <stdlib.h>
#include <dmz/stdmem.h>
#include "slab.h"



#define alignment_size (sizeof(size_t))


/**
 * correct_alignment_size : Will return the size value that is a direct multiple of the primitive type size, size_t;
 *
 * 	The resulting size will maintain the most strict alignment when added or subtracted to aligned addresses;
 *
 * @param size : the size to correct;
 * @return the closest superior multiple of alignment of size;
 */

static inline size_t correct_alignment_size(size_t size) {
	
	//If size is not a multiple of alignment_size(that is a power of 2);
	if (size & (alignment_size - 1)) {
		
		//Add alignment_word to be superior to @size, and truncate to the closest inferior multiple of alignment_size;
		return ((size + alignment_size) & ~(alignment_size - 1));
		
	} else {
		
		//If size if a multiple of alignment_size, no modifications to do;
		return size;
		
	}
	
}


//-------------------------------------------------------- Slab --------------------------------------------------------


static struct slab_head *slab_create(size_t object_size, size_t slab_size) {
	
	//Correct the object size, and add the header size to get the block size;
	size_t block_size = correct_alignment_size(object_size);
	
	//Decrease the slab size to determine the free size;
	size_t free_size = slab_size - sizeof(struct slab_head);
	
	//Determine the number of blocks;
	size_t nb_blocks = free_size / block_size;
	
	//Initialise the first slab;
	struct slab_head *slab = malloc(slab_size);
	
	//If failed, quit;
	if (!slab)
		return 0;
	
	//Determine the start of the free zone;
	void *free_start = slab + 1;
	
	//Create the initializer of the slab;
	struct slab_head slab_init = {
		
		//The size of the slab;
		.sa_slab_size = slab_size,
		
		//The size of a slab block, header comprised;
		.s_bsize = block_size,
		
		//The number of free blocks;
		.s_nb_free = nb_blocks,
		
		//A free block, at init, totally non fragmented.
		.s_free = 0,
		
		//The start of the non-fragmented zone;
		.sa_non_fragmented = free_start,
		
	};
	
	//Initialise the slab;
	memcpy(slab, &slab_init, sizeof(struct slab_head));
	
	//Return a ref to the allocated slab;
	return slab;
	
}

static void slab_delete(struct slab_head *slab) {
	
	//Delete the slab;
	free(slab);
	
}


static void *_slab_alloc(struct slab_head *slab) {


}

static void _slab_free(struct slab_head * )



//Initialise a slab allocator;
bool slab_init(struct slab_allocator *allocator, size_t object_size, size_t slab_size) {
	
	//Create a slab;
	struct slab_head *slab = slab_create(object_size, slab_size);
	
	//If error, quit;
	if (!slab)
		return false;
	
	//Create the initializer;
	struct slab_allocator init = {
		
		//The size of an object, slab object header not comprised;
		.sa_osize = object_size,
		
		//The slab;
		.sa_slab = slab,
	};
	
	//Initialize the slab;
	memcpy(allocator, &init, sizeof(struct slab_allocator));
	
	//Complete;
	return true;
	
}



//Deinit a slab allocator;
void slab_exit(struct slab_allocator *allocator) {
	
	//Free the slab;
	free(allocator->sa_slab);
	
}


//Allocate a block;
void *slab_alloc(struct slab_allocator *allocator);

//Free a block;
void slab_free(struct slab_allocator *allocator, void *);
