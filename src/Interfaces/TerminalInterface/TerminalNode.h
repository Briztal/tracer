/*
  TerminalNode.h - Part of TRACER

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

#ifdef ENABLE_TERMINAL_INTERFACE


#ifndef TRACER_TERMINALNODE_H
#define TRACER_TERMINALNODE_H

#include <hardware_language_abstraction.h>

class TerminalNode {

public :

    TerminalNode(string_t *name, uint8_t sub_nodes_nb, string_t *desc_log, string_t *args_log, bool (*f)(char*, uint8_t));

    const String *name;

    const uint8_t sub_nodes_nb;

    TerminalNode **sub_nodes;

    const string_t *args_log;

    const string_t *desc_log;

    bool (*function)(char*, uint8_t);

};


#endif //TRACER_TERMINALNODE_H
