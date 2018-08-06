/*
  tree.h - Part of TRACER

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


#ifndef TRACER_TREE_H
#define TRACER_TREE_H

#include <data_structures/containers/non_concurrent/vlarray.h>

/*
 * A tree in the strictest meaning contains a container of sub-trees;
 */

typedef struct {

    vlarray_t children;

} tree_t;

#define EMPTY_TREE(max_nb_children) {EMPTY_VLARRAY(tree_t, max_nb_children)}



//Add a child at the end of our child array;
void tree_append_child(tree_t *parent, tree_t *child);

//Add a child in our container array;
void tree_insert_child(tree_t *parent, size_t index, tree_t *child);

//Get the number of children;
inline size_t tree_nb_children(tree_t *parent) {

    //Return the nb of elements in the children container;
    return parent->children.length;

}

//Get a child;
tree_t *get_child(tree_t *parent, size_t index);

//Delete a tree;
void tree_delete(tree_t *parent, void (*destructor)(tree_t *));

//Remove a child; Pass a function that will properly destruct the child tree;
void remove_child(tree_t *parent, size_t index, void (*destructor)(tree_t *));

//Remove all children; Pass a function that will properly destruct the child tree;
void remove_all(tree_t *parent, void (*destructor)(tree_t *));


#endif //TRACER_TREE_H
