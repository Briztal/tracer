//
// Created by root on 5/9/18.
//

#include <DataStructures/Containers/container.h>
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

void tree_insert_child(tree_t *parent, container_index_t index, tree_t *child) {

    //Append the child to the tree;
    container_insert_element(&parent->children, index, &child);

}


/*
 * Get the number of children; As the parent function has already access to this piece of data, the function is inine;
 */

inline container_index_t tree_nb_children(tree_t *parent) {

    //Return the nb of elements in the children container;
    return parent->children.nb_elements;

}


/*
 * Get a child; This function is inline;
 */

tree_t *get_child(tree_t *parent, container_index_t index) {

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
    for (container_index_t child_index = root->children.nb_elements; child_index--;) {

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
    for (container_index_t child_index = root->children.nb_elements; child_index--;) {

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

    //Iterate the destructor on DFS post order;
    tree_DFS_post_order(parent, destructor);

}

/*
 * Remove a child; Pass a function that will properly destruct the child tree;
 */

void remove_child(tree_t *parent, const container_index_t index, void (*const destructor)(tree_t *)) {

    //First, delete the child tree of the memory;
    tree_delete(CONTAINER_GET_TYPED(tree_t*, &parent->children, index), destructor);

}



