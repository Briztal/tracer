//
// Created by root on 9/10/18.
//

#include "log.h"

#include "kernel/debug/debug.h"

#include <util/stdout.h>


static void (*log_output) = &debug_print_char;

/**
 * _kernel_log : decodes and displays the provided formatted string over the log output;
 *
 * @param str : the formatted string;
 * @param args : the array of arguments;
 * @param args_size : the size of the array;
 */

void _kernel_log(const char *str, const void **args, size_t args_size) {

	stdout(log_output, str, args, args_size);

	//TODO STRINGS ARGS ARE NOT DISPLAYED PROPERLY
	//TODO STRINGS ARGS ARE NOT DISPLAYED PROPERLY
	//TODO STRINGS ARGS ARE NOT DISPLAYED PROPERLY
	//TODO STRINGS ARGS ARE NOT DISPLAYED PROPERLY
	//TODO STRINGS ARGS ARE NOT DISPLAYED PROPERLY
	//TODO STRINGS ARGS ARE NOT DISPLAYED PROPERLY
	//TODO STRINGS ARGS ARE NOT DISPLAYED PROPERLY
	//TODO STRINGS ARGS ARE NOT DISPLAYED PROPERLY
	//TODO STRINGS ARGS ARE NOT DISPLAYED PROPERLY
	//TODO STRINGS ARGS ARE NOT DISPLAYED PROPERLY
	//TODO STRINGS ARGS ARE NOT DISPLAYED PROPERLY
	//TODO STRINGS ARGS ARE NOT DISPLAYED PROPERLY
	//TODO STRINGS ARGS ARE NOT DISPLAYED PROPERLY
	//TODO STRINGS ARGS ARE NOT DISPLAYED PROPERLY
	//TODO STRINGS ARGS ARE NOT DISPLAYED PROPERLY

}
