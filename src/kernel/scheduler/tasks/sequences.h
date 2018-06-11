/*
  sequences.h Part of TRACER

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

#ifndef TRACER_TASKSEQUENCER_H
#define TRACER_TASKSEQUENCER_H


#include "stdint.h"

#include "stdbool.h"

#include <kernel/scheduler/tasks/stask.h>

/*
 * The sequence task type is composed of a task and of a sequence identifier;
 */

typedef struct {

    //Task composition;
    stask_t task;

    //Sequence identifier;
    uint8_t sequence_id;

} sequence_task_t;


//--------------------- Initialisation ---------------------

//Set the pool size;
void sequences_initialise(size_t unordered_sequence_size, size_t nb_ordered_sequences,
                          const size_t *ordered_sizes);



//--------------------- Tasks containers management ---------------------

//Get the number of task that can be inserted in a particular sequence;
bool sequence_insertion_available(uint8_t sequence_id);

//Verify that tasks are available in the required sequences;
bool sequences_available(uint8_t type);

//Build and schedule a task;
bool sequences_add_task(uint8_t sequence_id, void (*func)(void *), void *args, void (*cleanup)()
        , uint16_t activity_time);


//--------------------- Tasks execution --------------------

//Remove the current task of the given sequence;
void sequences_remove_task(sequence_task_t *task);

//Get the first task of the given sequence;
stask_t *sequences_get_task(uint8_t sequence_id);


#endif //TRACER_TASKSEQUENCER_H