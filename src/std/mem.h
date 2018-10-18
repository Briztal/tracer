//
// Created by root on 10/17/18.
//

#ifndef TRACER_MEM_H
#define TRACER_MEM_H

#include <stdint.h>

#include <stddef.h>


//Copy @num bytes from src to dst;
void memcpy(void *dst, const void *src, size_t num);

//Initialise a memory block to a value;
void memset(void *dst, uint8_t value, size_t num);



#endif //TRACER_MEM_H
