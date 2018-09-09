/*
  type.h Part of TRACER

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

#include "type.h"



/**
 * list_concat_ref : concatenates @h0 and the list head referenced by h1.
 * 	If h1 is null, h1 will now reference h0;
 *
 * @param h0 : the list head to concat on the left;
 * @param h1 : the reference to the list head to concat on the right;
 */

void list_concat_ref(struct list_head *h0, struct list_head **h1_p) {

	//Dereference h1_p;
	struct list_head *h1 = *h1_p;

	//If h1 is null pointer :
	if (!h1) {

		//@h0 becomes referenced;
		*h1_p = h0;

	} else {

		//If *h1 exists, concatenate both lists and do not change h1;
		list_concat(h0, h1);

	}

}


/**
 * list_remove_ref : when we must remove an element from a list with a particular element referenced, these functions
 * 	can be used;
 *
 * 	It updates @ref to 0 when @head is the only element of the list, or to @head's successor or predecessor;
 */

void list_remove_ref_next(struct list_head *head, struct list_head **ref) {

	//If @head is referenced, @head may be the only element of the list; :
	if (*ref == head) {

		//Cache @head's successor;
		struct list_head *next = head->next;

		//If head effectively has a successor :
		if (head != next) {

			//The referenced element becomes the successor;
			*ref = next;

		} else {

			//If @head has no successor, the reference becomes null;
			*ref = 0;
		}

	}

	//Remove @head from its list;
	list_remove(head);

}

void list_remove_ref_prev(struct list_head *head, struct list_head **ref) {

	//If @head is referenced, @head may be the only element of the list; :
	if (*ref == head) {

		//Cache @head's predecessor;
		struct list_head *prev = head->prev;

		//If head effectively has a predecessor :
		if (head != prev) {

			//The referenced element becomes the predecessor;
			*ref = prev;

		} else {

			//If @head has no predecessor, the reference becomes null;
			*ref = 0;
		}

	}

	//Remove @head from its list;
	list_remove(head);

}

