/*
  ram.h Part of TRACER

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

#ifndef TRACER_RAM_H
#define TRACER_RAM_H

/*
 * The ram manager manages the available RAM memory.
 *
 * 	It uses variables defined in the unified linker script to manage the whole available RAM block;
 */

#include <stddef.h>


//Initialise memory management values. Will invalidate all previously reserved memory;
void ram_init();

//Get the size of a frame;
size_t ram_frame_size();

//Reserve a frame;
void *ram_alloc_frame();

//Release a reserved frame;
void ram_free_frame(void *frame);

//Debug
//TODO void ram_print();


#endif //TRACER_RAM_H
