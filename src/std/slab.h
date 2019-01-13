//
// Created by root on 12/28/18.
//

#ifndef TRACER_SLAB_H
#define TRACER_SLAB_H

#include <stdint.h>

#include "list.h"


struct sblock_head {
	
	struct list_head available_head;
	
};



struct slab_head {
	
	//The size of the slab;
	const size_t sa_slab_size;
	
	//The size of a slab block, header comprised;
	const size_t s_bsize;
	
	//The slab capacity;
	const size_t s_nb_blocks;
	
	//The number of free blocks;
	size_t s_nb_free;
	
	//A free block;
	struct sblock_head *s_free;
	
	//The start of the non-fragmented zone;
	void *sa_non_fragmented;
	
};



struct slab_allocator {
	
	//The object size, header not comprised;
	size_t sa_osize;
	
	//The slab;
	struct slab_head *sa_slab;

};


//Initialise a slab allocator;
bool slab_init(struct slab_allocator *allocator, size_t object_size, size_t slab_size);

//Deinit a slab allocator;
void slab_exit(struct slab_allocator *allocator);


//Allocate an object;
void *slab_alloc(struct slab_allocator *allocator);

//Free an object;
void slab_free(struct slab_allocator *allocator, void *object);



#endif //TRACER_SLAB_H
