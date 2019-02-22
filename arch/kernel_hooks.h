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
 * Functions declared in this file that can be referenced by different parts of the khal to call the kernel;
 */

#ifndef TRACER_KERNEL_HOOKS_H
#define TRACER_KERNEL_HOOKS_H

/*--------------------------------------------------- kernel hooks ---------------------------------------------------*/

/*The kernel entry point; called by __chip_initialisation; never returns;*/
void __kernel_entry();

/*The kernel tick function; must be called by the khal at each system tick;*/
void __kernel_tick();

/*The kernel context switcher; provides a new stack pointer; must be called by the khal at each preemption;*/
void __kernel_switch_context(void *sp);

/*The kernel fault handler - analyser; must be called by the khal at each fault detection;*/
void __kernel_fault_handler();


#endif /*TRACER_KERNEL_HOOKS_H*/
