/*
  panic.c Part of TRACER

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



#include <kernel/common.h>
#include <fstring.h>
#include <kernel/core/debug.h>
#include <kernel/core/except.h>
#include <khal/xcpt.h>


void __kernel_panic(const char *str)  {
	
	__xcpt_disable();

	while (1) {

		/*Transmit the message;;*/
		__printk(str);
		
		/*Wait for one second;*/
		debug_delay_ms((uint32_t) 1000);

	}

}


/**
 * printkf : decodes and displays the provided formatted string over the log output;
 *
 * @param str : the string to display;
 */

void __printk(const char *str) {

	critical_section_enter();

	fdecode(&debug_print_block, 0, str, 0, 0);

	critical_section_leave();


}

/**
 * printkf : decodes and displays the provided formatted string over the log output;
 *
 * @param str : the formatted string;
 * @param args : the array of arguments;
 * @param args_size : the size of the array;
 */

void __printkf(const char *str, const void **args, size_t args_size) {

	critical_section_enter();

	fdecode(&debug_print_block, 0, str, args, args_size);

	critical_section_leave();


}


