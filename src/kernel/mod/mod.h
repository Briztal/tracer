/*
  mod.h Part of TRACER

  Copyright (c) 2018 Raphaël Outhier

  TRACER is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  TRACER is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  aint32_t with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/


#ifndef TRACER_MOD_H
#define TRACER_MOD_H

//Add a module to the kernel;
void mod_add(const char *name, void (*init)(), void (*exit));

//Remove a module from the kernel;
void mod_remove(const char *name);

//Load all automatically loadable modules;
void mod_autoload();


#endif //TRACER_MOD_H
