/*
  lnk.h Part of TRACER

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
 * This file contains declarations for all constants provided by the linker script;
 */

#ifndef TRACER_LINK_H
#define TRACER_LINK_H

#include <stdint.h>

//The available RAM bounds;
extern uint8_t __ram_min;
extern const uint8_t __ram_max;

//Load and virtual min and max addresses of data section (resp) in FLASH, RAM and RAM;
extern const uint8_t __data_lma_min, __data_vma_min, __data_vma_max;

//virtual min and max addresses of bss section in RAM;
extern const uint8_t __bss_vma_min,  __bss_vma_max;

//The min and max address of proc modules structs in FLASH;
extern const uint8_t __prmod_min, __prmod_max;

//The min and max address of system modules structs in FLASH;
extern const uint8_t __smod_min, __smod_max;

//The min and max address of periph modules structs in FLASH;
extern const uint8_t __pemod_min, __pemod_max;

//The min and max address of kernel modules structs in FLASH;
extern const uint8_t __kmod_min, __kmod_max;

//The min and max address of user modules structs in FLASH;
extern const uint8_t __umod_min, __umod_max;


#endif //TRACER_LINK_H
