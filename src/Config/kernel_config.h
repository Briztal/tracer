/*
  kernel_config.h - Part of TRACER

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


/*
 * -------------------------------------------------- Task Scheduler ---------------------------------------------------
 */

//The following line defines the task pool size
#define TASK_POOL_SIZE 20


/*
 * The following block defines the number of task sequences, their ID, their name and their size
 *  (the maximal number of pending task they can contain).
 */

//TASK_SEQUENCE(id, size)
#ifdef TASK_SEQUENCE

TASK_SEQUENCE(20)

#endif



/*
 * --------------------------------------------------- Event Manager ---------------------------------------------------
 */


//The maximum number of events;
#define MAX_NB_EVENTS 10

//The maximum number of tasks per event;
#define MAX_TASKS_PER_EVENT 10


/*
 * -------------------------------------------------- Task Programmer --------------------------------------------------
 */

//The maximum number of simultaneous programmed tasks;
#define MAX_PROGRAMMED_TASKS 10