/*
  queue.h Part of nostd

  Copyright (c) 2017 Raphaël Outhier

  nostd is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  nostd is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  aint32_t with nostd.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef NOSTD_QUEUE_H
#define NOSTD_QUEUE_H

#include <stdbool.h>

#include <stddef.h>
#include <error.h>

#include "list.h"


/*
 * This file contains the spec for the queue library;
 *
 *	The queue owns and grants access to memory blocks of a determined size. All blocks managed by the same queue
 *	have the same size;
 *
 * 	All blocks are owned by the queue. Each block can be either :
 * 	- initialised : the block contains valid data and ready to be used; block is inaccessible;
 *  - uninitialised : the block contains no valid data, and can be initialised; block is inaccessible;
 * 	- shared : the block is shared with an external program;
 *
 * 	A function can request access from an uninitialised or initialised piece of data. When it gets access to it,
 * 	it passes in the shared state, and no one can access it;
 *
 * 	When the function has finished its job, it provides the data structure back, and tells the queue its new state.
 * 	Data can be accessed again by another part of the code;
 */


/*The queue status flags register type.*/
#define queue_status_t uint8_t

/*The queue status flags register type.*/
#define queue_error_t uint8_t

struct queue {

    /*Status flags;*/
    volatile uint32_t q_status;

    /*Error flags;*/
    volatile queue_error_t q_error;

    /*The size of a superblock;*/
    size_t q_sbsize;

    /*All data is allocated in a single block;*/
    void *q_mem;

    /*The size of the memory block;*/
    size_t q_mem_size;
    
    /*A shared block header;*/
    void *q_shared;

    /*An initialised block header;*/
    void *q_initialised;

    /*An uninitialised block header;*/
    void *q_uninitialised;

    /*The mutex to protect accesses;*/
    /*TODO mutex_t qmutex;*/
    

};

/*If set, queue_take_safe has returned a null pointer in the past;*/
#define QUEUE_STATUS_TAKE_RETURNED_NULL ((queue_status_t ) (1 << 0))

/*If set, attempted to use a block header outside the queue's memory block;*/
#define QUEUE_ERROR_REDETECTION ((queue_error_t ) (1 << 0))

/*If set, attempted to use a block header outside the queue's memory block;*/
#define QUEUE_ERROR_BLOCK_OUTSIDE_MEMORY ((queue_error_t ) (1 << 1))

/*If set, attempted to use a misaligned block header;*/
#define QUEUE_ERROR_BLOCK_MISALIGNED ((queue_error_t ) (1 << 2));

/*If set, attempted to use a block with an invalid status;*/
#define QUEUE_ERROR_BLOCK_INVALID_STATUS ((queue_error_t ) (1 << 3));

/*If set, attempted to use a block that wasn't in the list corresponding to its status;*/
#define QUEUE_ERROR_BLOCK_NOT_IN_LIST ((queue_error_t ) (1 << 4));

/*If set, an error occurred in function queue_take_safe;*/
#define QUEUE_ERROR_TAKE ((queue_error_t ) (1 << 5));

/*If set, an error occurred in function queue_release_safe;*/
#define QUEUE_ERROR_RELEASE ((queue_error_t ) (1 << 6));



/*Construct a queue, owning [length] elements of [element_size] bytes; concurrency not supported;*/
error_t queue_ctor(struct queue *qu, size_t block_size, size_t nb_blocks);

/*Destruct a queue ; concurrency not supported;*/
error_t queue_dtor(struct queue *qu);

/*Return true if the provided queue's initialised list is not empty;*/
bool queue_initialised_available(const struct queue *qu);

/*Return true if the provided queue's uninitialised list is not empty;*/
bool queue_uninitialised_available(const struct queue *qu);

/*Take the ownership of an element of the queue;*/
void *queue_take_safe(struct queue *qu, bool initialised);

/*Take the ownership of an element of the queue; thread unsafe;*/
void *queue_take_unsafe(struct queue *qu, bool initialised);


/*Release the ownership of an element of the queue;*/
void queue_release_safe(struct queue *qu, void *block, bool initialised);

/*Release the ownership of an element of the queue; thread unsafe;*/
void queue_release_unsafe(struct queue *qu, void *block, bool initialised);

#endif /*NOSTD_QUEUE_H*/
