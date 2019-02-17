/*
  startup.h Part of TRACER

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

#ifndef TRACER_STARTUP_H
#define TRACER_STARTUP_H

#include "kernel/hard/impl_defined/hard_defined.h"


static inline void startup_initialise_globals() {
	
	//Cache start addresses of rodata and data,
	const uint8_t *data_src = &__data_lma_min;
	uint8_t *data_dst = &__data_vma_min;

	//Copy rodata in data;
	while (data_dst < &__data_vma_max) {
		*(data_dst++) = (*data_src++);
	}

	//Cache the start address of the bss region;
	uint8_t *bss_ptr = &__bss_vma_min;

	//Null set bss;
	while(bss_ptr < &__bss_vma_max) {
		*(bss_ptr++) = 0;
	}

}

#endif //TRACER_STARTUP_H
