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


    //-------------------------------------- External string conversion --------------------------------------

public:

    /*
     * Following functions set the given array as the string representation of basic number types;
     */

    static void uint8_to_a(uint8_t i, char *data);

    static void int8_to_a(int8_t i, char *data);

    static void uint16_to_a(uint16_t i, char *data);

    static void int16_to_a(int16_t i, char *data);

    static void uint32_to_a(uint32_t i, char *data);

    static void int32_to_a(int32_t i, char *data);

    static void uint64_to_a(uint64_t i, char *data);

    static void int64_to_a(int64_t i, char *data);


private:

    static uint8_t _uint8_to_as(uint8_t i, char *data);

    static uint8_t _uint16_to_as(uint16_t i, char *data);

    static uint8_t _uint32_to_as(uint32_t i, char *data);

    static uint8_t _uint64_to_as(uint64_t i, char *data);


private:

    //Copy the given array to the dst array in the symetric order;
    static void symmetric_copy(const char *src, char *dst, uint8_t size);

};


void mmemset(void *dst, uint8_t value, size_t size);

void mmemcpy(void *dst, const void *src, size_t size);


#endif //TRACER_BASICSTRING_H
