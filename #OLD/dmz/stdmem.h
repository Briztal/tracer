/*
  stdmem.h Part of TRACER

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


#ifndef TRACER_MEM_H
#define TRACER_MEM_H

#include <stdint.h>

#include <stddef.h>


//Copy @size bytes from src to dst;
void memcpy(void *dst, const void *src, size_t size);

//Initialise a memory block to a value;
void memset(void *dst, uint8_t value, size_t size);



#endif //TRACER_MEM_H
