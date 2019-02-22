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

/*Print a string in the kernel log interface;*/
void __printk(const char *str);

/*Display a formatted string in the kernel log interface;*/
void __printkf(const char *fstr, const void **args, size_t args_size);

/*Display an error message and halt;*/
void __kernel_panic(const char *tmp_str);


#endif /*TRACER_ERROR_H*/
