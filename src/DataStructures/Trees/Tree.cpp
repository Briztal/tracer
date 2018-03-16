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

#include "Tree.h"


//--------------------------------------- Initialisation ---------------------------------------


/*
 * Default constructor : initialises the node and the children array;
 */

template<class L, class N>
Tree<L, N>::Tree() : label(L()), node(N()), children() {}


/*
 * Constructor with lvalue node : copies the provided reference and initialises the children array;
 */

template<class L, class N>
Tree<L, N>::Tree(const L &label, const N &node) : label(label), node(node), children() {}


/*
 * Constructor with rvalue node : copies the provided reference and initialises the children array;
 */

template<class L, class N>
Tree<L, N>::Tree(L &&label, N &&node) : label((L &&) label), node((N &&) node), children() {}


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
const N Tree<L, N>::getNode() const {
    return node;
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

    //Return a pointer to the child;
    return children.getElement(index);

}


/*
 * getChildrenNb : returns the number of children of the tree;
 */

template<class L, class N>
uint8_t Tree<L, N>::getChildrenNb() const {
    return children.getSize();
}


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
