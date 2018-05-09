//
// Created by root on 3/14/18.
//

#ifndef TRACER_TSTRINGCLASS_H
#define TRACER_TSTRINGCLASS_H


#include <DataStructures/Containers/DynamicPointerBuffer.h>
#include <DataStructures/string/string.h>
#include "tree.h"


typedef struct {

    //F
    tree_t tree_data;

} tstring_t;
/*
 * External operators : do not modify strings (except rvalues);
 */

tstring operator+(const tstring &s0, const string &i);

tstring operator+(const tstring &s0, string &&i);

tstring &&operator+(tstring &&s0, const string &i);

tstring &&operator+(tstring &&s0, string &&i);

tstring &&operator+(string &&i, tstring &&t);


#endif //TRACER_TSTRING_H
