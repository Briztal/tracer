//
// Created by root on 5/9/18.
//

#ifndef TRACER_TREE_H
#define TRACER_TREE_H


#include <DataStructures/containers/container.h>

/*
 * A tree in the strictest meaning contains a container of sub-trees;
 */

typedef struct {

    container_t children;

} tree_t;

#define EMPTY_TREE {EMPTY_CONTAINER(tree_t)}



//Add a child at the end of our child array;
void tree_append_child(tree_t *parent, tree_t *child);

//Add a child in our container array;
void tree_insert_child(tree_t *parent, container_index_t index, tree_t *child);

//Get the number of children;
inline container_index_t tree_nb_children(tree_t *parent);

//Get a child;
tree_t *get_child(tree_t *parent, container_index_t index);

//Delete a tree;
void tree_delete(tree_t *parent, void (*destructor)(tree_t *));

//Remove a child; Pass a function that will properly destruct the child tree;
void remove_child(tree_t *parent, container_index_t index, void (*destructor)(tree_t *));

//Remove all children; Pass a function that will properly destruct the child tree;
void remove_all(tree_t *parent, void (*destructor)(tree_t *));


#endif //TRACER_TREE_H
