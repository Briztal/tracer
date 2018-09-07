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

#include <stdint.h>

const extern uint8_t _data_lma;
extern uint8_t _data_vma;
extern uint8_t _end_data;
extern uint8_t _bss_vma;
extern uint8_t _end_bss;


static inline void startup_initialise_globals() {


	//Cache start addresses of rodata and data,
	const uint8_t *data_src = &_data_lma;
	uint8_t *data_dst = &_data_vma;

	//Copy rodata in data;
	while (data_dst < &_end_data) {
		*(data_dst++) = (*data_src++);
	}

	//Cache the start address of the bss region;
	uint8_t *bss_ptr = &_bss_vma;

	//Null set bss;
	while(bss_ptr < &_end_bss) {
		*(bss_ptr++) = 0;
	}

}

#endif //TRACER_STARTUP_H
