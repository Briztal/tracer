//
// Created by root on 8/21/18.
//

#include "krnl.h"

#include "mem.h"

#define KERNEL_RAM_SIZE 2048
#define KERNEL_STACK_SIZE 512

//------------------------------------------------------- Private ------------------------------------------------------

//The kernel heap head;
static struct prog_mem *kernel_memory;

//Initialise the memory struct;
static void kernel_memory_init();


//----------------------------------------------------- Kernel init ----------------------------------------------------

/**
 * kernel_init : this function is called once, by the core initialisation only. It is the project's entry point.
 */

static void kernel_init() {

	//TODO INIT ERROR MANAGER;

	//Initialise the RAM manager;
	ram_init();

	//Initialise the kernel program memory;
	kernel_memory_init();

	//TODO INIT FILE SYSTEM;

	//TODO INIT DRIVER MGR;

	//Start the scheduler;
	//scheduler_start(kernel_init_function);

	//TODO ERROR LOG;
	while(1);


}


/**
 * __program_start : the function called by the core library after init;
 */

void __program_start() {

	//Call the kernel initialisation;
	kernel_init();

}


/*
 * kernel_panic : this function is called when a fatal error is detected in the kernel;
 */

void kernel_panic(const char *error_message) {

	//Stop the scheduler;
	//scheduler_stop();

	//Exit all critical sections;
	//ic_force_critical_section_exit();

	//TODO RESET IDLE INTERRUPT HANDLERS

	//TODO THREAD MODES;

	/*
	if (error_message_function) {

		//Transmit the message;
		(*error_message_function)(error_message);

	}
	 */

	//infinite SOS sequence on debug led;
	debug_sos();

	while (1);

}



//----------------------------------------------------- Kernel Heap ----------------------------------------------------

/**
 * kernel_memory->heap_init : initialises the kernel heap;
 *
 * 	Requires the RAM manager to be initialised;
 */

static void kernel_memory_init() {

	/*
	 * Create the kernel program memory;
	 * The kernel is mono-tasked, with a large heap space;
	 */
	kernel_memory = prog_mem_create(KERNEL_RAM_SIZE, 1, KERNEL_STACK_SIZE);

}


/**
 * kmalloc : allocates and return a block of memory in the kernel heap;
 *
 * @return the lowest address of the allocated block;
 */

void *kmalloc(size_t size) {

	//If the kernel heap is not initialise
	return heap_malloc(kernel_memory->heap, size);

}


/**
 * kcalloc : allocates, zero-initialises, and return a block of memory in the kernel heap;
 *
 * @return the lowest address of the allocated block;
 */

void *kcalloc(size_t size) {

	//If the kernel heap is not initialise
	return heap_malloc(kernel_memory->heap, size);

}


/**
 * kialloc : allocates, initialises and return a block of memory in the kernel heap;
 *
 * @return the lowest address of the allocated block;
 */

void *kialloc(size_t size, void *init) {

	//If the kernel heap is not initialise
	return heap_ialloc(kernel_memory->heap, size, init);

}


/**
 * kfree : frees the block of memory referenced by @ptr in the kernel heap;
 *
 * @param ptr : the lowest address of the block's data part;
 */

void kfree(void *ptr) {

	//If the kernel heap is not initialise
	heap_free(kernel_memory->heap, ptr);

}
