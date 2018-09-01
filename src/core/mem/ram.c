/*
  ram.c Part of TRACER

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


#include <stdint.h>

#include "ram.h"

#include "heap.h"
#include "core/core.h"

#include <core/debug.h>

//------------------------------------------------------- Externs ------------------------------------------------------

extern uint32_t _ram_lowest;
extern uint32_t _ram_highest;


//------------------------------------------------------- Globals ------------------------------------------------------

//The heap that will manage the ram block;
static struct heap_head *ram_heap = 0;


//------------------------------------------------------- RAM mgt ------------------------------------------------------

/**
 * ram_init : resets the heap that manages the RAM block.
 *
 * 	Executing this function will invalidate all allocated memory in the ram.
 *
 * 	It must be ran carefully;
 */

void ram_init() {

	//Initialise the heap;
	ram_heap = heap_create(&_ram_lowest, &_ram_highest - &_ram_lowest, &heap_fifo_insertion);

}

/**
 * ram_alloc : allocate a memory block in the RAM;
 *
 * @param size : the size of the memory block to allocate;
 *
 * @return the lowest address of the allocated block;
 */

void *ram_alloc(size_t size) {

	//Cache the heap;
	struct heap_head *heap = ram_heap;

	//If the heap hasn't been initialised :
	if (!heap) {

		//Core panic;
		core_error("ram.c : ram_alloc : ram heap not initialised;");
		return 0;

	}

	//Allocate and return some data in the heap;
	return heap_malloc(heap, size);

}


/**
 * ram_free : frees an allocated memory block, referenced by its lowest data address;
 *
 * @param ptr : the lowest address of the data block;
 */

void ram_free(void *ptr) {

	//Cache the heap;
	struct heap_head *heap = ram_heap;

	//If the heap hasn't been initialised :
	if (!heap) {

		core_error("ram.c : ram_free : ram heap not initialised;");
		return;

	}

	//Free the required block;
	heap_free(heap, ptr);

}
