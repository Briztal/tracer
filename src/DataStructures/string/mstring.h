//
// Created by root on 3/14/18.
//

#ifndef TRACER_MSTRING_H
#define TRACER_MSTRING_H


#include "string.h"

class mstring : public string {
    

    //-------------------------------------- Append Operators --------------------------------------

public:

    //char append operator;
    mstring &operator+=(char);

    //Copcatenation operator;
    mstring &operator+=(const char *src);

    //append operator;
    mstring &operator+=(const mstring &src);


    //-------------------------------------- Concatenation --------------------------------------

public:

    //Concatenate a char array to the right;
    void right_concat(const char *src);

    //Concatenate a char array to the left;
    void left_concat(const char *src);
    
    
};


/*
 * External operators : do not modify strings (except rvalues);
 */

//mstring &operator+(const char *c, mstring &&src);

mstring operator+(const mstring &s0, const mstring &i);

mstring &&operator+(const mstring &s0, mstring &&i);

mstring &&operator+(mstring &&s0, const mstring &i);

mstring &&operator+(mstring &&s0, mstring &&i);



#endif //TRACER_MSTRING_H
