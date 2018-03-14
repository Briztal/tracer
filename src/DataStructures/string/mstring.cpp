//
// Created by root on 3/14/18.
//

#include <string.h>

#include "mstring.h"


//--------------------------------------------- Append Operators ---------------------------------------------

/*
 * add operator, for char;
 */

mstring &mstring::operator+=(char c) {

    //Cache the old size;
    uint16_t old_size = size;

    //Write the char at the end of the array;
    buffer[old_size - 1] = c;

    //Resize the array;
    resizeTo(size + (uint16_t) 1);

    buffer[old_size] = 0;

    //Return a reference to us;
    return *this;

}


/*
 * add operator, for const char arrays;
 */

mstring &mstring::operator+=(const char *src) {

    //Concatenate on the right;
    right_concat(src);

    //Return a reference to us;
    return *this;

}


/*
 * add operator, for mstring reference;
 */

mstring &mstring::operator+=(const mstring &src) {

    //Determine the new size
    uint16_t new_size = size + src.size - (uint16_t) 1;

    //Cache the old size;
    uint16_t old_size = size;

    //Now we must resize to the appropriate length;
    resizeTo(new_size);

    //Copy src in the created space, and copy the '\0' at the end;
    memcpy(buffer + old_size - 1, src.buffer, src.size * sizeof(char));

    //Return a reference to us;
    return *this;

}


//-------------------------------------- Concatenation --------------------------------------

/*
 * left_concat : this function concatenates the given char array on our right;
 */

void mstring::left_concat(const char *src) {

    //First cache the src pointer to determine the size;
    const char *c_src = src;

    //Initialise the new size as the current size;
    uint16_t src_size = 0;

    //Increment the new size while we haven't reached src's end;
    while (*(c_src++)) {
        src_size++;
    }

    //Update the new size
    uint16_t new_size = size + src_size;

    //Cache the old size;
    uint16_t old_size = size;

    //Now we must resize to the appropriate length;
    resizeTo(new_size);

    //First, shift our mstring of src_size positions on the right;
    memcpy(buffer + src_size, buffer, old_size * sizeof(char));

    //Then, insert src's mstring in the fred space;
    memcpy(buffer, src, src_size * sizeof(char));

}


/*
 * right_concat : this function concatenates the given char array on our right;
 */

void mstring::right_concat(const char *src) {

    //First cache the src pointer to determine the size;
    const char *c_src = src;

    //Initialise the new size as the current size;
    uint16_t src_size = 0;

    //Increment the new size while we haven't reached src's end;
    while (*(c_src++)) {
        src_size++;
    }

    //Update the new size
    uint16_t new_size = size + src_size;

    //Cache the old size;
    uint16_t old_size = size;

    //Now we must resize to the appropriate length;
    resizeTo(new_size);

    //Copy src in the created space, and copy the '\0' at the end;
    memcpy(buffer + old_size - (uint16_t) 1, src,(src_size + 1) * sizeof(char));

}


//--------------------------------------- External operators ---------------------------------------

/*
 * Sum operator : sums a char * to the left of src;
 */

mstring &&operator+(const char *c, mstring &&src) {

    //Concatenate c to the left of src;
    src.left_concat(c);

    //Return a reference to src;
    return (mstring &&)src;
}


mstring operator+(const mstring &s0, const mstring &i) {

    mstring s(s0);
    s += i;
    return s;

}

mstring &&operator+(const mstring &s0, mstring &&i) {

    return (mstring&&)(i += s0);

}

mstring &&operator+(mstring &&s0, const mstring &i) {

    return (mstring&&)(s0 += i);

}

mstring &&operator+(mstring &&s0, mstring &&i) {

    return (mstring&&)(s0 += i);

}
