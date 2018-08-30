//
// Created by root on 8/21/18.
//

#include "krnl.h"

#include "mem.h"

#include <core/ram.h>
#include <core/core.h>
#include <kernel/scheduler/sched.h>


#define KERNEL_RAM_SIZE 2048
#define KERNEL_STACK_SIZE 512

//-------------------------------------------------- Kernel variables --------------------------------------------------

//The kernel heap head;
static struct prog_mem *kernel_memory;

//The kernel scheduler;
static struct sched_data *kernel_scheduler;

//--------------------------------------------------- First function ---------------------------------------------------

//The kernel's first process function;
extern void __kernel_first_function(void *);


//------------------------------------------------------- Private ------------------------------------------------------

//Initialise the kernel memory and the syscall environment;TODO SYSCALL ENV;
static void kernel_memory_init();

//Initialise the kernel scheduler;
static void kernel_scheduler_init();

//Initialise the execution environment and start the execution;
static void kernel_start_execution();


//----------------------------------------------------- Kernel init ----------------------------------------------------

/**
 * kernel_init : this function is called once, by the core initialisation only. It is the project's entry point.
 */

static void kernel_init() {


	//Initialise the RAM manager;
	ram_init();

	//TODO SET MAIN STACK POINTER TO HIT THE KERNEL STACK;
	//Initialise the kernel program memory;
	kernel_memory_init();


	//TODO INIT FILE SYSTEM;

	//TODO INIT DRIVER MGR;

	//Initialise the kernel scheduler;
	kernel_scheduler_init();

	//Start the process execution;
	kernel_start_execution();

	//This point should never be reached;
	kernel_panic("kernel_init : execution not started;");

}

//TODO KERNEL HALT FOR PANIC
//TODO KERNEL HALT FOR PANIC
//TODO KERNEL HALT FOR PANIC
//TODO KERNEL HALT FOR PANIC
//TODO KERNEL HALT FOR PANIC
//TODO KERNEL HALT FOR PANIC
//TODO KERNEL HALT FOR PANIC
//TODO KERNEL HALT FOR PANIC
//TODO KERNEL HALT FOR PANIC
//TODO KERNEL HALT FOR PANIC
//TODO KERNEL HALT FOR PANIC
//TODO KERNEL HALT FOR PANIC
//TODO KERNEL HALT FOR PANIC
//TODO KERNEL HALT FOR PANIC
//TODO KERNEL HALT FOR PANIC

/**
 * __program_start : the function called by the core library after init;
 */

void __program_start() {

	//Call the kernel initialisation;
	kernel_init();

}


//----------------------------------------------------- Kernel Init ----------------------------------------------------

/**
 * kernel_memory_init : initialises the kernel heap;
 *
 * 	Requires the RAM manager to be initialised;
 */

static void kernel_memory_init() {

	/*
	 * Create the kernel program memory;
	 * The kernel heap is not ready yet, the prog_mem will be self-referential;
	 * The kernel is mono-tasked, with a large heap space;
	 */

	struct prog_mem *km = kernel_memory = prog_mem_create(KERNEL_RAM_SIZE, true);
	prog_mem_create_stacks(km, 1, KERNEL_STACK_SIZE);

}


/**
 * TODO;
 */

static void kernel_scheduler_init() {

	//Create the first process descriptor;
	struct prc_desc dsc = {

		//kernel first function;
		.function = &__kernel_first_function,

		//No arguments;
		.args = 0,
		.args_size = 0,

		//2KiB memory available;
		.ram_size = 2048,

		//One thread;
		.nb_threads = 1,

		//1KiB stack available;
		.stack_size = 1024,

		//10ms activity;
		.activity_time = 10,

	};

	//Create the first process;
	struct prc *first_process = prc_create(&dsc);

	//Initialise the scheduler providing the first process;
	kernel_scheduler = sched_create(first_process);

}


/**
 * kernel_start_execution : TODO;
 */

static void kernel_start_execution() {

	//TODO SYSTICK;

	//TODO UPDATE CORE STACK PROVIDER;


	//Enter thread mode and un-privilege, provide the kernel stack for interrupt handling;
	core_enter_thread_mode(kernel_memory->stacks[0]->sp_reset);

	//TODO PREEMPTION;

}

//-------------------------------------------------- Process Switching -------------------------------------------------


//void kernel
//TODO KERNEL PROCESS SWITCHING
//TODO KERNEL PROCESS SWITCHING
//TODO KERNEL PROCESS SWITCHING
//TODO KERNEL PROCESS SWITCHING
//TODO KERNEL PROCESS SWITCHING
//TODO KERNEL PROCESS SWITCHING
//TODO KERNEL PROCESS SWITCHING
//TODO KERNEL PROCESS SWITCHING


//--------------------------------------------------- Dynamic Memory ---------------------------------------------------

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

void *kialloc(size_t size, const void *init) {

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
