//
// Created by root on 8/12/18.
//


#include "arm_v7m.h"

//The armv7 M has only one core and one thread;
const uint8_t core_nb_threads = 1;

//The stack alignment is made on 8 bytes;
const uint32_t core_stack_alignment = 8;
