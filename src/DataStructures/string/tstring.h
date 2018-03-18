//
// Created by root on 3/14/18.
//

#ifndef TRACER_TSTRINGCLASS_H
#define TRACER_TSTRINGCLASS_H


#include <DataStructures/Containers/DynamicPointerBuffer.h>
#include <DataStructures/string/string.h>


class tstring : private DynamicPointerBuffer<string> {

    //------------------------------------------- Constructors -------------------------------------------

public:

    //Default constructor : max number of string defaulted to 10;
    tstring();

    //Constructor with size : max number of string provided;
    explicit tstring(uint8_t max_strings);

    //Constructor with array : max number of string provided;
    tstring(const char *c);

    //Constructor with string lvalue;
    tstring(const string &);

    //Constructor with string rvalue;
    tstring(const string &&);

    //Destructor;
    ~tstring() override;

    //Clear function;
    void clear() override;
    

    //------------------------------------------- Copy Constructors -------------------------------------------

public:

    //Copy constructor;
    tstring(const tstring &src);

    //Move constructor;
    tstring(tstring &&src) noexcept;

    //------------------------------------------- Assignment operators -------------------------------------------

public:

    //Copy assignment operator;
    tstring &operator=(const tstring &src);

    //Move assignment operator;
    tstring &operator=(tstring &&src) noexcept;


    //------------------------------------------- Append Operators -------------------------------------------

public:

    //Concatenation operator, for lvalues;
    tstring &operator+=(const string &src);

    //Concatenation operator, for rvalues;
    tstring &operator+=(string &&src);

    //Concatenation operator for tstring lvalues;
    tstring &operator+=(tstring &&src);

    //Concatenation on the right of an rvalue;
    void rightConcatenation(tstring &&src);

    //Concatenation on the left of an rvalue;
    void leftConcatenation(tstring &&src);


    //------------------------------------------- Internal Data -------------------------------------------

private:

    //A flag, set if the final string is generated;
    bool string_generated;

    //The final pointer to the string;
    char *final_data;


    //------------------------------------------- Data Getter -------------------------------------------

public:

    //Get the data pointer;
    const char *data();

    uint16_t length();

    //------------------------------------------- string Data -------------------------------------------

private:

    //Generate the string data;
    void generateData();

    //Delete the generated data;
    void deleteData();

    //Swap strings;
    void swap(tstring &a, tstring &b);

};


/*
 * External operators : do not modify strings (except rvalues);
 */

tstring operator+(const tstring &s0, const string &i);

tstring operator+(const tstring &s0, string &&i);

tstring &&operator+(tstring &&s0, const string &i);

tstring &&operator+(tstring &&s0, string &&i);

tstring &&operator+(string &&i, tstring &&t);


#endif //TRACER_TSTRING_H
