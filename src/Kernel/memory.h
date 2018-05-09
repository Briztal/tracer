//
// Created by root on 5/9/18.
//

#ifndef TRACER_MEMORY_H
#define TRACER_MEMORY_H

#include "malloc.h"

//The malloc to use across the code;
void *kernel_malloc(size_t size);

//The malloc to use across the code;
void *kernel_realloc(void *, size_t size);

//The malloc to use across the code;
void kernel_free(void *);

#endif //TRACER_MEMORY_H
