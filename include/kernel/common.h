/*
  common.h Part of TRACER

  Copyright (c) 2018 Raphaël Outhier

  TRACER is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  TRACER is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  aint32_t with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/

/*
 * This file contains declarations for kernel function that can be used by all kernel components;
 */

#ifndef TRACER_ERROR_H
#define TRACER_ERROR_H

#include <stddef.h>

//Print a string in the kernel log interface;
#define printk(str) _printkf(str, 0, 0);

//Display a formated string in the kernel log interface;
void _printkf(const char * str, const void ** args,  size_t args_size);


//Packer for formated strings;
#define printkf(msg, ...) {\
		const void * __klog_args__ [] = VARIADIC_PACKAGE(__VA_ARGS__);\
		_printkf(msg, __klog_args__, VARIADIC_COUNT(__VA_ARGS__));\
	}


//Display an error message and halt;
void kernel_panic(const char *tmp_str);


#endif //TRACER_ERROR_H
