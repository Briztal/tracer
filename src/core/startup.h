//
// Created by root on 8/15/18.
//

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
