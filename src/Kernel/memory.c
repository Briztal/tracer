//
// Created by root on 5/9/18.
//

#include "memory.h"

/*
 * kernel_malloc : the malloc to use across the code; It is thread safe, and checks for exceptions;
 */

void *kernel_malloc(size_t size) {

    //Allocate data in the heap;
    void *ptr = malloc(size);

    //If the allocation failed, error;
    if (!ptr)
        return 0;//TODO ERROR.

    //Return the allocated data;
    return ptr;

}


/*
 * kernel_realloc : the realloc to use across the code; It is thread safe, and checks for exceptions;
 */

void *kernel_realloc(void *data, size_t size) {

    //Reallocate data in the heap;
    void *ptr = realloc(data, size);

    //If the reallocation failed, error;
    if (!ptr)
        return 0;//TODO ERROR.

    //Return the reallocated data;
    return ptr;
}


/*
 * kernel_free : the free to use across the code; It is thread safe, and checks for exceptions;
 */

void kernel_free(void *data) {

    free(data);

}