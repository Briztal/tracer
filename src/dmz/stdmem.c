//
// Created by root on 10/17/18.
//

#include "stdmem.h"


//Copy @num bytes from src to dst;
void memcpy(void *_dst, const void *_src, size_t num) {
	
	//Convert;
	uint8_t *dst = _dst;
	const uint8_t *src = _src;
	
	//For each byte to copy :
	while(num--) {
		
		//Copy a byte from src to dst;
		*(dst++) = *(src++);
		
	}
	
}


//Initialise a memory block to a value;
void memset(void *_dst, uint8_t value, size_t num) {
	
	//Convert;
	uint8_t *dst = _dst;
	
	//For each byte to copy :
	while(num--) {
		
		//Copy a byte from src to dst;
		*(dst++) = value;
		
	}
	
}