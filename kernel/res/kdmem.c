//
/* Created by root on 10/10/18.*/
/**/


#include "kdmem.h"


#include <kernel/hard/ram.h>
#include <panic.h>
#include <kernel/hard/debug/printk.h>



/*--------------------------------------------------- Make Parameters --------------------------------------------------*/

/*The memory library required NB_THREADS to be provided by the makefile;*/
#if !defined(KDM_SIZE)

/*COmpilation fail;*/
#error "Error, one make parameter not provided, check your makefile"

/*Define the macro, allows debugging on an IDE;*/
#define KDM_SIZE 700

#endif


/*---------------------------------------------------- Kernel heap -----------------------------------------------------*/

/*The kernel heap head;*/
static struct heap_head *kernel_heap;


/*---------------------------------------------------- Kernel heap -----------------------------------------------------*/

/**
 * kernel_memory_init : initialises the kernel heap;
 *
 * 	Requires the RAM manager to be initialised;
 */

void kdmem_init() {
	
	/*Allocate some memory in the RAM to contain the heap;*/
	void *ram_block = ram_alloc(KDM_SIZE);
	
	/*Create a heap owning the whole RAM block;*/
	kernel_heap = heap_create(ram_block, KDM_SIZE, &heap_fifo_insertion);/*TODO SORTED INSERTION;*/
	
	/*Log;*/
	printk("kernel dynamic memory initialised\n\r");
	
}


/*--------------------------------------------------- Dynamic memory ---------------------------------------------------*/

/**
 * kmalloc : allocates and return a block of memory in the kernel heap;
 *
 * @return the lowest address of the allocated block;
 */

void *kmalloc(size_t size) {
	
	/*If the kernel heap is not initialised*/
	return heap_malloc(kernel_heap, size);
	
}


/**
 * kcalloc : allocates, zero-initialises, and return a block of memory in the kernel heap;
 *
 * @return the lowest address of the allocated block;
 */

void *kcalloc(size_t size) {
	
	/*If the kernel heap is not initialise*/
	return heap_malloc(kernel_heap, size);
	
}


/**
 * kialloc : allocates, initialises and return a block of memory in the kernel heap;
 *
 * @return the lowest address of the allocated block;
 */

void *kialloc(size_t size, const void *init) {
	
	/*If the kernel heap is not initialise*/
	return heap_ialloc(kernel_heap, size, init);
	
}


/**
 * kfree : frees the block of memory referenced by @ptr in the kernel heap;
 *
 * @param ptr : the lowest address of the block's data part;
 */

void kfree(void *ptr) {
	
	/*If the kernel heap is not initialise*/
	heap_free(kernel_heap, ptr);
	
}