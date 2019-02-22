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

#include <nostd/mem/pager.h>

#include <bitwise.h>

#include <kernel/common.h>

#include <khal/lnk.h>


/*Frames will be of size 2 ^ 10 = 1024 bytes;*/
#define FRAME_MAGNITUDE 10

/*TODO CHECK ON THE FRAME VALUE, MUST BE LESSER THAN 32;*/


/*------------------------------------------------------- Globals ------------------------------------------------------*/

/*The heap that will manage the ram block;*/
static struct page_allocator ram_allocator;



/*------------------------------------------------------- RAM mgt ------------------------------------------------------*/

/**
 * ram_init : resets the heap that manages the RAM block.
 *
 * 	Executing this function will invalidate all allocated memory in the ram.
 *
 * 	It must be ran carefully;
 */

void ram_init() {

	/*Initialise the ram allocator;*/
    pager_ctor(&ram_allocator, &__ram_min, &__ram_max - &__ram_min,  ALIGNMENT_SIZE(FRAME_MAGNITUDE), FRAME_MAGNITUDE);

    __printk("RAM manager initialised\n\r");

}


/**
 * ram_alloc_frame : allocates a RAM frame;
 *
 * Fail-safe, a kernel panic is generated in case of error;
 *
 * @return the lowest address of the allocated frame;
 */

void *ram_alloc_frame() {

    void *frame;

	/*Allocate a frame;*/
	frame = pager_alloc_page_safe(&ram_allocator);

    /*If the allocation fails :*/
    if (!frame) {

        /*Kernel panic;*/
        __kernel_panic("RAM frame allocation failure;");

    }

    /*Return the ref of the frame;*/
    return frame;

}


/**
 * ram_free_frame : frees an allocated RAM frame, referenced by its lowest address;
 *
 * @param frame : the lowest address of the frame to free;
 */

void ram_free_frame(void *frame) {

    /*Allocate a frame;*/
    pager_free_page_safe(&ram_allocator, frame);

}