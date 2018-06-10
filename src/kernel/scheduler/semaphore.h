/*
  semaphore.h Part of TRACER

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
#ifndef TRACER_SEMAPHORE_H
#define TRACER_SEMAPHORE_H


#include <data_structures/containers/container.h>



/*
 * The Semaphore library.
 *
 *  It is used by the kernel to synchronise processes.
 *
 *  ----------------------- WARNING -----------------------
 *
 *  Semaphores synchronise processes. If they are not called from a process_t (in the ThreadManager's meaning), the
 *  The semaphore call will have no effect, as semaphores lock and unlock processes.
 *
 *  Semaphore can't (lol) lock interrupts;
 *
 *  A semaphore call from an interrupt will be detected, and ignored.
 *
 *  As a consequence, interrupts have full control on drivers, and can use them without any restrictions, but it
 *  can also cause damage, as it can shortcut potentially ordered procedures.
 *
 *  You must take that in consideration when writing your ISRs. Use it carefully;
 */

//TODO COMPOSITION;

typedef struct {

    //The allocation counter, scheduler_initialised to the maximum number of concurrent code execution;
    uint16_t allocation_counter;

    //The lock container. Contains pointers of all locked threads;
    container_t locked_threads;

} semaphore_t;


#define SEMAPHORE(nb_accesses) {.allocation_counter = nbaccesses, .locked_threads = EMPTY_CONTAINER(process_t *)}


//The P function : requires the execution;
void sem_wait(semaphore_t *semaphore);

//The V function : lets an execution happen;
void sem_post(semaphore_t *semaphore);

//The deletion function. Unlocks all threads;
void sem_delete(semaphore_t *semaphore);


#endif //TRACER_SEMAPHORE_H
