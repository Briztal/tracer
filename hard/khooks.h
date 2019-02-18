/*
  khooks.h Part of TRACER

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

/*
 * This file declares all function that are accessible to the hardware implementation;
 */

#ifndef TRACER_HOOK_H
#define TRACER_HOOK_H

//TODO DOC
//TODO DOC
//TODO DOC
//TODO DOC
//TODO DOC
//TODO DOC


void __proc_init();


void __chip_init();


void __krnl_tick();


void __krnl_switch_context(void *sp);


/*
 * The kernel can handle faults. A fault will place the kernel in a safe state, where it can analyse the fault,
 * 	and decide what to do.
 *
 * 	kernel_fault_handler shall be called whenever the fault is executed.
 *
 * 	The kernel have three hardware related functions that implement fault management :
 *
 * 	__flt_update_env caches fault related registers; can't fail;
 *
 * 	__flt_repair analyses the fault and triggers kernel actions, to repair the fault; can fail;
 *
 * 	__flt_log displays a log message describing the fault that has happened;
 *
 *
 * 	When a fault occurs, the kernel calls __flt_update_env and __flt_repair;
 *
 * 	If the repairing succeeds, it stops and let the execution continue; if not, it calls __flt_log and stalls
 * 	indefinitely;
 */

void kernel_fault_handler();




#endif //TRACER_HOOK_H
