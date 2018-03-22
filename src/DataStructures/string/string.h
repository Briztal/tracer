//
// Created by root on 3/10/18.
//

#ifndef TRACER_BASICSTRING_H
#define TRACER_BASICSTRING_H

#include "stdint.h"

//A macro to ease the redaction of string;
#define st(i) string(i)

class string {


    //-------------------------------------- Required constructors --------------------------------------

public:

    //Default constructor;
    string();

    //Copy constructor;
    string(const string &src);

    //Move constructor;
    string(string &&src) noexcept;

    //Destructor;
    ~string();


    //-------------------------------------- Primitive types constructors --------------------------------------

public:

    /*
     * The following constructors are trivial constructor, that set the string empty
     *  and call the appropriate setTo function;
     */

    //Numbers constructors;

    string(uint8_t i);

    string(int8_t i);

    string(uint16_t i);

    string(int16_t i);

    string(uint32_t i);

    string(int32_t i);

    string(uint64_t i);

    string(int64_t i);


    //Float constructors;

    string(float f);

    string(float f, uint8_t resolution);

    void setTo(float f, uint8_t resolution);


    //Char constructors;

    string(char i);

    //Char array constructor;
    string(const char *src);

    //Char with size constructor;
    string(const char *src, uint16_t size);


    //-------------------------------------- Assignment Operators --------------------------------------

public:

    //Copy assignment operator;
    string &operator=(const string &src);

    //Move assignment constructor;
    string &operator=(string &&src) noexcept;

    //Char set operator;
    string &operator=(char i);

    //array set operator;
    string &operator=(const char *);

    /*
     * Following functions assign the string as the string representation of basic number types;
     */

    string &operator=(uint8_t i);

    string &operator=(int8_t i);

    string &operator=(uint16_t i);

    string &operator=(int16_t i);

    string &operator=(uint32_t i);

    string &operator=(int32_t i);

    string &operator=(uint64_t i);

    string &operator=(int64_t i);

    string &operator=(float f);

    bool operator==(const string &s) const ;

    //-------------------------------------- Size modification --------------------------------------

protected:

    //Resize the string;
    bool resizeTo(uint16_t new_size);


    //-------------------------------------- Getters --------------------------------------

public:

    //Return the length of the string;
    uint16_t length() const;

    //Return the char at the given index;
    char charAt(uint16_t index) const;

    //Return a pointer to the first char;
    const char *data() const;


    //-------------------------------------- Data swap --------------------------------------

public:

    //Copy and swap idiom;
    static void swap(string &t0, string &t1);


    //-------------------------------------- Data --------------------------------------

protected:

    //The current length of the string;
    uint16_t size;

    //The content of the string;
    char *buffer;

};


//-------------------------------------- Comparison Operators --------------------------------------

bool operator<(const string &left_s, const string &right_s);

bool operator>(const string &left_s, const string &right_s);

bool operator<=(const string &left_s, const string &right_s);

bool operator>=(const string &left_s, const string &right_s);


//-------------------------------------- Equality Operators --------------------------------------

bool operator==(const string &left_s, const string &right_s);

bool operator!=(const string &left_s, const string &right_s);


//-------------------------------------- Memory Access Utilities; --------------------------------------

#include "malloc.h"

//Set a block of bytes at a given value;
void mmemset(void *dst, uint8_t value, size_t size);

//Copy a block of bytes in another;
void mmemcpy(void *dst, const void *src, size_t size);

//Copy the given array to the dst array in the symetric order;
void mmemcpy_reverse(void *dst, const void *src, size_t size);


//-------------------------------------- C style strings functions  ----------------------------------

namespace cstring {

    //-------------------------------------- various conversions ----------------------------------

    //Get the number of times that 'verif_char' is consecutively present, at the beginning of the char sequence.
    uint8_t lstrip(const char *in_buffer, const char verif_char);

    //Get the length of a string;
    uint8_t length(const char *in_buffer);

    //Count thenumber of chars before the limit_char, or the end of the string;
    uint8_t count_until_char(const char *in_buffer, char limit_char);

    //Get the number of words in the string;
    uint8_t count_words(const char *in_buffer);

    //Compare the two strings, and return true if they are the same;
    bool strcmp(const char *string0, const char *string1);


    //-------------------------------------- To string conversion --------------------------------------

    /*
     * Following functions set the given array as the string representation of basic number types;
     */

    void uint8_to_a(uint8_t i, char *data);

    void int8_to_a(int8_t i, char *data);

    void uint16_to_a(uint16_t i, char *data);

    void int16_to_a(int16_t i, char *data);

    void uint32_to_a(uint32_t i, char *data);

    void int32_to_a(int32_t i, char *data);

    void uint64_to_a(uint64_t i, char *data);

    void int64_to_a(int64_t i, char *data);


    uint8_t _uint8_to_as(uint8_t i, char *data);

    uint8_t _uint16_to_as(uint16_t i, char *data);

    uint8_t _uint32_to_as(uint32_t i, char *data);

    uint8_t _uint64_to_as(uint64_t i, char *data);

}

#endif //TRACER_BASICSTRING_H
