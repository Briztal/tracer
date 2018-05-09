//
// Created by root on 3/14/18.
//

#ifndef TRACER_TSTRINGCLASS_H
#define TRACER_TSTRINGCLASS_H


#include <DataStructures/Containers/DynamicPointerBuffer.h>
#include <DataStructures/string/string.h>
#include "DataStructures/Containers/tree.h"


typedef struct {

    //To allow composition, start by a tree structure;
    tree_t tree_data;

    string_t *string;

} tstring_t;

//The struct default initialiser;
#define EMPTY_TSTRING(s) {.tree_data = EMPTY_TREE, .string = (s)}


//Create a tstring from a string; Moves data;
tstring_t *tstring_from_string(string_t *src);

//Create a tstring from a tstring; moves data;
tstring_t *tstring_from_tstring(tstring_t *src);


//Concatenate a string to a tstring;
void tstring_move_string(tstring_t *parent, string_t *child);

//Concatenate a tstring to a tstring;
void tstring_move_tstring(tstring_t *parent, tstring_t *child);



#endif //TRACER_TSTRING_H
