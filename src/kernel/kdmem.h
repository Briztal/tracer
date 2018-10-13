//
// Created by root on 10/10/18.
//

#ifndef TRACER_KMALLOC_H
#define TRACER_KMALLOC_H

#include <stddef.h>

#include <kernel/mem/heap.h>


//Initialise the kernel memory;
void kdmem_init();

//Allocate some data in the kernel heap;
void *kmalloc(size_t);

//Allocate and zero-initialise some data in the kernel heap;
void *kcalloc(size_t);

//Allocate and initialise some data in the kernel heap and initialise them;;
void *kialloc(size_t, const void *);

//Free some data in the kernel heap;
void kfree(void *);


#endif //TRACER_KMALLOC_H
