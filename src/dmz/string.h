/*
  string.h Part of TRACER

  Copyright (c) 2017 Raphaël Outhier

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

#ifndef TRACER_STRING_H
#define TRACER_STRING_H

#include <stdbool.h>

#include <stdint.h>

#include <stddef.h>

//Compare two strigs;
int8_t strcmp(const char *c1, const char *c2);

//Determine the length of a string;
size_t strlen(const char *str);

//Returns the length of the string, or the max if superior;
size_t strlen_safe(const char *str, size_t max_lengh);

//Copy the string and null terminate. src can be not null terminated, will not deadloop;
void strcopy_safe(char *dst, const char *src, size_t length);

//TODO DOC
char *itoa(uint32_t value, char *buffer, size_t buffer_size, uint8_t radix);


#endif //TRACER_STRING_H
