//
// Created by root on 8/21/18.
//

#ifndef TRACER_ERROR_H
#define TRACER_ERROR_H

#include <stddef.h>

/*
 * The kernel manic library offers a log system for debug purposes, and a trigger for kernel panics.
 *
 * 	A kernel panic will halt any current code, put the kernel in safe mode, and display the log stack in the debug
 * 	interface;
 *
 * 	The log stack is static, and does not require any initialisation, so that it can be used in case of error, and
 * 	without dynamic memory;
 */

#define KERNEL_LOG_STACK_SIZE 10


//------------------------------------------------------ Log stack -----------------------------------------------------

//Push a log in the panic log stack;
void panic_push_log(const char *log);

//Pop a log from the panic log stack;
void panic_pop_log();

//Reset the log stack;
void panic_reset_log();

//Get the log stack and its size for debug purposes;
const char **panic_get_log_stack(size_t *stack_size);

#endif //TRACER_ERROR_H
