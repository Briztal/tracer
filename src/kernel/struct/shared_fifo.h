/*
  shared_fifo.h Part of TRACER

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

#ifndef TRACER_SHARED_FIFO_H
#define TRACER_SHARED_FIFO_H


#include <stdint.h>



#include <list.h>

/*
 * The shared fifo encapsulates a list, and wraps concerned functions in critical sections;
 */

struct shared_fifo {

	//The list that will encapsulate the fifo;
	struct list_head *list;

};



//Push @element in @fifo;
void shared_fifo_push(volatile struct shared_fifo *fifo, struct list_head *element);

//Pull and return and element from @fifo. 0 returned if no element available;
struct list_head *shared_fifo_pull(volatile struct shared_fifo *fifo);

//Get the whole linked list; List will be empty after;
struct list_head *shared_fifo_get_all(volatile struct shared_fifo *fifo);


/**
 * shared_fifo_empty : Asserts if pull is locked or if no elements can be pulled of @fifo;
 * @fifo : the fifo to examine;
 *
 * Purely indicative;
 */

static inline bool shared_fifo_empty(const volatile struct shared_fifo *fifo) {
	return fifo->list == 0;
}

#endif //TRACER_SHARED_FIFO_H