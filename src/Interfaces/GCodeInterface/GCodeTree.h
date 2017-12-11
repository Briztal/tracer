/*
  Node.h - Part of TRACER

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
  along with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/


#include <config.h>

#ifndef TRACER_GCodeNode_H
#define TRACER_GCodeNode_H

#include <hardware_language_abstraction.h>
#include <TaskScheduler/task_state_t.h>

class GCodeTree {


public:

    //Constructor
    GCodeTree(char name, uint8_t sub_nodes_nb, task_state_t (*const f)(char *));

    //Destructor.
    ~GCodeTree();

    //The name of the tree
    const char name;

    //The function called from this tree.
    task_state_t (*const function)(char *);

    //The number of children trees
    const uint8_t nb_children;

    //Assign a sub_tree. Fails if the child is already assigned.
    void set_child(uint8_t id, GCodeTree *child, bool *success);

    //Get a pointer to a child. Fails if the child is not assigned.
    const GCodeTree *get_child (uint8_t id, bool *success) const;


private:

    //As verifying that a sub_tree is allocated before accessing it, we will store our children in structs.

    struct child_container_t {

        //The allocation flag;
        bool allocated = false;

        //The tree;
        GCodeTree *tree;
    };

    //The array of structs containing children.
    child_container_t *children;

};


#endif //TRACER_NODE_H

