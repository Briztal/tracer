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
    Tree();

    //Class lvalue constructor;
    explicit Tree(const L &label, const N &node);

    //Class rvalue constructor;
    explicit Tree(L &&label, N &&node);

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


    //--------------------------------------- Getters ---------------------------------------

    //Label getter;
    const L getLabel() const;

    //Node getter;
    const N getNode() const;

    //--------------------------------------- children alteration ---------------------------------------

public:

    //Child getter;
    Tree<L, N> *getChild(uint8_t index) const;

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

#include "Tree.cpp"


#endif //TRACER_TREE_H
