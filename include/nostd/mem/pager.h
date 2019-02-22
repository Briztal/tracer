
/*
  pager.h Part of nostd

  Copyright (c) 2019 Raphaël Outhier

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

#ifndef NOSTD_PAGER_H
#define NOSTD_PAGER_H

#include <stdint.h>

#include <error.h>

#include "list.h"


/**
 * This file contains the spec for the page allocator;
 *
 *  The page allocator is in charge of managing a fixed contiguous memory area, and to allocate pages in this area,
 *  with the following constraints :
 *  - all pages have the same size;
 *  - page size is a power of 2;
 *  - pages are aligned relatively to their sizes;
 *
 *  The library can be compiled at two diagnosis levels :
 *	- no diagnosis : fast, no error detection;
 *	- diagnosis enabled : slow, error detection : each time a page is manipulated, a checks are made on its address;
 */


/*The pager error flag fits on a byte;*/
#define pager_error_t uint8_t

/**
 * pager : contains metadata about the memory area the pager manages
 */

struct page_allocator {

    /*Error flags;*/
    pager_error_t pa_error;

    /*The size of a page;*/
    size_t pa_page_size;

    /*The number of free pages;*/
    size_t pa_nb_free;

    /*A free page;*/
    void *pa_free;

    /*The start of the memory area;*/
    void *pa_memory_start;

    /*The start of the page array area;*/
    void *pa_page_array_start;

    /*The end of the page array area;*/
    void *pa_page_array_end;

    /*The start of the non-allocated pages zone;*/
    void *pa_non_allocated;

};


/*
 * pager error flags are listed below :
 */

/*If set, a query was made with a pager in error mode;*/
#define PAGER_ERROR_REDETECTION   ((pager_error_t)    (1 << 0))

/*If set, a check was made on a pager outside the mpool's memory zone*/
#define PAGER_ERROR_BLOCK_OUTSIDE_MEMORY   ((pager_error_t)    (1 << 1))

/*If set, a check was made on a misaligned page;*/
#define PAGER_ERROR_BLOCK_MISALIGNED   ((pager_error_t)    (1 << 2))

/*If set, the non allocated page zone went outside the pager memory zone;*/
#define PAGER_ERROR_NON_ALLOCATED_BLOCK_START   ((pager_error_t)    (1 << 3))

/*If set, a supposedly allocated page was located in the non allocated zone;*/
#define PAGER_ERROR_ALLOCATED_PAGE_IN_NON_ALLOCATED_ZONE   ((pager_error_t)    (1 << 4))

/*If set, a supposedly allocated page was found in the free pages list;*/
#define PAGER_ERROR_ALLOCATED_PAGE_IN_FREE_LIST   ((pager_error_t)    (1 << 4))

/*If set, an error occurred in the page allocation function;*/
#define PAGER_ERROR_ALLOC   ((pager_error_t)    (1 << 5))

/*If set, an error occurred in the page free function;*/
#define PAGER_ERROR_FREE  ((pager_error_t)    (1 << 6))


/*Construct a page allocator;*/
void pager_ctor(struct page_allocator *pager, void *memory_start, size_t memory_size, size_t page_size, uint8_t align_magnitude);

/*Destruct a page allocator;*/
void mpool_dtor(struct page_allocator *pager);

/*Return how many pages can be allocated;*/
size_t pager_available(struct page_allocator *pool);


/*Allocate an page, thread safe;*/
void *pager_alloc_page_safe(struct page_allocator *pager);

/*Allocate an page, thread unsafe;*/
void *pager_alloc_page_unsafe(struct page_allocator *pager);


/*Free an page, thread safe;*/
void pager_free_page_safe(struct page_allocator *pager, void *page);

/*Free an page, thread unsafe;*/
void pager_free_page_unsafe(struct page_allocator *pager, void *page);


#endif /*NOSTD_PAGER_H*/
