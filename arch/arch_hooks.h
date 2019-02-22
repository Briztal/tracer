/*
  khooks.h Part of TRACER

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

/*
 * Functions declared in this file that can be referenced by different parts of the khal to call each other;
 */

#ifndef TRACER_ARCH_HOOKS_H
#define TRACER_ARCH_HOOKS_H

/*---------------------------------------------------- khal hooks ----------------------------------------------------*/

/*Entry point of the executable point; calls __chip_initialisation; never returns; mentioned for doc, do not hook it;*/
/*void __executable_entry();*/

/*Chip initialisation function; called by __executable_entry; calls __kernel_entry; never returns;*/
void __chip_initialisation();



#endif /*TRACER_ARCH_HOOKS_H*/
