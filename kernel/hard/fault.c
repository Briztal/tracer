/*
  fault.c Part of TRACER

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


#include <kernel/hard/except.h>
#include <kernel/res/printk.h>


//Place the kernel in a safe state, call the fault analyser, and recover, if possible. If not, panic;
void kernel_fault_handler() {

    bool repaired;

    //Enter a critical section;
    critical_section_enter();

    //Update the fault environment;
    __flt_update_env();

    //Attempt to repair the fault;
    repaired = __flt_repair();

    //If the fault is repaired, complete here;
    if (repaired) return;

    //If the fault is not repaired, log;
    printk("Non recoverable fault detected. Executing the fault log function...");

    //Execute the fault logger;
    __flt_log();

    //Stop here;
    while (1);

}
