//
// Created by root on 9/5/18.
//

#ifndef TRACER_NLIST_H
#define TRACER_NLIST_H

#include <util/type.h>


struct nlist_elmt;

/*
 * The named list is a linked list whose elements contain a name, that can be used to search them in the list;
 */

struct nlist {

	//The maximal length of all elements names;
	const size_t name_max_length;

	//The elements list reference;
	struct nlist_elmt *elements;

};



//Add an element in the list;
bool nlist_add(struct nlist *list, const char *name, void *data);

//Search the list for an element;
void *nlist_search(const struct nlist *list, const char *name);

//Remove an element from the list and return it;
void *nlist_remove(struct nlist *list, const char *name);

//Clear the list; A function must be provided to delete properly all elements;
void nlist_clear(struct nlist *list, void (*deleter)(void *));


#endif //TRACER_NLIST_H
