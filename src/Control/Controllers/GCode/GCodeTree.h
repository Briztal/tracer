/*
  GCodeTree.h - Part of TRACER

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

/*
 * GCodeTree : This class will be used to parse the GCode packets we will receive.
 *
 *  The GCode language will be stored in a tree, build from configuration file.
 *
 *  Every tree will contain a char, a command, and child trees, representing sub_commands.
 */

#ifndef TRACER_GCodeNode_H
#define TRACER_GCodeNode_H

#include <Config/control_config.h>

#ifdef ENABLE_GCODE_INTERFACE

#include <hardware_language_abstraction.h>
#include <TaskScheduler/task_state_t.h>

class GCodeTree {


public:

    //Constructor
    GCodeTree(char name, uint8_t nb_children, task_state_t (*const f)(char *));

    //Destructor.
    ~GCodeTree();

    //The name of the tree
    const char name;

    //The number of nb_children trees
    const uint8_t nb_children;

    //The function called from this tree.
    task_state_t (*const function)(char *);

    //Assign a sub_tree. Fails if the child is already assigned.
    void set_child(uint8_t id, GCodeTree *child, bool *success);

    //Get a pointer to a child. Fails if the child is not assigned.
    const GCodeTree *get_child (uint8_t id, bool *success) const;

private:

    /*
     * As verifying that a sub_tree is allocated before accessing it, we will store our nb_children in structs.
     */

    struct gcode_child_container_t {

        //The allocation flag;
        bool allocated = false;

        //The tree;
        GCodeTree *tree = nullptr;
    };

    //The array of structs containing nb_children.
    gcode_child_container_t *children;

};


#endif //TRACER_NODE_H

#endif