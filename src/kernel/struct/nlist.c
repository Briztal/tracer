/*
  nlist.c Part of TRACER

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

#include <kernel/krnl.h>
#include <util/string.h>
#include <string.h>
#include <kernel/debug/debug.h>
#include <kernel/log.h>
#include "nlist.h"


struct nlist_elmt {

	//Modules are linked together;
	struct list_head head;

	//The element's name;
	const char *const name;

	//The data structure referenced by the element;
	void *data;

};


//------------------------------------------------------- Search -------------------------------------------------------


/**
 * mod_find : returns a reference to the content of the element that has the required name, if it exists, 0 if not;
 * @param name : the name to search for;
 */

void *nlist_search(const struct nlist *list, const char *const name) {

	//Cache the first element;
	struct nlist_elmt *const first = list->elements, *element = first;

	//If no elements are registered, stop here;
	if (!first) {
		return 0;
	}

	//For each registered element :
	do {

		//If the element matched :
		if (!strcmp(element->name, name)) {

			//Return the element's reference;
			return element->data;

		}

		//Update the element;
		element = element->head.next;

		//Stop when all elements have been evaluated, or a match was found;
	} while (element != first);

	//Fail, all elements have been evaluated;
	return 0;

}


//-------------------------------------------------- List manipulation -------------------------------------------------

/**
 * mod_add : creates the element struct, copies the name, adds it to the element list, and executes @init;
 *
 * @param name : the element's name; will be copied;
 * @param data : the element's content;
 *
 * @return true if the element has been created. False if not;
 */

bool nlist_add(struct nlist *list, const char *name, void *data) {


	//If another element has this name :
	if (nlist_search(list, name)) {

		kernel_log("\nWarning : nlist_add : file with name [%s] already exists;", name);

		//Do nothing;
		return false;

	}

	//Allocate some memory for the element;
	struct nlist_elmt *elmt = kmalloc(sizeof(struct nlist_elmt));

	//Get the length of the string;
	size_t len = strlen_safe(name, list->name_max_length);

	//Allocate some data for the name;
	char *heap_name = kmalloc(len + 1);

	//Initialise the name;
	strcopy_safe(heap_name, name, len);

	//Create the element initializer;
	struct nlist_elmt elmt_init = {

		//Link to itself;
		.head = {
			.next = elmt,
			.prev = elmt,
		},

		//Transfer the ownership of the heap name;
		.name = heap_name,

		//Save the exit function;
		.data = data,

	};

	//Initialise the element;
	memcpy(elmt, &elmt_init, sizeof(struct nlist_elmt));

	//Link the element to the referenced list;
	list_concat_ref((struct list_head *) elmt, (struct list_head **) &list->elements);

	//Complete;
	return true;


}


/**
 * mod_remove : searches for the required element, and if it exists :
 * 	- removes it from the list;
 * 	- frees its name of the heap;
 * 	- frees its struct;
 *
 * @param name : the name to search for;
 * @return the content of the element, or 0 if no element had this name;
 */

void *nlist_remove(struct nlist *list, const char *name) {

	//Search for an element with that name;
	struct nlist_elmt *element = nlist_search(list, name);

	//If the element is null, stop here;
	if (!element)
		return 0;

	//The element exists, we can delete it;

	//Remove the element from the elements list;
	list_remove_ref_next((struct list_head *) element, (struct list_head **) list->elements);

	//Cache the element's content;;
	void *data = element->data;

	//Delete the element's heap name;
	kfree((void *) element->name);

	//Delete the element struct;
	kfree(element);

	//Return the content;
	return data;

}

//---------------------------------------------------- List cleanup ----------------------------------------------------


/**
 * nlist_clear : deletes each element, and its content, thanks to the deletion function passed in argument;
 * @param list : the named list to clear;
 * @param deleter : the function in charge to delete each element's content;
 */

void nlist_clear(struct nlist *list, void (*deleter)(void *)) {

	//Cache the first element;
	struct nlist_elmt *const first = list->elements, *element = first;

	//If no elements are registered, stop here;
	if (!first) {
		return;
	}

	//For each element :
	do {

		//Cache the successor;
		struct nlist_elmt *next = element->head.next;

		//Delete the element's content;
		(*deleter)(element->data);

		//Delete the element's heap name;
		kfree((void *) element->name);

		//Delete the element struct;
		kfree(element);

		//Update the element;
		element = next;

		//Stop when all elements have been evaluated, or a match was found;
	} while (element != first);

	//Reset the list reference;
	list->elements = 0;

}



//List all files;
void nlist_list(struct nlist *list) {

	//Cache the current file, as the first file;
	struct nlist_elmt *const first = list->elements, *file = first;

	//If there are no files :
	if (!file) {
		kernel_log_("no files");
		return;
	}

	//For each file :
	do {

		kernel_log_(file->name);


		//If not, focus on the next file;
		file = file->head.next;

	} while (file != first);

}
