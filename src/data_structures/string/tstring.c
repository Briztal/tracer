/*
  tstring.c - Part of TRACER

  Copyright (c) 2017 Raphaël Outhier

  TRACER is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  TRACER is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  aint32_t with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "tstring.h"

#include <kernel/kernel.h>

/*
 * tstring_from_string : creates a tstring from a string; Moves data;
 */

tstring_t *tstring_from_string(string_t *src) {

    //Move src's string;
    string_t *clone = string_move(src);

    //Initialise a stack version of the clone;
    tstring_t init = EMPTY_TSTRING(clone);

    //Allocate some space for the new tstring and initialise it;
    tstring_t *tstring = kernel_malloc_copy(sizeof(tstring_t), &init);

    //Return the new scheduler_initialised tstring;
    return tstring;

}


/*
 * tstring_from_tstring : create a tstring from a tstring; Moves data;
 */

tstring_t *tstring_from_tstring(tstring_t *src) {

    //Move src's string;
    string_t *clone = string_move(src->string);

    //Initialise the tstring with the moved string;
    tstring_t init = EMPTY_TSTRING(clone);

    //Allocate some space for the new tstring and initialise it;
    tstring_t *tstring = (tstring_t *)kernel_malloc_copy(sizeof(tstring_t), &init);

    //Move container data from src to the new tstring;
    container_move((container_t *)tstring, (container_t *)src);

    //Return moved tstring;
    return tstring;

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
