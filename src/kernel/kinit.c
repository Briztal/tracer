/*
  kinit.c Part of TRACER

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

#include <mem/kdmem.h>

#include <debug/printk.h>

#include <async/except.h>

#include <clock/sysclock.h>

#include <mem/ram.h>

#include <mod/mod.h>

#include <run/proc.h>



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



//Start the kernel. Different modes can be entered, depending on compilation settings;
static void kernel_start();


//----------------------------------------------------- Kernel init ----------------------------------------------------

/**
 * kernel_init : this function is called once, by the core initialisation only. It is the project's entry point.
 */

void __krnl_init() {
	
	//Create an OTP flag;
	static bool reentrance = false;
	
	//If we enter for the second time :
	if (reentrance) {
		
		//Panic; Kernel must be initialised only once;
		kernel_panic("kernel_init : attempted to initialise a second time;");
		
	}
	
	//Set the OTP;
	reentrance = true;
	
	//Log.
	printk("Entering kernel initialisation sequence;\n")
	
	//Disable interrupt management:
	exceptions_disable();
	
	
	//Initialise the RAM manager;
	ram_init();
	
	//Initialise the kernel dynamic memory;
	kdmem_init();
	
	//Load all run modules;
	load_proc_modules();
	
	//Load all system modules;
	load_system_modules();
	
	//TODO INIT FILE SYSTEM;
	
	//Load all kernel modules;
	load_peripheral_modules();
	
	//Load all kernel modules;
	load_kernel_modules();
	
	//Load all user modules;
	load_user_modules();
	
	
	//Start the system clock;
	sysclock_start();
	
	//Start the execution;
	kernel_start();
	
	//This point should never be reached;
	kernel_panic("kernel_init : execution not started;");
	
}



/**
 * kernel_start : determines the mode to enter, and enters in it;
 */

static void kernel_start() {
	
	//Start the execution of the scheduler's first process, via the processor library;
	proc_start_execution();
	
	kernel_panic("execution mode failed to execute or returned;");
	
}
