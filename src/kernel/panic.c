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

#include "panic.h"

#include <debug/debug.h>

#include <async/except.h>
#include <debug/printk.h>


void kernel_panic(const char *str)  {
	
	exceptions_disable();

	while (1) {

		//Transmit the message;;
		printk(str);
		
		//Wait for one second;
		debug_delay_ms((uint32_t) 1000);

	}

}
