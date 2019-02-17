/*
  except.h Part of TRACER

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

#ifndef TRACER_IC_H
#define TRACER_IC_H

#include <stdbool.h>

#include <stdint.h>

#include <kernel/hard/arch/xcpt.h>


/*
 * In the kernel, some priority levels are used for particular exceptions and interrupts;
 */

//The preemption has the lowest priority level, so that a context switch never occurs over an interrupt;
#define KERNEL_PREEMPTION_PRIORITY __xcpt_priority_0

//A if status interrupt has a low priority;
#define KERNEL_DRIVER_STATUS_PRIORITY __xcpt_priority_2

//A if error must be caught, and so has a higher priority;
#define KERNEL_DRIVER_ERROR_PRIORITY __xcpt_priority_4

//The systick has the highest priority level, so that ticks are never missed;
#define KERNEL_SYSTICK_PRIORITY __xcpt_priority_6


//Enter a critical section;
void critical_section_enter();

//Leave a critical section;
void critical_section_leave();

//Force leaving a critical section. Unsafe, must be executed by the kernel only;
void critical_section_force_exit();


#endif //TRACER_IC_H
