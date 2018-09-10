/*
  nlist.h Part of TRACER

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

//Print all names in the list;
void nlist_list(struct nlist *list);


#endif //TRACER_NLIST_H
