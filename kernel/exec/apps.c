/*
  apps.c Part of TRACER

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

#ifndef TRACER_APPS_H
#define TRACER_APPS_H


#include <stdint.h>

#include <stddef.h>

#include <std/app_hook.h>

#include <kernel/panic.h>

#include <kernel/hard/debug/printk.h>

#include "sched.h"


void load_applications(void *args, size_t size) {
	
	extern const uint8_t __apps_min, __apps_max;
	
	#define APPS_MIN &__apps_min
	#define APPS_MAX &__apps_max
	
	printkf("apps bounds : %h %h\n\r", APPS_MAX, APPS_MIN)
	
	//Determine the byte length of the module array;
	const size_t apps_array_size = APPS_MAX - APPS_MIN;
	
	//The module array should contain only auto_mod structs. If size validity check fails :
	if (apps_array_size % sizeof(struct app_hook)) {
		
		//Kernel panic, invalid module array size, probably caused by poor linking;
		kernel_panic("apps.c : load_applications : apps array bounds invalid;");
		
	}
	
	//Cache the array to the right type;
	const struct app_hook *application = (const struct app_hook *) APPS_MIN;
	
	//If no modules are to load :
	if ((size_t) application >= (size_t) APPS_MAX) {
		
		//Log;
		kernel_panic("\tNo applications to load.");
		
	}
	
	//For each module :
	while ((size_t) application < (size_t) APPS_MAX) {
		
		//Create the application's first process;
		sched_create_prc(application->desc, application->req);
		
		//Log;
		printkf("\tapp %s loaded\n\r", application->name);
		
		//Focus on the next module;
		application++;
		
	}
	
}


#endif //TRACER_APPS_H
