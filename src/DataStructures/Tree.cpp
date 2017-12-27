/*
  Tree.h - Part of TRACER

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
  aint32_t with TRACER. If not, see <http://www.gnu.org/licenses/>.

*/

#include <malloc.h>

#include "Tree.h"

Tree::Tree() {

    //Initialise the children pointer to nullptr;
    children = (Tree **) malloc(0);

    //Initialise the number of children to zero;
    nb_children = 0;

}

Tree::~Tree() {

    //Delete each child;
    for (uint8_t child = 0; child < nb_children; child++) {

        delete children[child];

    }

    //Free the child container;
    free(children);

}


/*
 * addChild : this function adds a child to the children array, using realloc;
 */

void Tree::addChild(Tree *child) {


    //Increment the number of children;
    nb_children++;

    //Reallocate the children array
    void *new_ptr = realloc(children, nb_children);

    //If the realloc fails (0 is returned)
    if (!new_ptr) {

        //Restore the number of children;
        nb_children--;

        //Display an error message;
        //TODO ERROR MESSAGE;

        //Fail;
        return;
    }

    //Save the new pointer;
    children = (Tree **) new_ptr;

    //Save the new child;
    children[nb_children - 1] = child;

}


/*
 * getChild : this function returns a pointer to the required child. If the required child doesn't exist,
 *  it returns nullptr;
 */

Tree *Tree::getChild(uint8_t child_index) {

    //If the child doesn't exist, fail;
    if (child_index >= nb_children)
        return nullptr;

    //If the child does exist :

    //Return a pointer to the child;
    return children[child_index];

}
