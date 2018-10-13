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


#include <kernel/debug/debug.h>
#include <util/stdout.h>
#include "kernel/async/interrupt.h"

static void (*log_output) = &debug_print_char;


void _kernel_panic(const char *str, const void **args, size_t args_size)  {
	
	exceptions_disable();

	while (1) {

		//Transmit the message;;
		stdout(log_output, str, args, args_size);
		
		//Wait for one second;
		debug_delay_ms((uint32_t) 1000);

	}

}


//If panic logs are disabled :
#ifdef DISABLE_PANIC_LOG

//Trigger a kernel panic, displaying a default message;
void _kernel_panic_nolog() {

	//Display the default error message;
	_kernel_panic("A kernel panic has occurred. As log has been disabled at compile time, you are fucked.\n"
					  "Try to re-enable them, recompile and reproduce error conditions.\n"
					  "Good luck pal !");

}

#endif