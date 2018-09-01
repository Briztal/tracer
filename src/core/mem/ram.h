//
// Created by root on 8/22/18.
//

#ifndef TRACER_RAM_H
#define TRACER_RAM_H

/*
 * The ram manager manages the available RAM memory.
 *
 * 	It uses variables defined in the unified linker script to manage the whole available RAM block;
 */

#include <stddef.h>


//Initialise memory management values. Will invalidate all previously reserved memory;
void ram_init();

//Reserve some memory in RAM;
void *ram_alloc(size_t);

//Release some reserved memory;
void ram_free(void *);


#endif //TRACER_RAM_H
