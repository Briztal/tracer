/*
  nvic.c Part of TRACER

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


#include <stdbool.h>
#include <kernel/debug/log.h>
#include <kernel/mod/auto_mod.h>

#include "arm_v7m.h"

//Declare the kernel vtable;
extern void (*__kernel_vtable[])(void);

//TODO KERNEL_VTABLE IN HARD.H


/**
 * nvic_relocation : sets the kernel vector table as the nvic vector table;
 * @return true;
 */

bool nvic_relocation() {
	
	//Check that the kernel vtable is properly aligned; If not :
	if ((uint32_t)__kernel_vtable & 511) {
		
		//Log;
		kernel_log_("nvic_relocation : the kernel vtable is not properly aligned. Aborting.");
		
		//Complete;
		return true;
		
	}

	//Relocate the vector table;
	*ARMV7_VTOR = (uint32_t) __kernel_vtable;
	
	//Complete;
	return true;

}


//Register the nvic relocation module;
KERNEL_EMBED_MODULE(PROC_MODULE, nvic_reloc, &nvic_relocation);