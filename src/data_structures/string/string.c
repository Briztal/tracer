/*
  string.c - Part of TRACER

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

#include <string.h>

#include "string.h"


#include <kernel/kernel.h>


//----------------------------------------------- C string lib complement ----------------------------------------------

/*
 * strcnt : counts the number of chars separating the first char of the separator 0;
 */

size_t strcnt(const char *str, const char s) {

	//Initialise the char counter;
	size_t char_count = 0;

	//Initialise a cache for the current char
	char crt = *(str++);

	//While the current char is not null, or the limit char
	while (crt && (crt != s)) {

		//Update the current char
		crt = *(str++);

		//Increment the size of the copied buffer.
		char_count++;
	}

	//Return the number of characters before the limit char (or the end of the string
	return char_count;

}


/*
 * strdiff : gets the location of the first char of str that is not equal to c;
 *
 * 	If 0 is provided, returns str;
 */

char *strdiff(const char *str, const char c) {

	//If c is null, return str;
	if (!c)
		return (char *) str;

	//While the current char equals c :
	while (*(str) == c) {

		//Go to the next char;
		str++;

	}

	//Return the number of characters before the limit char (or the end of the string
	return (char *) str;

}


/*
 * strcw : counts the number of words in the string, words being non-empty char sequences, separated by the separator
 * char.
 */

size_t strcw(const char *str, const char s) {

	//Initialise a word counter.
	size_t nb_words = 0;

	//initialise_peripheral a size for the current word.
	size_t size;

	do {

		//Remove unnecessary nb_spaces at the initial of the char sequence.
		str = strdiff(str, s);

		//Get the size of the next word (will ne zero only if the sequence is finished).
		size = strcnt(str, s);

		//If we already are at the end of the word, the string is finished :
		if (!size) {
			break;
		}

		//Go to the end of the word.
		str += size;

		//Increment the number of words
		nb_words++;

	} while (true);

	//Return the number of words.
	return nb_words;

}

/*
 * strgw : receives a string and a separator, determines the address of the word's first char, and its size;
 */

void strgw(const char *const str, const char s, const char **const word_p, size_t *const size) {

	//Remove separators at the beginning of the char sequence.
	*word_p = strdiff(str, s);

	//Determine and save the size of the next word;
	//As separators have been removed, will be zero only if the string is over;
	*size = strcnt(str, s);

}


//----------------------------------------------- String struct functions ----------------------------------------------

/*
 * length_to_size : this function converts a string length (null termination and size of char non comprised) to a
 *  real array size;
 */

static inline size_t length_to_size(const size_t length) {

	return (length + 1) * sizeof(char);

}


//Create a string of the given length
string_t *string_create(const size_t string_length, const char *const heap_data) {

	//Create the string initializer;
	string_t init = {
		.length = string_length,
		.data = heap_data,
	};

	//If the allocation completed, cache a casted copy of the pointer. Compiler optimised;
	string_t *string = kernel_malloc_copy(sizeof(string_t), &init);

	//Return the created string;
	return string;

}



/*
 * string_delete : a string deletion required two operations (data and struct deletion).
 *  This function implements both;
 */

void string_delete(const string_t *const string) {

	//Free the string's data;
	kernel_free((void *) string->data);

	//Free the string;
	kernel_free((void *) string);

}


/*
 * String move : creates a new string, and moves src's data to it. Then, resets src to an empty string;
 */

string_t *string_move(string_t *src) {

	//First, create a copycat of src, phagocyting its data;
	string_t *clone = string_create(src->length, src->data);

	//Then, allocate data in the heap for an empty string;
	char *ptr = kernel_malloc(length_to_size(0));

	//Initialise the string to empty.
	*ptr = 0;

	//Create a stack initialiser;
	string_t init = {
		.data = ptr,
		.length = 0,
	};

	//Re-initialise src with new data;
	memcpy(src, &init, sizeof(string_t));

	//Return the new string;
	return clone;

}


//---------------------------------------------- String symmetric parsing ----------------------------------------------

/**
 * uint_to_as : receives @u a pointer to an int of an unknow size, and uses the digit parser @to_digit to convert it
 * 	to its string value, in @str;
 *
 * @param src : the pointer to the unsigned int;
 * @param digit_converter : the funtion to get the current digit and update the integer; Returns '\0' when the
 * conversion is finished;
 * @param dst : the char array where to store the string;
 * @param negative : if true, will insert '-' at the end of the string;
 * @return the size of the computed string;
 */

static size_t string_parse_int(void *src, char (*digit_converter)(void *, bool *), char *dst, bool negative) {

	//Initialise the size counter;
	size_t data_length = 0;

	//Cache the stop pointer;
	bool stop = false;

	//While there are digits to process :
	do {

		//Determine the current char;
		char c = (*digit_converter)(src, &stop);

		//Insert the char;
		*(dst++) = c;

		//Increment the size counter;
		data_length++;

	} while (!stop);

	//Write the sign and increase the length;
	if (negative) {
		dst[data_length++] = '-';
	}

	//Return the data length;
	return data_length;


}


//-------------------------------------------------- Digit Converters --------------------------------------------------

//size converter;
#define TO_U(size) uint##size##_t

//Digit parser declarator;
#define define_digit_converter(size)\
static char u##size##_to_digit(void *p, bool *stop) {\
\
    /*Cache the int;*/\
    TO_U(size) u = *(TO_U(size) *) p;\
\
    /*Cache the first char : modulate by 10 and add 48 to convert to char digits;*/\
    char c = (uint8_t)((TO_U(size)) u % (TO_U(size)) 10) + (uint8_t) 48;\
\
    /*Divide u by 10;*/\
    u /= 10;\
\
    /*If u is null now the conversion is done;*/\
    *stop = !u;\
\
    /*Update the int;*/\
    *(TO_U(size) *)p = u;\
\
    /*Return the char;*/\
    return c;\
\
}

define_digit_converter(8)
define_digit_converter(16)
define_digit_converter(32)
define_digit_converter(64)


//--------------------------------------------------- Symmetric copy ---------------------------------------------------

/*
 * symmetric_copy : this function turns dst to the symmetric of sym_str;
 */

void string_symmetric_copy(void *dst, const void *sym_str, size_t size) {

	//Convert dst and src to a uint8_t pointer, as arithmetic on void * is illegal;
	uint8_t *d = (uint8_t *) dst;
	uint8_t *s = (uint8_t *) sym_str;

	//Increment the dst pointer;
	d += size - (uint8_t) 1;

	//Nullify the end;
	*(d + 1) = 0;

	//For each char :
	for (; size--;) {

		//Symmetric copy from src to dst;
		*(d--) = *(s++);

	}

}


/*
 * string_create_from_symmetric : allocates heap space for the string, and copies symmetrically the source;
 *
 *  Calls string_create and returns the result;
 */

static string_t *string_create_from_symmetric(const size_t string_length, const char *const stack_src) {

	//Cache the size of the zone to allocate zone;
	size_t size = length_to_size(string_length);

	//Create a memory space in the heap for data;
	void *ptr = kernel_malloc(size);

	//Symmetric copy from stack to heap; destination is null-terminated;
	string_symmetric_copy(ptr, stack_src, string_length);

	//Built the string and return the result;
	return string_create(string_length, ptr);

}


//--------------------------------------------- String numeric constructor ---------------------------------------------

string_t *str_u8(uint8_t i) {

	//Declare a temporary array;
	char t[3];

	//Parse the int with the u8 parser,
	size_t data_length = string_parse_int(&i, &u8_to_digit, t, false);

	//Create and return a string of the given length with uint data;
	return string_create_from_symmetric(data_length, t);

}


string_t *str_u16(uint16_t i) {

	//Declare a temporary array;
	char t[5];

	//Parse the int with the u8 parser,
	size_t data_length = string_parse_int(&i, &u16_to_digit, t, false);

	//Create and return a string of the given length with uint data;
	return string_create_from_symmetric(data_length, t);

}


string_t *str_u32(uint32_t i) {

	//Declare a temporary array;
	char t[10];

	//Parse the int with the u8 parser,
	size_t data_length = string_parse_int(&i, &u32_to_digit, t, false);

	//Create and return a string of the given length with uint data;
	return string_create_from_symmetric(data_length, t);

}


string_t *str_u64(uint64_t i) {

	//Declare a temporary array;
	char t[20];

	//Parse the int with the u8 parser,
	size_t data_length = string_parse_int(&i, &u64_to_digit, t, false);

	//Create and return a string of the given length with uint data;
	return string_create_from_symmetric(data_length, t);

}


string_t *str_s8(int8_t i) {

	//Determine the absolute value of i;
	uint8_t abs = (i < 0) ? (uint8_t) -i : (uint8_t) i;

	//Declare a temporary array;
	char t[4];

	//Convert the int to its string value in reverse order in t;
	size_t data_length = string_parse_int(&abs, &u16_to_digit, t, i < 0);


	//Create and return a string of the given length with int data;
	return string_create_from_symmetric(data_length, t);

}


string_t *str_s16(int16_t i) {

	//Determine the absolute value of i;
	uint16_t abs = (i < 0) ? (uint16_t) -i : (uint16_t) i;

	//Declare a temporary array;
	char t[6];

	//Convert the int to its string value in reverse order in t;
	size_t data_length = string_parse_int(&abs, &u16_to_digit, t, i < 0);


	//Create and return a string of the given length with int data;
	return string_create_from_symmetric(data_length, t);

}

string_t *str_s32(int32_t i) {

	//Determine the absolute value of i;
	uint32_t abs = (i < 0) ? (uint32_t) -i : (uint32_t) i;

	//Declare a temporary array;
	char t[11];

	//Convert the int to its string value in reverse order in t;
	size_t data_length = string_parse_int(&abs, &u16_to_digit, t, i < 0);


	//Create and return a string of the given length with int data;
	return string_create_from_symmetric(data_length, t);

}

string_t *str_s64(int64_t i) {

	//Determine the absolute value of i;
	uint64_t abs = (i < 0) ? (uint64_t) -i : (uint64_t) i;

	//Declare a temporary array;
	char t[21];

	//Convert the int to its string value in reverse order in t;
	size_t data_length = string_parse_int(&abs, &u16_to_digit, t, i < 0);


	//Create and return a string of the given length with int data;
	return string_create_from_symmetric(data_length, t);

}


/*
 * setTo : this function sets the string as the representation of the given float,
 *  with the given resolution, ie the given number of figures after comma.
 */

string_t *str_f(float f, uint8_t resolution) {

	//Before anything, let's determine the sign and take the absolute value;
	bool negative;
	float abs = ((negative = (f < 0))) ? -f : f;

	//The first step will be to call a constructor for the integer part;
	string_t *int_string;

	//Declare the fractional part;
	float frac;


	/*
	 * First, we will determine the optimal function to call to convert the integer part;
	 *  bounds will depend on the sign;
	 *
	 * As float integer part can be signed, we will only call the signed int constructor;
	 *
	 * As signed numbers absolute bounds differ of 1 (ex : -128, 127), the negative bound will be treated of the next
	 *  greater type (except for the greatest type;
	 */

	if (abs <= (float) ((uint8_t) -1 >> 1)) {
		//1 byte integer part :
		int_string = str_s8((int8_t) abs);
		frac = f - (float) (int8_t) f;
	} else if (abs < (float) ((uint16_t) -1 >> 1)) {
		//2 bytes integer part :
		int_string = str_s16((int16_t) abs);
		frac = f - (float) (int16_t) f;
	} else if (abs < (float) ((uint32_t) -1 >> 1)) {
		//4 bytes integer part :
		int_string = str_s32((int32_t) abs);
		frac = f - (float) (int32_t) f;
	} else if (abs < (float) ((uint64_t) -1 >> 1)) {
		//4 bytes integer part :
		int_string = str_s64((int64_t) abs);
		frac = f - (float) (int64_t) f;
	} else {

		//Create and return a string containing ovf
		int_string = string_create(3, "ovf");
		return int_string;

	}

	//If the fractional part is negative, take its absolute value;
	if (frac < 0)
		frac = -frac;

	//Create a temp array to contain the frac part. + 1 from the dot; no null termination required; cache a copy;
	char frac_part[resolution + 1], *insertion_ptr = frac_part;

	//Insert the dot;
	*(insertion_ptr++) = '.';

	//For each digit under the comma :
	for (; resolution--;) {

		//Multiply the fractional part by 10;
		frac *= (float) 10;

		//Get the digit;
		char c = (char) ((uint8_t) frac + (uint8_t) 48);

		frac -= (float) (uint8_t) frac;

		//Insert the digit;
		*(insertion_ptr++) = c;

	}

	//Create a string from the stack array;
	//string_t *frac_string = string_create_from_stack(resolution + 1, frac_part);TODO

	//For instance, return the integer part;
	//TODO TSTRING;
	return int_string;

}
