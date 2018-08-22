//
// Created by root on 8/15/18.
//

#include "startup.h"

extern uint8_t _start_rodata;
extern uint8_t _end_rodata;
extern uint8_t _start_data;
extern uint8_t _start_bss;
extern uint8_t _end_bss;


void startup_initialise_globals() {

	//Cache start addresses of rodata and data,
	uint8_t *rodata_ptr = &_start_rodata;
	uint8_t *data_ptr = &_start_data;

	//Copy rodata in data;
	while (rodata_ptr < &_end_rodata) {
		*(data_ptr++) = (*rodata_ptr++);
	}

	//Cache the start address of the bss region;
	uint8_t *bss_ptr = &_start_bss;

	//Null set bss;
	while(bss_ptr < &_end_bss) {
		*(bss_ptr++) = 0;
	}
}