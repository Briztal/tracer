//
// Created by root on 10/17/18.
//

#ifndef TRACER_COPROC_H
#define TRACER_COPROC_H

#include <stdint.h>

struct coproc {
	
	//The size of the coprocessor context;
	uint8_t context_size;
	
	//The
	
};


//Register a coprocessor, for future context switching; The size of the context, and the context saver must be provided;
uint8_t register_coproc(uint8_t context_size, void (*context_saver)(void *));

#endif //TRACER_COPROC_H
