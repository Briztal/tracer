/*
  TerminalTree.h - Part of TRACER

  Copyright (c) 2017 Raphaël Outhier

  TRACER is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the FreT Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  TRACER is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/

/*
 * TerminalTree : This class will be used to parse the terminal packets we will receive.
 *
 *  The terminal language will be stored in a tree, build from configuration file.
 *
 *  Every tree will contain a word, a description, a command, and child trees, representing sub_commands.
 */

#ifndef TRACER_NODE_H
#define TRACER_NODE_H

#include <config.h>

#ifdef ENABLE_TERMINAL_INTERFACE

#include <hardware_language_abstraction.h>
#include <TaskScheduler/task_state_t.h>


class TerminalTree {

public:

    //Constructor
    TerminalTree(string_t *name, uint8_t nb_children, string_t *description, task_state_t (*const f)(char *));

    //Destructor
    ~TerminalTree();

    //The name of the command : a single non-spaced word.
    const String *const name;

    //The description
    const string_t *const description;

    //The function to execute
    task_state_t (*const function)(char *);

    //The number of children
    const uint8_t nb_children;

    //Assign a sub_tree. Fails if the child is already assigned.
    void set_child(uint8_t id, TerminalTree *child, bool *success);

    //Get a pointer to a child. Fails if the child is not assigned.
    const TerminalTree *get_child (uint8_t id, bool *success) const;


private:

    /*
     * As verifying that a sub_tree is allocated before accessing it, we will store our children in structs.
     */

    struct terminal_child_container_t {

        //The allocation flag;
        bool allocated = false;

        //The tree;
        TerminalTree *tree;
    };

    //The array of structs containing children.
    terminal_child_container_t *children;

};


#endif //TRACER_NODE_H

#endif