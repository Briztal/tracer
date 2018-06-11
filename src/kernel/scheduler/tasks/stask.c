/*
  task.c Part of TRACER

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

#include "stask.h"

#include <malloc.h>
#include <string.h>
#include <kernel/kernel.h>


//---------------------- Public Functions ----------------------



/*
 * stask_delete : delete entirely a task's content; Must be heap-stored;
 */

void stask_delete(stask_t *task_p) {

    //Free arguments;
    kernel_free((void *) task_p->args);

    //Free the task itself;
    kernel_free(task_p);

}
