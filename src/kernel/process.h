//
// Created by root on 7/12/18.
//

#ifndef TRACER_PROCESS_H
#define TRACER_PROCESS_H


typedef struct {

	void *init_args;

	size_t args_size;


	void *heap;

	void (*exit)();

} process_t;

#endif //TRACER_PROCESS_H
