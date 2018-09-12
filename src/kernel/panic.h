/*
  panic.h Part of TRACER

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

#ifndef TRACER_ERROR_H
#define TRACER_ERROR_H

#include <stddef.h>

//------------------------------------------------------- Config -------------------------------------------------------

//Uncomment to disable kernel panic log;
//#define DISABLE_PANIC_LOG


//-------------------------------------------------------- Panic -------------------------------------------------------

//If panic logs are enabled :
#ifndef DISABLE_PANIC_LOG


//Declare the function that will trigger a kernel panic, and displaying a message related to the error;
void _kernel_panic(const char *msg);

//Use panic with log;
#define kernel_panic(msg) _kernel_panic(msg)


//If panic logs are disabled :
#else


//Declare a special function that will trigger a kernel panic, displaying a default message;
void _kernel_panic_nolog();

//Use panic with no logs;
#define kernel_panic(msg) _kernel_panic_nolog()


#endif


#endif //TRACER_ERROR_H
