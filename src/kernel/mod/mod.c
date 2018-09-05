//
// Created by root on 9/4/18.
//

#include "mod.h"


#include <util/type.h>

#include <kernel/krnl.h>

#include <util/string.h>
#include <string.h>


//---------------------------------------------------- Module struct ---------------------------------------------------

struct mod {

	//Modules are linked together;
	struct list_head head;

	//The module's name;
	const char *const name;

	//The module's entry function;
	void (*const init)();

	//The module's exit function;
	void (*const exit)();

};


//--------------------------------------------------- Modules globals --------------------------------------------------

#define MOD_NAME_MAX_LENGTH 32

//The modules list reference;
static struct mod *modules;


//---------------------------------------------------- Module search ---------------------------------------------------


/**
 * mod_find : returns a reference to the module that has the required name, if it exists, 0 if not;
 * @param name : the name to search for;
 */

static struct mod *mod_find(const char *const name) {

	//Cache the first module;
	struct mod *const first = modules, *module = first;

	//If no modules are registered, stop here;
	if (!first) {
		return 0;
	}

	//For each registered module :
	do {

		//If the module matched :
		if (!strcmp(module->name, name)) {

			//Return the module's reference;
			return module;

		}

		//Stop when all modules have been evaluated, or a match was found;
	} while (module != first);

	//Fail, all modules have been evaluated;
	return 0;

}


//------------------------------------------------- Modules management -------------------------------------------------

/**
 * mod_add : creates the module struct, copies the name, adds it to the module list, and executes @init;
 *
 * @param name : the module's name; will be copied;
 * @param init : the module's init function; will be executed;
 * @param exit : the module's deinit function; will be executed when the module will be removed;
 */

void mod_add(const char *name, void (*init)(), void (*exit)) {


	//If another module has this name :
	if (mod_find(name)) {

		//Do nothing;
		return;

	}

	//Allocate some memory for the module;
	struct mod *module = kmalloc(sizeof(struct mod));

	//Get the length of the string;
	size_t len = strlen_safe(name, MOD_NAME_MAX_LENGTH);

	//Allocate some data for the name;
	char *heap_name = kmalloc(len+1);

	//Initialise the name;
	strcopy_safe(heap_name, name, len);

	//Create the module initializer;
	struct mod module_init = {

		//Link to itself;
		.head = {
			.next = module,
			.prev = module,
		},

		//Transfer the ownership of the heap name;
		.name = heap_name,

		//Save the exit function;
		.exit = exit,

	};

	//Initialise the module;
	memcpy(module, &module_init, sizeof(struct mod));

	//Link the module to the referenced list;
	list_concat_ref((struct list_head *) module, (struct list_head **) modules);

	//Call the initialisation function;
	(*init)();

}


/**
 * mod_remove : serches for the required module, and if it exists :
 * 	- removes it from the list;
 * 	- frees its name of the heap;
 * 	- frees its struct;
 *
 * @param name : the name to search for;
 */

void mod_remove(const char *name) {

	struct mod *module = mod_find(name);

	//If the module is null, stop here;
	if (!module)
		return;

	//The module exists, we can delete it;

	//Remove the module from the modules list;
	list_remove_ref_next((struct list_head *) module, (struct list_head **) modules);

	//Call the module's exit function;
	(*(module->exit))();

	//Delete the module's heap name;
	kfree((void *) module->name);

	//Delete the module struct;
	kfree(module);

}


//Load all automatically loadable modules;
void mod_autoload() {



}
