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

#include "kernel/common.h"

#include <kernel/res/kdmem.h>


#include <kernel/hard/except.h>

#include <kernel/exec/sysclock.h>

#include <kernel/hard/ram.h>

#include <kernel/exec/mod.h>

#include <kernel/exec/proc.h>

#include "kernel/hard/debug.h"

#include "kernel/printk.h"



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



//----------------------------------------------------- Kernel init ----------------------------------------------------

/**
 * kernel_init : this function is called once, by the core initialisation only. It is the project's entry point.
 */

void __krnl_init() {

    __dbg_delay_ms(1000);

	//Create an OTP flag;
	static volatile bool reentrance = false;
	
	//If we enter for the second time :
	if (reentrance) {
		
		//Panic; Kernel must be initialised only once;
		kernel_panic("kernel_init : attempted to initialise a second time;");
		
	}
	
	//Set the OTP;
	reentrance = true;


    //Log.
	printk("Entering kernel initialisation sequence;\n");


	/*
	 * Kernel hardware components;
	 */

    //Initialise the exceptions manager;
	exceptions_init();

	//Initialise the RAM manager;
	ram_init();


    /*
	 * Kernel resources management components init;
	 */

    //TODO SYMTABLE INIT;
    //TODO FS INIT;
    //TODO STD_PERIPH INIT;
    //TODO CLOCK MANAGER INIT;
    //TODO DYNAMIC MEMORY INIT ?;
    //TODO COPROC MGR INIT ?

	/*
	 * Modules init;
	 */

	//Load all run modules;
	load_proc_modules();
	
	//Load all system modules;
	load_system_modules();

	
	//Load all kernel modules;
	load_peripheral_modules();
	
	//Load all kernel modules;
	load_kernel_modules();
	
	//Load all user modules;
	load_user_modules();


	/*
	 * Kernel execution components;
	 */

	//Start the execution of the scheduler's first process, via the processor library;
	proc_start_execution();
	
	//This point should never be reached;
	kernel_panic("kernel_init : execution not started;");
	
}


