//
// Created by root on 3/14/18.
//

#include <Kernel/memory.h>

#include <DataStructures/containers/container.h>

#include "hardware_language_abstraction.h"

#include "tstring.h"


/*
 * tstring_from_string : creates a tstring from a string; Moves data;
 */

tstring_t *tstring_from_string(string_t *src) {

    //Move src's string;
    string_t *clone = string_move(src);

    //Allocate some space for the new tstring;
    tstring_t *tstring = (tstring_t *)kernel_malloc(sizeof(tstring_t));

    //Initialise the tstring with the moved string;
    *tstring = EMPTY_TSTRING(clone);

    //Return the new initialised tstring;
    return tstring;

}


/*
 * tstring_from_tstring : create a tstring from a tstring; Moves data;
 */

tstring_t *tstring_from_tstring(tstring_t *src) {

    //Move src's string;
    string_t *clone = string_move(src->string);

    //Allocate some space for the new tstring;
    tstring_t *tstring = (tstring_t *)kernel_malloc(sizeof(tstring_t));

    //Initialise the tstring with the moved string;
    *tstring = EMPTY_TSTRING(clone);

    //Move container data from src to the new tstring;
    container_move((container_t *)tstring, (container_t *)src);

}


/*
 * Concatenate a string to a tstring; Data is moved to the tstring;
 */

void tstring_move_string(tstring_t *const parent, string_t *const child) {

    //First, phagocyte the child string to a new tstring;
    tstring_t *new_element = tstring_from_string(child);

    //Then, add the element to the parent;
    tree_append_child((tree_t *)parent, (tree_t *)new_element);

}


/*
 * Concatenate a tstring to a tstring;
 */

void tstring_move_tstring(tstring_t *const parent, tstring_t *const child) {

    //First, phagocyte the child string to a new tstring;
    tstring_t *new_element = tstring_from_tstring(child);

    //Then, add the element to the parent;
    tree_append_child((tree_t *)parent, (tree_t *)new_element);

}
