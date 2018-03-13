//
// Created by root on 3/10/18.
//

#ifndef TRACER_TSTRING_H
#define TRACER_TSTRING_H

#include "stdint.h"

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

    /*
     * The following constructors are trivial constructor, that set the string empty
     *  and call the appropriate setTo function;
     */

    //Int constructors;

    explicit string(uint8_t i);

    explicit string(int8_t i);

    explicit string(uint16_t i);

    explicit string(int16_t i);

    explicit string(uint32_t i);

    explicit string(int32_t i);

    explicit string(uint64_t i);

    explicit string(int64_t i);

    explicit string(char i);

    //Float constructor;
    string(float f, uint8_t resolution);

    //Char array constructor;
    explicit string(const char *src);


    //-------------------------------------- Assignment Operators --------------------------------------


    //Copy assignment operator;
    string &operator=(string src);

    //Move assignment constructor;
    string &operator=(string &&src) noexcept;

    //Char set operator;
    string& operator=(char i);

    //array set operator;
    string &operator=(const char *);

    /*
     * Following functions assign the string as the string representation of basic number types;
     */

    string& operator=(uint8_t i);

    string& operator=(int8_t i);

    string& operator=(uint16_t i);

    string& operator=(int16_t i);

    string& operator=(uint32_t i);

    string& operator=(int32_t i);

    string& operator=(uint64_t i);

    string& operator=(int64_t i);

    string& operator=(float f, uint8_t resolution);


    //-------------------------------------- Append Operators --------------------------------------

    //char append operator;
    string &operator+=(char);

    //Copcatenation operator;
    string &operator+=(const char *src);

    //Copy assignment operator;
    string &operator+=(string &src);

    //Move assignment operator;
    string &operator+=(string &&src);


    //-------------------------------------- Concatenation --------------------------------------

    //Concatenate a char array to the right;
    void right_concat(const char *src);

    //Concatenate a char array to the left;
    void left_concat(const char *src);


    //-------------------------------------- Size modification --------------------------------------

private:

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

    //Copy and swap idiom;
    static void swap(string &t0, string &t1);


    //-------------------------------------- Data --------------------------------------

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

    static void float_to_a(float i, char *data);


private:

    static uint8_t _uint8_to_as(uint8_t i, char *data);

    static uint8_t _uint16_to_as(uint16_t i, char *data);

    static uint8_t _uint32_to_as(uint32_t i, char *data);

    static uint8_t _uint64_to_as(uint64_t i, char *data);


private:

    //Copy the given array to the dst array in the symetric order;
    static void symmetric_copy(const char *src, char *dst, uint8_t size);

};


/*
 * External operators : do not modify strings (except rvalues);
 */

string &operator+(const char *c, string &&src);

string &operator+(const string &&s0, const string &&s1);

string &operator+(const string &s0, const string &&s1);

string &operator+(const string &&s0, const string &s1);

string &operator+(const string &&s0, const char *c);

string &operator+(const string &&s0, const string &src1);

#define def_operators(type)\
string operator+(const string &s0, type i);\
string &operator+(const string &&s0, type i);

def_operators(uint8_t)

def_operators(int8_t)

def_operators(uint16_t)

def_operators(int16_t)

def_operators(uint32_t)

def_operators(int32_t)

def_operators(uint64_t)

def_operators(int64_t)

def_operators(const string &)

def_operators(float)


#endif //TRACER_TSTRING_H
