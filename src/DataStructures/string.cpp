//
// Created by root on 3/10/18.
//

#include <malloc.h>
#include <Interaction/Interaction.h>
#include "string.h"

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
 * Copy constructor : this function will initialise to the given size, and copy all data;
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
 * Move constructor : this function will initialise to the given size and siphon all data;
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

    //Free the content of the string;
    free(buffer);

}


//---------------------------------------- Primitive types constructors ----------------------------------------

#define define_constructor(type) \
string::string(type i) : string() {Serial.println(#type); *this = i; }

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
    Serial.println("from char");
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

        Serial.println(((uint8_t) frac + (uint8_t)48));
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
 * It first initialises the string to empty, and then updates its content;
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
    uint8_t data_length = _uint##nb_bits##_to_as(i, t);\
    \
    /*Resize the array;*/\
    resizeTo(data_length + 1);\
    \
    /*Copy digits of i in data;*/\
    symmetric_copy(t, buffer, data_length);\
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
    uint8_t data_length = _uint##nb_bits##_to_as((uint##nb_bits##_t)(i), t);\
    \
    /*Resize the array;*/\
    resizeTo(data_length + 2);\
    \
    /*Mark the "-" eventually;*/\
    if (negative) *buffer = '-';\
    \
    /*Copy digits of i in data;*/\
    symmetric_copy(t, buffer + 1, data_length);\
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

//--------------------------------------------- Append Operators ---------------------------------------------

/*
 * add operator, for char;
 */

string &string::operator+=(char c) {

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

string &string::operator+=(const char *src) {

    //Concatenate on the right;
    right_concat(src);

    //Return a reference to us;
    return *this;

}


/*
 * add operator, for string reference;
 */

string &string::operator+=(const string &src) {

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

void string::left_concat(const char *src) {

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

    //First, shift our string of src_size positions on the right;
    memcpy(buffer + src_size, buffer, old_size * sizeof(char));

    //Then, insert src's string in the fred space;
    memcpy(buffer, src, src_size * sizeof(char));

}


/*
 * right_concat : this function concatenates the given char array on our right;
 */

void string::right_concat(const char *src) {

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
    void *new_array = realloc(buffer, new_size);

    //If the reallocation failed :
    if (!new_array) {

        //Log;
        std_out(string("Error in string::resizeTo : the reallocation failed;"));

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
    symmetric_copy(t, data, size);\



/*
 * uint8_to_a : this function will copy the 1 byte unsigned int in the array;
 */

void string::uint8_to_a(uint8_t i, char *data) {

    //Copy i's digits in data;
    euint_to_a(i, 8, data);

}


/*
 * int8_to_a : this function will copy the 1 byte signed int in the array;
 */

void string::int8_to_a(int8_t i, char *data) {

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

void string::uint16_to_a(uint16_t i, char *data) {

    //Copy i's digits in data;
    euint_to_a(i, 16, data);

}


/*
 * int16_to_a : this function will copy the 2 bytes signed int in the array;
 */

void string::int16_to_a(int16_t i, char *data) {

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

void string::uint32_to_a(uint32_t i, char *data) {

    //Copy i's digits in data;
    euint_to_a(i, 32, data);

}


/*
 * int32_to_a : this function will copy the 4 bytes signed int in the array;
 */

void string::int32_to_a(int32_t i, char *data) {

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


void string::uint64_to_a(uint64_t i, char *data) {

    euint_to_a(i, 64, data);

}


/*
 * int64_to_a : this function will copy the 2 bytes signed int in the array;
 */

void string::int64_to_a(int64_t i, char *data) {

    //Eventually put a - at the beginning, take the abs of i, and increment data;
    sign_check(i, data);

    //Cache i's abs value;
    uint64_t j = (uint64_t) i;

    //Copy i's digits in data;
    euint_to_a(j, 64, data);

}

#undef sign_check
#undef euint_to_a


/*
 * symmetric_copy : this function turns dst to the symmetric of src;
 */

void string::symmetric_copy(const char *src, char *dst, uint8_t size) {

    //Increment the dst pointer;
    dst += size - (uint8_t) 1;

    //Nullify the end;
    *(dst + 1) = 0;

    //For each char :
    for (; size--;) {

        //Symmetric copy from src to dst;
        *(dst--) = *(src++);

    }


}


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

uint8_t string::_uint8_to_as(uint8_t i, char *data) {

    //Copy digits of i in data;
    uint_to_as(i, data);

}


/*
 * _uint16_to_as : this function will copy the 1 byte unsigned int in the array;
 */

uint8_t string::_uint16_to_as(uint16_t i, char *data) {

    //Copy digits of i in data;
    uint_to_as(i, data);

}


/*
 * _uint32_to_as : this function will copy the 1 byte unsigned int in the array;
 */

uint8_t string::_uint32_to_as(uint32_t i, char *data) {

    //Copy digits of i in data;
    uint_to_as(i, data);

}


/*
 * _uint64_to_as : this function will copy the 1 byte unsigned int in the array;
 */

uint8_t string::_uint64_to_as(uint64_t i, char *data) {

    //Copy digits of i in data;
    uint_to_as(i, data);

}

#undef uint_to_as


//--------------------------------------------- + operators ---------------------------------------------

/*
#define impl_operators(type)\
string operator+(const string &s0, type i) {\
    string s(s0);\
    s+=string(i);\
    return s;\
}\
string &operator+(string &&s0, type i) {\
    return s0+=string(i);\
}\


impl_operators(uint8_t)

impl_operators(int8_t)

impl_operators(uint16_t)

impl_operators(int16_t)

impl_operators(uint32_t)

impl_operators(int32_t)

impl_operators(uint64_t)

impl_operators(int64_t)
*/

/*
 * Sum operator : sums a char * to the left of src;
 */

string &operator+(const char *c, string &&src) {

    //Concatenate c to the left of src;
    src.left_concat(c);

    //Return a reference to src;
    return src;
}


string operator+(const string &s0, const string &i) {

    string s(s0);
    s += i;
    return s;
}

string operator+(const string &s0, string &&i) {

    string s(s0);
    s += i;
    return s;
}

string &operator+(string &&s0, const string &i) {

    return s0 += i;
}

string &operator+(string &&s0, string &&i) {

    return s0 += i;
}
