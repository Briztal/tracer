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


#ifndef TRACER_TREE_CPP
#define TRACER_TREE_CPP

#include "Tree.h"
//--------------------------------------- Initialisation ---------------------------------------


/*
 * Default constructor : initialises the node and the children array;
 */

template<class L, class N>
Tree<L, N>::Tree(uint8_t max_children_nb) : label(L()), node(N()), children(max_children_nb) {}


/*
 * Constructor with lvalue node : copies the provided reference and initialises the children array;
 */

template<class L, class N>
Tree<L, N>::Tree(const L &label, const N &node, uint8_t max_children_nb) : label(label), node(node), children(max_children_nb) {}


/*
 * Constructor with rvalue node : copies the provided reference and initialises the children array;
 */

template<class L, class N>
Tree<L, N>::Tree(L &&label, N &&node, uint8_t max_children_nb) : label((L &&) label), node((N &&) node), children(max_children_nb) {}


/*
 * Destructor : as node and child container are references, will be deleted automatically;
 */

template<class L, class N>
Tree<L, N>::~Tree() = default;


//--------------------------------------- Copy constructors ---------------------------------------

/*
 * Copy constructor : copies node and children array;
 */

template<class L, class N>
Tree<L, N>::Tree(const Tree<L, N> &src) : label(src.label), node(src.node), children(src.children) {}


/*
 * Move constructor : moves node and children array;
 */

template<class L, class N>
Tree<L, N>::Tree(Tree<L, N> &&src): label((L &&) src.label), node((N &&) src.node),
                                    children((DynamicPointerBuffer<Tree<L, N>> &&) src.children) {}


//--------------------------------------- Assignment operators ---------------------------------------


/*
 * Copy assignment operator : copies the node and the child array;
 */

template<class L, class N>
Tree<L, N> &Tree<L, N>::operator=(const Tree<L, N> &src) {

    //Call the copy assignment operator on node and children array;
    label = src.label;
    node = src.node;
    children = src.children;

}


/*
 * Move assignment operator : moves the node and the child array;
 */

template<class L, class N>
Tree<L, N> &Tree<L, N>::operator=(Tree<L, N> &&src) {

    //Call the move assignment operator on node and children array;
    label = (L &&) src.label;
    node = (N &&) src.node;
    children = (DynamicPointerBuffer<Tree<L, N>> &&) src.children;

}

//--------------------------------------- Getters ---------------------------------------

/*
 * getLabel : returns a copy of the tree's label;
 */

template<class L, class N>
const L Tree<L, N>::getLabel() const {
    return label;
}


/*
 * getNode : returns a copy of the tree's node;
 */

template<class L, class N>
N *Tree<L, N>::getNode() {
    return &node;
}


/*
 * setNode : updates the node with the new one;
 */

template<class L, class N>
void Tree<L, N>::setNode(N new_node) {
    node = new_node;
}


//--------------------------------------- children alteration ---------------------------------------

/*
 * addChild : this function adds a child to the children array, using realloc;
 */

template<class L, class N>
void Tree<L, N>::addChild(Tree<L, N> *child) {

    children.add(child);

}


/*
 * getChild : this function returns a pointer to the required child. If the required child doesn't exist,
 *  it returns nullptr;
 */
template<class L, class N>
Tree<L, N> *Tree<L, N>::getChild(uint8_t index) const {

    //If the index is invalid, fail;
    if (index >= children.getSize())
        return nullptr;

    //Return a pointer to the child;
    return children.getElement(index);

}


/*
 * getChild : this function returns a pointer to the child with . If the required child doesn't exist,
 *  it returns nullptr;
 */

template<class L, class N>
Tree<L, N> *Tree<L, N>::getChild(const L &label) const {

    //Search all children :
    for (uint8_t child_index = children.getSize(); child_index--;) {

        //Cache the child's pointer;
        Tree<L, N> *child = children.getElement(child_index);

        //If labels match, return the child's pointer;
        if (child->label == label)
            return child;

    }

    //If no child with the label was found, return an null pointer;;
    return nullptr;
}


/*
 * getChildrenNb : returns the number of children of the tree;
 */

template<class L, class N>
uint8_t Tree<L, N>::getChildrenNb() const {
    return children.getSize();
}



//--------------------------------------- Recursive call ---------------------------------------template<class L, class N>


/*
 * Recursive call : calls the given function the tree and every child and their children;
 */

template<class L, class N>
void Tree<L, N>::recursiveCall(void (*function)(Tree<L, N> *)) {

    //Call the function on our tree;
    function(this);

    //For each child :
    for (uint8_t child_index = children.getSize(); child_index--;) {

        //Recursive call;
        children.getElement(child_index)->recursiveCall(function);

    }

};


//--------------------------------------- Sort ---------------------------------------

template<class L, class N>
void Tree<L, N>::sort() {

    //Sort our sons array;
    children.sort();

    //For each of our children :
    for (uint8_t i = children.getSize(); i--; ) {

        //Sort the child;
        children.getElement(i)->sort();

    }

}


template<class L, class N>
bool Tree<L, N>::operator<(Tree<L,N> tree) {
    return this->label < tree.label;
};


#endif //TRACER_TREE_CPP