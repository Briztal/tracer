//
// Created by root on 3/14/18.
//

#ifndef TRACER_TREE_H
#define TRACER_TREE_H

#include "DataStructures/Containers/DynamicPointerBuffer.h"

template<class L, class N>
class Tree {

    //--------------------------------------- Initialisation ---------------------------------------

public:

    //Default constructor;
    Tree(uint8_t max_children_nb);

    //Class lvalue constructor;
    explicit Tree(const L &label, const N &node, uint8_t max_children_nb);

    //Class rvalue constructor;
    explicit Tree(L &&label, N &&node, uint8_t max_children_nd);

    //Destructor;
    ~Tree();


    //--------------------------------------- Copy constructors ---------------------------------------

public:

    //Copy constructor;
    Tree(const Tree<L, N> &src);

    //Move constructor;
    Tree(Tree<L, N> &&src) noexcept;


    //--------------------------------------- Assignment operators ---------------------------------------

public:

    //Copy assignment operator;;
    Tree<L, N> &operator=(const Tree<L, N> &src);

    //Move assignment operator;;
    Tree<L, N> &operator=(Tree<L, N> &&src) noexcept;


    //--------------------------------------- Getters / setters ---------------------------------------

    //Label getter;
    const L getLabel() const;

    //Node getter;
    const N getNode() const;

    //Node setter;
    void setNode(N new_node);

    //--------------------------------------- children alteration ---------------------------------------

public:

    //Child getter;
    Tree<L, N> *getChild(uint8_t index) const;

    //Child getter;
    Tree<L, N> *getChild(L &label) const;

    //Child adder;
    void addChild(Tree<L, N> *child);

    //Children number getter;
    uint8_t getChildrenNb() const;

    //--------------------------------------- Sort ---------------------------------------

    //Sort the tree;
    void sort();

    //--------------------------------------- Data ---------------------------------------

private:

    //The label will be a reference to the template label;
    L label;

    //The node will be a reference to the template node;
    N node;

    //The array of children;
    DynamicPointerBuffer<Tree<L, N>> children;


};


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
const N Tree<L, N>::getNode() const {
    return node;
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
Tree<L, N> *Tree<L, N>::getChild(L &label) const {

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


#endif //TRACER_TREE_H
