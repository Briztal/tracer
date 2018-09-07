/*
  shared_fifo.c Part of TRACER

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

#include "shared_fifo.h"

#include <kernel/core.h>


/**
 * shared_fifo_push : concatenate @head to the end of the list of @fifo; @head will appear in the pending list;
 * @param fifo : the fifo to push into;
 * @param element : the head of the list to push;
 */

void shared_fifo_push(volatile struct shared_fifo *fifo, struct list_head *head) {

	//Enter in a critical section;
	ic_enter_critical_section();

	//Cache the fifo's list;
	struct list_head *const list = fifo->list;

	//If the list doesn't exist (empty fifo) :
	if (!list) {

		//Head becomes the list;
		fifo->list = head;

	} else {

		//If the fifo is not empty :

		//Concatenate both lists
		list_add(head, fifo->list);

		//fifo->list references the element to pull. No modification is required;

	}

	//Leave the critical section;
	ic_leave_critical_section();

}


/**
 * shared_fifo_pull : returns the first element of the list of @fifo, or 0 if it is empty;
 * @param fifo : the list to pull from;
 * @return the first element if fifo, or 0 if it is empty;
 */

struct list_head *shared_fifo_pull(volatile struct shared_fifo *fifo) {

	//Enter in a critical section;
	ic_enter_critical_section();

	//Cache the fifo's head. We will return this value :
	struct list_head *const head = fifo->list;

	//If the fifo has elements :
	if (head) {

		//Cache the successor of head;
		struct list_head *next = head->next;

		//If the fifo contains one element :
		if (next == head) {

			//Nullify next;
			next = 0;

		} else {

			//If the fifo does contain more than one element, remove head from it;
			list_remove(head);

		}

		//Update the fifo's list;
		fifo->list = next;

	}

	//Leave the critical section;
	ic_leave_critical_section();

	//Return the removed value;
	return head;

}



/**
 * shared_fifo_get_all : returns the first element, can be 0, and resets the cached element to 0;
 */

struct list_head *shared_fifo_get_all(volatile struct shared_fifo *fifo) {

	//Enter in a critical section;
	ic_enter_critical_section();

	//Cache the fifo's head;
	struct list_head *head = fifo->list;

	//Reset the fifo's head;
	fifo->list = 0;

	//Leave the critical section;
	ic_leave_critical_section();

	//Return the linked list head;
	return head;

}