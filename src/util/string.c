/*
  string.c Part of TRACER

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

#include "string.h"

const char alphabet[37] = "0123456789abcdefghijklmnopqrstuvwxyz";


//TODO DOC;
char *itoa(uint32_t value, char *buffer, size_t buffer_size, const uint8_t radix) {

	//As we will build the string reversed, focus on the last case of the buffer;
	char *dst = buffer + buffer_size;

	//Null terminate;
	*dst = 0;

	//If the buffer is empty, we can't avoid an error. Return the last case.
	if (!buffer_size) {
		return dst;
	}

	//Decrement the size;
	buffer_size--;

	//If the buffer is empty or radix is invalid, return the last case.
	if ((!buffer_size) || (radix > 36) || (radix < 2)) {
		return dst;
	}


	while (1) {

		//Focus on the next char;
		dst--;

		//Cache the digits numeric value;
		uint8_t r = (uint8_t) (value % (uint32_t)radix);

		//Update value;
		value /= (uint32_t)radix;

		//Update the buffer with the char value and focus on the next char;
		*dst = alphabet[r];

		//Decrement the size;
		buffer_size--;

		//While the value is not null, iterate
		if ((value == 0) || (buffer_size == 0)) {
			return dst;
		}

	}

}
