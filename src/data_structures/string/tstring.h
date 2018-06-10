/*
  tstring.h - Part of TRACER

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


#ifndef TRACER_TSTRINGCLASS_H
#define TRACER_TSTRINGCLASS_H


#include <data_structures/string/string.h>
#include "data_structures/containers/tree.h"


typedef struct {

    //To allow composition, start by a tree structure;
    tree_t tree_data;

    string_t *string;

} tstring_t;

//The struct default initialiser;
#define EMPTY_TSTRING(s) (tstring_t ) {.tree_data = EMPTY_TREE(10), .string = (s)}

//The struct default initialiser;
#define EMPTY_SIZED_TSTRING(max_nb_children, s) (tstring_t ) {.tree_data = EMPTY_TREE(max_nb_children), .string = (s)}


//Create a tstring from a string; Moves data;
tstring_t *tstring_from_string(string_t *src);

//Create a tstring from a tstring; moves data;
tstring_t *tstring_from_tstring(tstring_t *src);


//Concatenate a string to a tstring;
void tstring_move_string(tstring_t *parent, string_t *child);

//Concatenate a tstring to a tstring;
void tstring_move_tstring(tstring_t *parent, tstring_t *child);



#endif //TRACER_TSTRING_H
