/*
  mod.c Part of TRACER

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

#include "mod_hook"

#include <kernel/hard/impl_defined/hard_defined.h>

#include <struct/nlist.h>

#include <panic.h>

#include <kernel/hard/debug/printk.h>




/*--------------------------------------------------- Modules globals --------------------------------------------------*/

#define MOD_NAME_MAX_LENGTH 32

static struct nlist modules = {
	.elements = 0,
	.name_max_length = MOD_NAME_MAX_LENGTH,
};


/*------------------------------------------------- Modules management -------------------------------------------------*/

/**
 * mod_add : creates the module struct, copies the name, adds it to the module list, and executes @init;
 *
 * @param name : the module's name; will be copied;
 * @param init : the module's init function; will be executed;
 * @param exit : the module's deinit function; will be executed when the module will be removed;
 */

void mod_add(const char *name, bool (*init)()) {
	
	/*Call the initialisation function;*/
	(*init)();
	
	/*Add the exit function to the list;*/
	bool success = nlist_add(&modules, name, 0);/*TODO EXIT;*/
	
	/*If the add succeeded :*/
	if (!success) {
		/*TODO EXIT;*/
	}
	
}


/**
 * mod_remove : serches for the required module, and if it exists, removes it and executes its exit function;
 *
 * @param name : the name to search for;
 */

void mod_remove(const char *const name) {
	
	/*Remove the module from the list; TODO cache its exit function;*/
	nlist_remove(&modules, name);
	
	/*
	/*If the module was found and removed, and has a valid exit function :*/
	if (exit) {

		/*Execute it;*/
		bool cleanup = (*exit)();

		/*If the module didn't manage to clean all its resources :*/
		if (!cleanup) {

			/*Reinsert the module, as it is still active;*/
			nlist_add(&modules, name, exit);

			/*TODO LOG;*/

		}

	}*/
	
	
}


static void load_modules(const uint8_t *const mod_start, const uint8_t *const mod_end) {
	
	/*Determine the byte length of the module array;*/
	const size_t module_array_size = mod_end - mod_start;
	
	/*The module array should contain only mod_hook structs. If size validity check fails :*/
	if (module_array_size % sizeof(struct mod_hook)) {
		
		/*Kernel panic, invalid module array size, probably caused by poor linking;*/
		kernel_panic("mod.c : mod_autoload : embedded modules array bounds invalid;");
		
	}
	
	/*Cache the array to the right type;*/
	const struct mod_hook *auto_module = (const struct mod_hook *) mod_start;
	
	/*If no modules are to load :*/
	if ((size_t) auto_module >= (size_t) mod_end) {
		
		/*Log;*/
		printk("No modules to load\n\r");
		
	}
	
	/*For each module :*/
	while ((size_t) auto_module < (size_t) mod_end) {
		
		/*Load the module;*/
		mod_add(auto_module->name, auto_module->init);
		
		/*Log;*/
		printkf("%s module loaded\n\r", auto_module->name);
		
		/*Focus on the next module;*/
		auto_module++;
		
	}
	
}



/**
 * load_x_modules : declare the modules struct bounds, and load all modules inside;
 */

void load_proc_modules() {
	
	/*Log;*/
	printk("Loading proc modules\n\r");
	
	/*Load peripheral modules;*/
	load_modules(&__prmod_min, &__prmod_max);
	
}

void load_system_modules() {
	
	/*Log;*/
	printk("Loading system modules\n\r");
	
	/*Load peripheral modules;*/
	load_modules(&__smod_min, &__smod_max);
	
}


void load_peripheral_modules() {
	
	/*Log;*/
	printk("Loading peripheral modules\n\r");
	
	/*Load peripheral modules;*/
	load_modules(&__pemod_min, &__pemod_max);
	
}


void load_kernel_modules() {
	
	/*Log;*/
	printk("Loading kernel modules\n\r");
	
	/*Load peripheral modules;*/
	load_modules(&__kmod_min, &__kmod_max);
	
}


void load_user_modules() {
	
	/*Log;*/
	printk("Loading user modules\n\r");
	
	/*Load peripheral modules;*/
	load_modules(&__umod_min, &__umod_max);
	
}



