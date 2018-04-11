/*
  ProgrammedTask.h - Part of TRACER

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
  aint32_t with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <Kernel/Scheduler/Systick.h>
#include "ProgrammedTask.h"

/*
 * Constructor;
 */

ProgrammedTask::ProgrammedTask(task_function_t function, uint32_t offset, uint32_t period, uint32_t nb_executions)
        : nextExecutionTime(offset + Systick::milliseconds()), remainingExecutions(nb_executions),
          function(function), period(period), executionFlag(false) {}
