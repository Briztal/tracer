//
// Created by root on 9/10/18.
//

#include "kernel/res/printk.h"

#include <fstring.h>

#include <kernel/hard/except.h>


//static void (*log_output) = &__debug_print_char;


/**
 * _printkf : decodes and displays the provided formatted string over the log output;
 *
 * @param str : the formatted string;
 * @param args : the array of arguments;
 * @param args_size : the size of the array;
 */

void _printkf(const char * str, const void ** args,  size_t args_size) {
	
	critical_section_enter();
	
	fdecode(&__dbg_print_block, 0, str, args, args_size);
	
	critical_section_leave();
	
	
}
