//
// Created by root on 3/10/18.
//

#include <malloc.h>

#include "string.h"

#include "Arduino.h"
#define SIZE_LIMIT (uint16_t) 255



//-------------------------------------- Required constructors --------------------------------------

/*
 * Default constructor : initialises the string to an empty string (null buffer and zero size;
 */
string::string() : size(1), buffer(nullptr) {

    //Set to zero size string;
    resizeTo(1);

    //Set first char to end;
    *buffer = 0;

}


/*
 * Copy constructor : this function will start to the given size, and copy all data;
 */

string::string(const string &src) : string() {

    //Resize to the given size;
    if (!resizeTo(src.size)) {
        return;
    }

    //Copy all data;
    memcpy(buffer, src.buffer, size * sizeof(char));

}


/*
 * Move constructor : this function will start to the given size and siphon all data;
 */

string::string(string &&src) noexcept : size(src.size), buffer(src.buffer) {

    //Reset fields in the src string;
    src.size = 0;
    src.buffer = nullptr;

}


/*
 * Destructor : will free the buffer;
 */

string::~string() {

    //Free the data of the string;
    free(buffer);

}


//---------------------------------------- Primitive types constructors ----------------------------------------

#define define_constructor(type) \
string::string(type i) : string() {*this = i; }

define_constructor(uint8_t);
define_constructor(int8_t);
define_constructor(uint16_t);
define_constructor(int16_t);
define_constructor(uint32_t);
define_constructor(int32_t);
define_constructor(uint64_t);
define_constructor(int64_t);

string::string(float i) : string() { *this = i; }


/*
 * Constructor from char : initialises to empty and calls the char init function;
 */

string::string(char c) : string() {
    *this = c;
}


/*
 * Constructor from float : initialises to empty and calls the float init function;
 */

string::string(float f, uint8_t resolution) : string() {
    this->setTo(f, resolution);
}


/*
 * setTo : this function sets the string as the representation of the given float,
 *  with the given resolution, ie the given number of figures after comma.
 */

void string::setTo(float f, uint8_t resolution) {

    //Before anything, let's determine the sign and take the absolute value;
    bool negative;
    float abs = ((negative = (f < 0))) ? -f : f;

    //First, we will determine the optimal function to call to convert the integer part; bounds will depend on the sign;
    if (negative) {
        if (f < (float) ((uint8_t) -1 >> 1)) {
            //1 byte integer part :
            *this = (int8_t) f;
        } else if (f < (float) ((uint16_t) -1 >> 1)) {
            //1 byte integer part :
            *this = (int16_t) f;
        } else if (f < (float) ((uint32_t) -1 >> 1)) {
            //1 byte integer part :
            *this = (int32_t) f;
        } else {
            //1 byte integer part :
            *this = (int64_t) f;
        }
    } else {
        if (f < (float) (uint8_t) -1) {
            //1 byte integer part :
            *this = (uint8_t) f;
        } else if (f < (float) (uint16_t) -1) {
            //1 byte integer part :
            *this = (uint16_t) f;
        } else if (f < (float) (uint32_t) -1) {
            //1 byte integer part :
            *this = (uint32_t) f;
        } else {
            //1 byte integer part :
            *this = (uint64_t) f;
        }

    }

    //Get the fractional part;
    float frac = abs - (float) (uint64_t) f;

    //Cache the insertion index;
    uint16_t insertion_index = size - (uint16_t) 1;

    //Resize to the array to contain the decimal part (plus dot);
    resizeTo(size + resolution + (uint16_t) 1);

    //Insert the dot;
    buffer[insertion_index++] = '.';

    //For each digit under the comma :
    for (; resolution--;) {

        //Multiply the fractionnal part by 10;
        frac *= (float)10;

        //Get the digit;
        char c = (char) ((uint8_t) frac + (uint8_t)48);

        frac -= (float)(uint8_t)frac;
        //Insert the digit;
        buffer[insertion_index++] = c;

    }

    //Null terminate the string;
    buffer[insertion_index] = 0;

}

/*
 * Char array constructor : initialises the data to represent exactly the given char array;
 *
 * It first initialises the string to empty, and then updates its data;
 */

string::string(const char *src) : string() {


    //If src is empty, nothing to do;
    if (!*src)
        return;


    //First, we must determine the size of the char array;
    const char *c_src = src;

    //The size of src (null termination comprised;
    uint8_t src_size = 1;

    //Increment the size wile the current char is not null;
    while (*(c_src++)) {
        src_size++;
    }

    resizeTo(src_size);

    memcpy(buffer, src, size * sizeof(char));

}


/*
 * Constructor with char array and size;
 *
 * Initialises the string as empty, resizes to the required length and copies the data;
 */

string::string(const char *src, uint16_t size) : string() {

    //The, resize to the required size, plus 1 to insert the null termination;
    resizeTo(size+(uint16_t)1);

    //Then, copy the given portion of the string;
    memcpy(buffer, src, size * sizeof(char));

    //Null terminate;
    buffer[size] = 0;

}


//-------------------------------------- Assignment operators--------------------------------------

/*
 * Move assignment operator : follows the copy and swap idiom;
 */

string &string::operator=(string &&src) {

    //Swap objects;
    swap(*this, src);

    //Return a reference to us;
    return *this;

}


/*
 * Copy assignment operator : follows the copy and swap idiom;
 */

string &string::operator=(const string &src) {

    //Clone src;
    string ssrc = src;

    //Swap objects;
    swap(*this, ssrc);

    //Return a reference to us;
    return *this;

}


/*
 * char assignment operator;
 */

string &string::operator=(char i) {

    //Resize to size_1;
    resizeTo(2);

    //Copy the char at the first position;
    *buffer = i;

    //Nullify the end (char 1);
    buffer[1] = 0;

    //Return a pointer to this;
    return *this;

}


/*
 * char array assignment;
 */

string &string::operator=(const char *src) {

    //First, we will determine the size;
    uint8_t new_size = 1;

    //Cache the char pointer for size determination;
    const char *size_src = src;

    //Increment the size while the current char is not zero;
    while (*(size_src++)) {
        new_size++;
    }

    //Resize to the given size;
    resizeTo(new_size);

    //Copy the whole src array to our buffer;
    memcpy(buffer, src, new_size * sizeof(char));

    //Return a pointer to this;
    return *this;

}


#define uint_to_a(i, max_digits, nb_bits)\
    /*Declare a temporary array;*/\
    char t[max_digits];\
    \
    /*Convert the int to its string value in reverse order in t;*/\
    uint8_t data_length = cstring::_uint##nb_bits##_to_as(i, t);\
    \
    /*Resize the array;*/\
    resizeTo(data_length + 1);\
    \
    /*Copy digits of i in data;*/\
    mmemcpy_reverse(buffer, t, data_length);\
    \
    /*Return a pointer to this;*/\
    return *this;

#define int_to_a(i, max_digits, nb_bits)\
    \
    /*First, determine the sign and take the absolute value of i*/\
    bool negative;\
    if ((negative = (i) < 0)) (i) = -(i);\
    \
    /*Declare a temporary array;*/\
    char t[max_digits];\
    \
    /*Convert the int to its string value in reverse order in t;*/\
    uint8_t data_length = cstring::_uint##nb_bits##_to_as((uint##nb_bits##_t)(i), t);\
    \
    /*Resize the array;*/\
    resizeTo(data_length + 2);\
    \
    /*Mark the "-" eventually;*/\
    if (negative) *buffer = '-';\
    \
    /*Copy digits of i in data;*/\
    mmemcpy_reverse(buffer + 1, t, data_length);\
    \
    /*Return a pointer to this;*/\
    return *this;


#define impl_int_initialiser(length, max_digits)\
    string& string::operator=(uint##length##_t i) {\
        uint_to_a(i, max_digits, length);\
    }\
    string& string::operator=(int##length##_t i) {\
        int_to_a(i, max_digits, length);\
    }


/*
 * Implement all initializer, for all uints and ints of all sizes;
 */

impl_int_initialiser(8, 3);

impl_int_initialiser(16, 5);

impl_int_initialiser(32, 10);

impl_int_initialiser(64, 20);


/*
 * float assignment operator;
 */

string &string::operator=(float f) {

    setTo(f, 2);

    return *this;

}


#undef uint_to_a


//-------------------------------------- Size modification --------------------------------------

/*
 * ResizeTo : this function will modify the buffer to be of the required size.
 *
 *  This function cannot set the size to zero, as a string always contains almost a null char;
 *
 *  For efficiency purposes, no modification is made on data in the reallocated array (null termination, etc...).
 *      Each calling function has to implements its modifications;
 */

bool string::resizeTo(uint16_t new_size) {

    //If the buffer must be reset to an empty buffer :
    if (!new_size) {

        //If the size is already 1, nothing to do;
        if (size == 1) {
            return true;
        }

        //If not, set the new_size to 1 and resize;
        new_size = 1;

    }

    //Reallocate the buffer;
    void *new_array = realloc(buffer, new_size * sizeof(char));

    //If the reallocation failed :
    if (!new_array) {

        //Fail;
        return false;

    }

    //If the reallocation succeeded :

    //Modify the size;
    size = new_size;

    //Modify the buffer;
    buffer = (char *) new_array;

    //Succeed;
    return true;

}

//---------------------------------------- Getters ----------------------------------------

/*
 * size : this function returns the size of the string : this value is one lesser than the size of the buffer;
 */

uint16_t string::length() const {

    //Return the size;
    return size - (uint16_t) 1;

}


/*
 * size : this function returns the size of the string : this value is one lesser than the size of the buffer;
 */

char string::charAt(uint16_t index) const {

    //If the index is invalid, return '\0'
    if (index > size)
        return 0;

    //Return the required
    return buffer[index];

}


/*
 * Data : this function returns the data of the string;
 */

const char *string::data() const {

    //Return the data pointer;
    return buffer;

}


//--------------------------------------------- Swap ---------------------------------------------

/*
 * swap : this function will swap contents of the two given strings.
 */

void string::swap(string &t0, string &t1) {

    //Swap lengths;
    uint16_t t0_length = t0.size;
    t0.size = t1.size;
    t1.size = t0_length;

    //Swap contents;
    char *t0_buffer = t0.buffer;
    t0.buffer = t1.buffer;
    t1.buffer = t0_buffer;

}

//-------------------------------------- Comparison Operators --------------------------------------

/*
 * Comparison operator : uses the lexicographic order;
 */

bool operator<(const string &left_s, const string &right_s) {

    //Cache both buffers;
    const char *left_array = left_s.data();
    const char *right_array = right_s.data();

    //Compare all chars;
    for (uint16_t i = left_s.length()+(uint16_t)1; i--;) {

        //Cache the left array's current char and incr;
        char l = *(left_array++);
        char r = *(right_array++);

        //If l is terminated, return true;
        if (!l) return true;

        //If l and r are equal (not null), reiterate on next chars;
        if (l == r) continue;

        //As one string is lower than another, a boolean evaluation does the job;
        return (l < r);

    }

    //End case, never happens as strings are null terminated;
    return false;

}


/*
 * operator== : calls cstring::strcmp;
 */

bool string::operator==(const string &s) const {

    return cstring::strcmp(this->data(), s.data());

}


/*
 * Equality operator;
 */

bool operator==(const string &left_s, const string &right_s) { return left_s.operator==(right_s); }

bool operator>(const string &left_s, const string &right_s) { return right_s < left_s; }

bool operator<=(const string &left_s, const string &right_s) { return !(left_s > right_s); }

bool operator>=(const string &left_s, const string &right_s) { return !(left_s < right_s); }


//-------------------------------------- Equality Operators --------------------------------------

bool operator==(const string &left_s, const string &right_s);

bool operator!=(const string &left_s, const string &right_s) { return !(left_s.operator==(right_s)); }

//--------------------------------------------- Memory copy utils ---------------------------------------------


/*
 * memset : sets the data of size_t first bytes of dst to value;
 */

void mmemset(void *dst, uint8_t value, size_t size) {

    //Convert dst to a uint8_t pointer, as arithmetic on void * is illegal;
    uint8_t *d = (uint8_t *)dst;

    //For each byte of data :
    for (;size--;) {

        //Set the byte to the value and increment;
        *(d++) = value;

    }

}


/*
 * memcpy : copies the data of size_t first bytes of data of src to dst;
 */

void mmemcpy(void *dst, const void *src, size_t size) {

    //Convert dst and src to a uint8_t pointer, as arithmetic on void * is illegal;
    uint8_t *d = (uint8_t *)dst;
    uint8_t *s = (uint8_t *)src;

    //For each byte of data :
    for (;size--;) {

        //Set the byte to the value of src at this position and increment both pointers;;
        *(d++) = *(s++);

    }

}


/*
 * symmetric_copy : this function turns dst to the symmetric of src;
 */

void mmemcpy_reverse(void *dst, const void *src, size_t size) {

    //Convert dst and src to a uint8_t pointer, as arithmetic on void * is illegal;
    uint8_t *d = (uint8_t *)dst;
    uint8_t *s = (uint8_t *)src;

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





//-------------------------------------- C style functions --------------------------------------

/*
 * length : this function determines the length of the string (without the \0 comprised);
 */

uint8_t cstring::length(const char *in_buffer) {

    uint8_t char_count = 0;

    //While the current char is not null;
    while (*(in_buffer++)) {

        //Increment the size of the copied buffer.
        char_count++;
    }

    //Return the number of characters before the end of the string
    return char_count;

}


//TODO RE-COMMENT, THIS IS THE OLD FUNCTION

/*
 * count_until_char : this function copies the char sequence in in_buffer in out_buffer char by char,
 *  until it reads the limit char, or \0. This char is not copied to output_buffer.
 *
 *  When the copy is done, it null-terminates the copied sequence.
 *
 */

uint8_t cstring::count_until_char(const char *in_buffer, const char limit_char) {

    uint8_t char_count = 0;
    //Initialise a cache for the current char
    char c = *(in_buffer++);

    //While the current char is not null, or the limit char
    while (c && (c != limit_char)) {

        //Update the current char
        c = *(in_buffer++);

        //Increment the size of the copied buffer.
        char_count++;
    }

    //Return the number of characters before the limit char (or the end of the string
    return char_count;

}


/*
 * lstrip : this function counts the number of time that 'verif_char' is present (consecutively) at the beginning
 *  of in_buffer. For example, with in_buffer containing   '0' '0' '0' '0' '5' '2' '\0', the result of the command
 *      lstrip(in_buffer, '0') will be 4.
 *
 */

uint8_t cstring::lstrip(const char *in_buffer, const char verif_char) {

    //initialise_hardware a counter
    uint8_t counter = 0;

    //Initialise a cache var for the current char
    char c = *(in_buffer++);

    //While the current char is not null, and is equal to 'verif_char'
    while (c && (c == verif_char)) {

        //Increment the presence counter
        counter++;

        //Update the char
        c = *(in_buffer++);

    }

    return counter;

}


/*
 * count_words : this function determines the number of words in a char sequence.
 *
 */

uint8_t cstring::count_words(const char *in_buffer) {

    //Initialise a word counter.
    uint8_t nb_words = 0;

    //initialise_hardware a size for the current word.
    uint8_t size;

    do {

        //Remove unnecessary nb_spaces at the beginning of the char sequence.
        in_buffer += lstrip(in_buffer, ' ');

        //Get the size of the next word (will ne zero only if the sequence is finished).
        size = count_until_char(in_buffer, ' ');

        //increment the data pointer at the end of the word.
        in_buffer += size;

        //Stop working if the size is zero (sequence finished)
    } while (size);

    //Return the number of words.
    return nb_words;

}


/*
 * strcmp : compares the two provided strings, and return true if they are equal;
 */

bool cstring::strcmp(const char *string0, const char *string1) {

    //Comparison loop;
    while(true) {

        //Cache the current char of string0 and increment string0;
        char c = *(string0++);

        //Compare with the current char of string1 and increment string1. If chars are different :
        if (c != *(string1++)) {

            //Found two different chars at the same index -> not equal;
            return false;

        }

        //Now, we are shure that both chars are the same;

        //If the current char is a null termination :
        if (!c) {

            //End of string found for both strings : both strings are the same;
            return true;

        }


        //If not, re-iterate;
    }

    //End case, never happens as strings are null terminated;
    return false;

}



//---------------------------------------- External string Conversion  ----------------------------------------

#define sign_check(i, data)\
    \
    /*If i is negative : */\
    if (i<0) {\
        /*Put a - at the beginning of the string*/\
        *(data++) = '-';\
        \
        /*Take the absolute value of i;*/\
        i = -i;\
        \
    }


#define euint_to_a(i, size, data)\
    /*Declare a temporary array;*/\
    char t[size]{0};\
    \
    _uint##size##_to_as(i, t);\
    \
    /*Copy digits of i in data;*/\
    mmemcpy_reverse(data, t, size);\



/*
 * uint8_to_a : this function will copy the 1 byte unsigned int in the array;
 */

void cstring::uint8_to_a(uint8_t i, char *data) {

    //Copy i's digits in data;
    euint_to_a(i, 8, data);

}


/*
 * int8_to_a : this function will copy the 1 byte signed int in the array;
 */

void cstring::int8_to_a(int8_t i, char *data) {

    //Eventually put a - at the beginning, take the abs of i, and increment data;
    sign_check(i, data);

    //Cache i's abs value;
    uint8_t j = (uint8_t) i;

    //Copy i's digits in data;
    euint_to_a(j, 8, data);

}


/*
 * uint16_to_a : this function will copy the 2 bytes unsigned int in the array;
 */

void cstring::uint16_to_a(uint16_t i, char *data) {

    //Copy i's digits in data;
    euint_to_a(i, 16, data);

}


/*
 * int16_to_a : this function will copy the 2 bytes signed int in the array;
 */

void cstring::int16_to_a(int16_t i, char *data) {

    //Eventually put a - at the beginning, take the abs of i, and increment data;
    sign_check(i, data);

    //Cache i's abs value;
    uint16_t j = (uint16_t) i;

    //Copy i's digits in data;
    euint_to_a(j, 16, data);

}


/*
 * uint32_to_a : this function will copy the 4 bytes unsigned int in the array;
 */

void cstring::uint32_to_a(uint32_t i, char *data) {

    //Copy i's digits in data;
    euint_to_a(i, 32, data);

}


/*
 * int32_to_a : this function will copy the 4 bytes signed int in the array;
 */

void cstring::int32_to_a(int32_t i, char *data) {

    //Eventually put a - at the beginning, take the abs of i, and increment data;
    sign_check(i, data);

    //Cache i's abs value;
    uint32_t j = (uint32_t) i;

    //Copy i's digits in data;
    euint_to_a(j, 32, data);

}


/*
 * uint64_to_a : this function will copy the 8 bytes unsigned int in the array;
 */


void cstring::uint64_to_a(uint64_t i, char *data) {

    euint_to_a(i, 64, data);

}


/*
 * int64_to_a : this function will copy the 2 bytes signed int in the array;
 */

void cstring::int64_to_a(int64_t i, char *data) {

    //Eventually put a - at the beginning, take the abs of i, and increment data;
    sign_check(i, data);

    //Cache i's abs value;
    uint64_t j = (uint64_t) i;

    //Copy i's digits in data;
    euint_to_a(j, 64, data);

}

#undef sign_check
#undef euint_to_a



//---------------------------------------- To string Privates ----------------------------------------

#define uint_to_as(i, data)\
    /*Initialise the size counter*/\
    uint8_t data_length = 0;\
    \
    /*Set the current digit:*/\
    do {\
        /*Increment the size counter;*/\
        data_length++;\
        \
        /*Set the current case of data to the current digit;*/\
        *(data++) = (char) ((uint8_t)(i % (uint8_t) 10) + (uint8_t)48);\
        \
    } while (i/=10);\
    /*While number is superior to zero;*/\
    \
    return data_length;


/*
 * _uint8_to_as : this function will copy the 1 byte unsigned int in the array;
 */

uint8_t cstring::_uint8_to_as(uint8_t i, char *data) {

    //Copy digits of i in data;
    uint_to_as(i, data);

}


/*
 * _uint16_to_as : this function will copy the 1 byte unsigned int in the array;
 */

uint8_t cstring::_uint16_to_as(uint16_t i, char *data) {

    //Copy digits of i in data;
    uint_to_as(i, data);

}


/*
 * _uint32_to_as : this function will copy the 1 byte unsigned int in the array;
 */

uint8_t cstring::_uint32_to_as(uint32_t i, char *data) {

    //Copy digits of i in data;
    uint_to_as(i, data);

}


/*
 * _uint64_to_as : this function will copy the 1 byte unsigned int in the array;
 */

uint8_t cstring::_uint64_to_as(uint64_t i, char *data) {

    //Copy digits of i in data;
    uint_to_as(i, data);

}

#undef uint_to_as
