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

#include "panic.h"
#include "kernel/async/interrupt.h"

#include <string.h>

#include <kernel/clock/sysclock.h>

#include <kernel/mem/ram.h>

#include <kernel/scheduler/sched.h>

#include <kernel/proc/proc.h>

#include <kernel/mod/mod.h>
#include <kernel/debug/debug.h>
#include <kernel/fs/inode.h>
#include <kernel/log.h>

#include <kernel/async/preempt.h>


#define KERNEL_RAM_SIZE 7000
#define KERNEL_STACK_SIZE 1000


//--------------------------------------------------- First function ---------------------------------------------------

//The kernel's first process function;
extern void __kernel_first_function(void *unused) {

	fs_list();

	while(1) {

	}

}


//------------------------------------------------------- Private ------------------------------------------------------

//Initialise the kernel scheduler;
static void kernel_scheduler_init();


//----------------------------------------------------- Kernel init ----------------------------------------------------

/**
 * kernel_init : this function is called once, by the core initialisation only. It is the project's entry point.
 */

static void kernel_init() {

	//Log.
	kernel_log_("Entering kernel initialisation sequence;\n")

	//Disable interrupt management:
	exceptions_disable();

	//Initialise the RAM manager;
	ram_init();

	//Initialise the kernel program memory;
	kernel_memory_init();
	
	//Load all proc modules;
	load_proc_modules();
	
	//Load all system modules;
	load_system_modules();
	
	//TODO INIT FILE SYSTEM;
	
	//Load all kernel modules;
	load_peripheral_modules();
	
	//Load all kernel modules;
	load_kernel_modules();
	
	
	//Initialise the system clock;
	sysclock_init();

	//Initialise the kernel scheduler;
	kernel_scheduler_init();
	
	
	//Load all user modules;
	load_user_modules();

	//Start the process execution;
	proc_sta();

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

void __kernel_init() {

	//Call the kernel initialisation;
	kernel_init();

}




//----------------------------------------------------- Kernel Init ----------------------------------------------------


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
	sched_init(first_process);
	


}
