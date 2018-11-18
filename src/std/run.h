/*
  run.h Part of TRACER

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

#ifndef TRACER_RUN_H
#define TRACER_RUN_H

/*
 * run_exec : the process execution function;
 *
 * 	Executes the process function, with the process args, and returns;
 */

static void run_exec();


/**
 * run_exit : the process exit function. Called automatically when prc_entry returns;
 *
 * 	Marks the process terminated and triggers the preemption;
 *
 * 	If preemption fails, an internal error has occurred, and a kernel panic is generated;
 */

static void run_exit();


#endif TRACER_RUN_H

