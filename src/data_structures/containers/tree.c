/*
  tree.c - Part of TRACER

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

#include "tree.h"

/*
 * Append a child to our children container;
 */

void tree_append_child(tree_t *parent, tree_t *child) {

    //Append the child to the tree;
    container_append_element(&parent->children, &child);

}


/*
 * Append a child to our children container;
 */

void tree_insert_child(tree_t *parent, size_t index, tree_t *child) {

    //Append the child to the tree;
    container_insert_element(&parent->children, index, &child);

}


/*
 * Get a child; This function is inline;
 */

tree_t *get_child(tree_t *parent, size_t index) {

    //Call the macro get to avoid a syntax mistake;
    return CONTAINER_GET_TYPED(tree_t*, &parent->children, index);

}


//TODO NON RECURSIVE FUNCTION FOR LOW MEMORY SYSTEMS;


/*
 * Call the rec_function on the provided tree, and all its nodes;
 */

void tree_DFS_pre_order(tree_t *root, void (*const traversal_function)(tree_t *)) {

    //First, call the function on the node;
    traversal_function(root);

    //Then, for each sub_tree, call the function;
    for (size_t child_index = root->children.nb_elements; child_index--;) {

        //Cache the child;
        tree_t *child = CONTAINER_GET_TYPED(tree_t *, &root->children, child_index);

        //If the child has sub-nodes :
        if (child->children.nb_elements) {

            //Recursive call on him;
            tree_DFS_pre_order(child, traversal_function);

        }

    }

}


/*
 * Call the rec_function on the provided tree, and all its nodes;
 */

void tree_DFS_post_order(tree_t *root, void (*const traversal_function)(tree_t *)) {

    //First, for each sub_tree, call the function;
    for (size_t child_index = root->children.nb_elements; child_index--;) {

        //Cache the child;
        tree_t *child = CONTAINER_GET_TYPED(tree_t *, &root->children, child_index);

        //If the child has sub-nodes :
        if (child->children.nb_elements) {

            //Recursive call on him;
            tree_DFS_post_order(child, traversal_function);

        }

    }

    //Then, call the function on the node;
    traversal_function(root);

}


/*
 * tree_delete : deletes a tree from the memory. Pass a function that will properly destruct the tree;
 */

void tree_delete(tree_t *parent, void (*const destructor)(tree_t *)) {

    //Iterate the close on DFS post order;
    tree_DFS_post_order(parent, destructor);

}

/*
 * Remove a child; Pass a function that will properly destruct the child tree;
 */

void remove_child(tree_t *parent, const size_t index, void (*const destructor)(tree_t *)) {

    //First, delete the child tree of the memory;
    tree_delete(CONTAINER_GET_TYPED(tree_t*, &parent->children, index), destructor);

}



