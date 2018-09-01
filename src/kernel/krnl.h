//
// Created by root on 8/21/18.
//

#ifndef TRACER_KRNL_H
#define TRACER_KRNL_H

#include <stddef.h>
#include <kernel/scheduler/prc.h>


//----------------------------------------------------- Kernel Heap ----------------------------------------------------

//Allocate some data in the kernel heap;
void *kmalloc(size_t);

//Allocate and zero-initialise some data in the kernel heap;
void *kcalloc(size_t);

//Allocate and initialise some data in the kernel heap and initialise them;;
void *kialloc(size_t, const void *);

//Free some data in the kernel heap;
void kfree(void *);


//---------------------------------------------------- Kernel panic ---------------------------------------------------

//The kernel panic function;
void kernel_panic(const char *msg);



#endif //TRACER_KRNL_H
