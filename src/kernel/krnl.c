/*
  krnl.c Part of TRACER

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

#include "krnl.h"
#include "panic.h"
#include "ic.h"

#include <string.h>

#include <kernel/sysclock.h>

#include <kernel/mem/ram.h>

#include <kernel/scheduler/sched.h>

#include <kernel/proc.h>

#include <kernel/mod/mod.h>
#include <kernel/debug/debug.h>
#include <kernel/fs/dfs.h>
#include <kernel/log.h>


#define KERNEL_RAM_SIZE 2048
#define KERNEL_STACK_SIZE 512

//-------------------------------------------------- Kernel variables --------------------------------------------------

//The kernel heap head;
static struct prog_mem *kernel_memory;

//The kernel scheduler;
static struct sched_data *kernel_scheduler;

//The current process in execution;
static struct prc *current_process;


//The remaining number of threads that must stop before the next process gets executed; protected by the spinlock below;
static uint8_t nb_active_threads;

//The premption spin lock;TODO MULTITHREADING PATCH;
//static core_spin_lock *active_lock;


void kernel_private() {

}


//--------------------------------------------------- First function ---------------------------------------------------

//The kernel's first process function;
extern void __kernel_first_function(void *unused) {


	while(1) {
		//TODO debug_log("BITe");
		dfs_list();
		debug_delay_ms(500);
	}

}

extern void __create_drivers();


//------------------------------------------------------- Private ------------------------------------------------------

//Initialise the kernel memory and the syscall environment;TODO SYSCALL ENV;
static void kernel_memory_init();

//Initialise the kernel scheduler;
static void kernel_scheduler_init();

//Initialise the execution environment and start the execution;
static void kernel_start_execution();


static void kernel_syscall_handler() {
	//TODO HANDLE SYSCALLS;
}


//----------------------------------------------------- Kernel init ----------------------------------------------------

/**
 * kernel_init : this function is called once, by the core initialisation only. It is the project's entry point.
 */

static void kernel_init() {

	//Log.
	kernel_log_("Entering kernel initialisation sequence;\n")

	//Disable interrupt management:
	ic_disable_interrupts();

	//Initialise the RAM manager;
	ram_init();

	//Initialise the kernel program memory;
	kernel_memory_init();

	//Initialise the system clock;
	sysclock_init();

	//TODO INIT FILE SYSTEM;

	//TODO INIT DRIVER MGR;

	//Initialise the kernel scheduler;
	kernel_scheduler_init();

	//Initialise embedded drivers;
	mod_autoload();


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

	kernel_log_("kernel memory initialised, creating kernel stacks ...");

	prog_mem_create_stacks(km, 1, KERNEL_STACK_SIZE);

	//Log;
	kernel_log("\t%d kernel threads created", 1);

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

		//10ms activity;TODO
		.activity_time = 3000,

	};

	//Create the first process;
	struct prc *first_process = prc_create(&dsc);


	//Initialise the scheduler providing the first process;
	kernel_scheduler = sched_create(first_process);

	/*
	 * When the first preemption will happen, the preempted environment will be invalid, as it does not concern
	 * any process, but the start stack environment;
	 *
	 * To work properly, the preemption needs @current process to hit a valid memory zone, but it can't concern
	 * a real process. A blank process will be created to contain this environment;
	 *
	 * The blank process needs to contain references to valid program memory and core stacks;
	 * Those structs also need to be created;
	 *
	 * As core stacks are not contained in the process struct, we also need to create some.
	 * As those will never be queried, used or again, we will use N time the same proc_stack struct;
	 */

	static struct prc blank_process;
	static struct prog_mem blank_mem;
	static struct proc_stack blank_stack;


	//Cache the number of threads;
	const uint8_t nb_stks = proc_nb_threads;


	//Create the blank mem initializer;
 	struct prog_mem blank_mem_init =  {
		.heap = 0,
		.nb_stacks = nb_stks,
	};

	//Initialize the blank prog mem;
	memcpy(&blank_mem, &blank_mem_init, sizeof(struct prog_mem));

	//For each stack ref :
	for (uint8_t stack_id = nb_stks; stack_id--;) {

		//Redirect to blank stack;
		blank_mem.stacks[stack_id] = &blank_stack;

	}

	//Reference the blank prog mem;
	blank_process = (struct prc)  {
		.prog_mem = &blank_mem,
	};

	//Initialise the current process;
	current_process = &blank_process;

	kernel_log_("scheduler initialised");

}


/**
 * kernel_start_execution : TODO;
 */

static void kernel_start_execution() {

	/*
	 * Thread count;
	 */

	//Initialise the number of active threads;
	nb_active_threads = proc_nb_threads;


	/*
	 * Preemption;
	 */

	//Update the preemption handler;
	preemption_set_handler(&proc_context_switcher);

	//Update the preemption exception priority;
	preemption_set_priority(KERNEL_PREMPTION_PRIORITY);

	//Enable the preemption exception;
	preemption_enable();

	//Log;
	kernel_log_("preemption initialised");



	/*
	 * Syscalls;
	 */

	/*
	//Set the syscall handler; Same prio as preemption;
	syscall_set_handler(kernel_syscall_handler);

	//Set the syscall exception priority to the lowest possible;
	syscall_set_priority(KERNEL_PREMPTION_PRIORITY);

	//Enable the syscall exception;
	syscall_enable();
	 */


	/*
	 * Start execution;
	 */

	//Log;
	kernel_log_("\nKernel initialisation sequence complete. Entering thread mode ...\n");


	//Enter thread mode and un-privilege, provide the kernel stack for interrupt handling;
	//Interrupts will be enabled at the end of the function;
	proc_enter_thread_mode(kernel_memory->stacks);

}


//-------------------------------------------------- Process Switching -------------------------------------------------

/**
 * kernel_get_new_stack : called by threads to get a new stack pointer, providing the current one, and the index of the
 * 	thread;
 *
 * @param thread_id : the thread's index;
 * @param sp : the previous stack pointer;
 * @return the the new stack pointer;
 */

extern bool prc_process_terminated;


void *kernel_switch_thread_stack(const volatile uint8_t thread_id, void *volatile sp) {

	//In order to know when new stack pointers can be safely queried, a static variable will be used;
	static volatile bool process_updated;

	//As the process can be updated when all threads are stopped, the flag can be cleared when a threads enters;
	process_updated = false;

	//Save the stack pointer;
	current_process->prog_mem->stacks[thread_id]->sp = sp;

	//Lock TODO MULTITHREAD PATCH;
	//spin_lock(sp);

	//Decrement the active threads counter; if it was null before :
	if (!(nb_active_threads--)) {

		//Error;
		kernel_panic("krnl.c : kernel_switch_thread_stack : more entries than existing threads;");

	}

	//If no more threads have to stop :
	if (!nb_active_threads) {

		//TODO IN SYSCALL;
		if (prc_process_terminated) {

			//Terminate the current process;
			sched_terminate_prc(kernel_scheduler);

			//Clear termination flag;
			prc_process_terminated = false;

		}

		//Commit changes to the scheduler;
		scheduler_commit(kernel_scheduler);

		//Get the first process;
		current_process = sched_get(kernel_scheduler);

		//Update the duration until next preemption;
		sysclock_set_process_duration(current_process->desc.activity_time);

		//Update the number of active threads;
		nb_active_threads = proc_nb_threads;

		//Update the flag;
		process_updated = true;

	}

	//Unlock TODO MULTITHREAD PATCH;
	//spin_unlock();


	/*
	 * The stop position. All threads except the last will enter and loop here, until the last updates the current
	 * process;
	 */

	while(!process_updated);

	//Return the appropriate stack pointer;
	return current_process->prog_mem->stacks[thread_id]->sp;

}



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