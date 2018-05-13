//
// Created by root on 3/10/18.
//

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

//Get the number of times that 'verif_char' is consecutively present, at the beginning of the char sequence.
uint8_t string_lstrip(const char *in_buffer, char verif_char);

//Get the length of a string;
uint8_t string_length(const char *in_buffer);

//Count thenumber of chars before the limit_char, or the end of the string;
uint8_t string_count_until_char(const char *in_buffer, char limit_char);

//Get the number of words in the string;
uint8_t string_count_words(const char *in_buffer);

//Compare the two strings, and return true if they are the same;
bool string_strcmp(const char *string0, const char *string1);



#endif //TRACER_BASICSTRING_H
