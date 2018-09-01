//
// Created by root on 8/7/18.
//

#ifndef TRACER_SHARED_FIFO_H
#define TRACER_SHARED_FIFO_H


#include <stdint.h>

#include <core/type/list.h>

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