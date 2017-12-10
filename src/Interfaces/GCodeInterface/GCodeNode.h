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

class GCodeNode {


public:

    GCodeNode(char name, uint8_t sub_nodes_nb, task_state_t (*const f)(char *));

    const char name;

    const uint8_t sub_nodes_nb;

    GCodeNode **sub_nodes;

    task_state_t (*const function)(char *);


};


#endif //TRACER_NODE_H

