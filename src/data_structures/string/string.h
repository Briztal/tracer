/*
  string.h - Part of TRACER

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

#ifndef TRACER_BASICSTRING_H
#define TRACER_BASICSTRING_H

#include "stdint.h"

#include "stddef.h"

#include "stdbool.h"

typedef struct {

	//The current length of the string;
	const size_t length;

	//The data of the string;
	const char *const data;

} string_t;



//-------------------------------------- String lifecycle; --------------------------------------



//String deletion;
void string_delete(const string_t *);

//String move;
string_t *string_move(string_t *);


//Unsigned converters;

string_t *str_u8(uint8_t);

string_t *str_u16(uint16_t);

string_t *str_u32(uint32_t);

string_t *str_u64(uint64_t);


//Signed converters

string_t *str_s8(int8_t);

string_t *str_s16(int16_t);

string_t *str_s32(int32_t);

string_t *str_s64(int64_t);


//Float converter
string_t *str_f(float, uint8_t resolution);


//Copy the given array to the dst array in the symetric order;
void string_symmetric_copy(void *dst, const void *src, size_t size);


//-------------------------------------- various conversions ----------------------------------

//Get the location of the first char that is not equal to c from the end of the string.
char *strdiff(const char *str, char c);

//Count the number of chars separating the string beginning and c (or the end);
size_t strcnt(const char *str, char c);

//Count the number of words in the string;
size_t strcw(const char *str, char s);

//Get the next word and its size;
void strgw(const char *str, char s, const char **word_p, size_t *size);

//Get the number of words in the string;
uint8_t string_count_words(const char *in_buffer);


#endif //TRACER_BASICSTRING_H
