
#ifndef TRACER_HEAP_H
#define TRACER_HEAP_H

#include <stddef.h>

//Predeclare the heap block structure; Struct definition can be found in heap.c;
struct heap_block;


typedef void (*heap_insertion_f)(struct heap_block **first_block, struct heap_block *new_block);


/*
 * A heap head contains data describing the whole memory region covered by the heap;
 */

struct heap_head {

	//The address we created the heap in. Used to free the space;
	void *creation_address;

	//The size of the heap;
	const size_t init_block_size;

	//The first block of the heap, constant;
	struct heap_block *const first_block;
	
	//The first available block (with its space available) of the heap, mutable;
	struct heap_block *first_available_block;

	//The function that inserts available blocks in the list;
	heap_insertion_f insertion_f;

};


//------------------------------------------------------ Creation ------------------------------------------------------

//Create a heap in a memory block;
struct heap_head *heap_create(
	void *start_address,
	size_t size,
	heap_insertion_f insertion_f
);

//Reset the heap; All data will be invalidated;
void heap_reset(struct heap_head *heap);


void heap_print(struct heap_head *head);


//---------------------------------------------------- Manipulation ----------------------------------------------------

//Allocate some data in the heap;
void *heap_malloc(struct heap_head *heap, size_t size);

//Allocate some data in the heap, and initialises all bytes to 0;
void *heap_calloc(struct heap_head *heap, size_t size);

//Allocate some data in the heap, and copy the initializer in the allocated memory region;
void *heap_ialloc(struct heap_head *heap, size_t size, void *initializer);

//Free some data in the heap;
void heap_free(struct heap_head *heap, void *ptr);

//Reallocate some data in the heap;
void *heap_realloc(struct heap_head *heap, void *ptr, size_t size);


//----------------------------------------------------- Insertion ------------------------------------------------------

/*
 * Here are defined several heap insertion functions, that will have different complexities/efficiency.
 */

//The available blocks list is a fifo;
void heap_fifo_insertion(struct heap_block **, struct heap_block *);

//The available fifo is sorted, on a size base;
void heap_size_sort_insertion(struct heap_block **, struct heap_block *);


#endif //TRACER_HEAP_H