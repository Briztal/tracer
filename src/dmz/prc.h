//
// Created by root on 11/3/18.
//

#ifndef TRACER_PRC_H
#define TRACER_PRC_H

#include <stdint.h>


/**
 * prc_desc : contains the process hardware requirements;
 */

struct prc_req {
	
	//The required ram size;
	size_t ram_size;
	
	//The size available for each stack;
	size_t stack_size;
	
	//The required period between two preemptions;
	uint16_t activity_time;
	
};


/**
 * prc_desc : contains the process's runtime data;
 */

struct prc_desc {
	
	//The process function;
	void (*function)(void *args, size_t args_size);
	
	//The function's arguments;
	const void *args;
	
	//The arguments size;
	size_t args_size;
	
};

#endif //TRACER_PRC_H
