/*
  sequences.c Part of TRACER

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

#include "sequences.h"

#include <string.h>

#include <kernel/kernel.h>


//-------------------------------------------- Const variable init --------------------------------------------

/*
 * The sequence type;
 */

typedef enum {
    LOCKED,

    UNLOCKED
} sequence_state_t;


/*
 * The sequence struct;
 */

typedef struct {

    //The tasks buffer;
    linked_list_t tasks;

    //The sequence's sequences_initialised;
    sequence_state_t state;

} sequence_t;



/*
 * The sequence manager starts in the uninitialised sequences_initialised, then becomes scheduler_initialised, and is finally started;
 */

bool sequences_initialised = false;

//Task sequences;
sequence_t *sequences = 0;

//Number of task sequences;
size_t nb_sequences = 0;


//TODO MAKE A CHECK INDEX FUNCTION

/*
 * sequences_initialise : initialises the sequence
 */

void sequences_initialise(const size_t unordered_sequence_size, const size_t nb_ordered_sequences,
                          const size_t *ordered_sizes) {

    //If sequences are already scheduler_initialised, error;
    if (sequences_initialised) {
        return;//TODO ERROR;
    }

    //Update the number of sequences;
    nb_sequences = nb_ordered_sequences + 1;

    //Create an array in the heap to contain all sequences;
    sequences = kernel_malloc(nb_sequences * sizeof(sequence_t));

    //Initializer for the first sequence (unordered);
    sequence_t init = {
            .tasks = EMPTY_LINKED_LIST(unordered_sequence_size),
            .state = UNLOCKED,
    };

    //Initialize the first sequence (unordered);
    memcpy(sequences, &init, sizeof(sequence_t));

    //Cache the pointer to the current sequence to initialise;
    sequence_t *ptr = sequences + 1;

    //For each ordered sequence;
    for (size_t sequence_index = 0; sequence_index < nb_ordered_sequences; sequence_index++) {

        //Cache the current sequence's size;
        size_t size = *(ordered_sizes++);

        //Update the initializer;
        init.tasks = EMPTY_LINKED_LIST(size);

        //Initialize the task;
        memcpy(ptr++, &init, sizeof(sequence_t));

    }

    //Update the sequences_initialised to scheduler_initialised;
    sequences_initialised = true;

}


//-------------------------------------------------Type Verification ---------------------------------------------------

/*
 * sequences_add_task : determines the required sequence, and appends the task to it, if there is one space.
 */

bool sequences_add_task(uint8_t sequence_id, void (*func)(void *), void *args, void (*cleanup)()) {

    //If sequences are not scheduler_initialised :
    if (!sequences_initialised) {

        //Error;
        return 0;//TODO ERROR

    }

    //If the sequence identifier is invalid :
    if (sequence_id >= nb_sequences) {

        return false;//TODO ERROR;

    }

    //Query the appropriate sequence;
    sequence_t *const sequence = sequences + sequence_id;

    //Access to sequences is critical;
    kernel_enter_critical_section();

    //A task is available if the sequence is unlocked and not empty;
    bool available = llist_insertion_available((const linked_list_t *const) sequence);

    //Leave the critical section;
    kernel_leave_critical_section();

    //If the sequence is full, fail;
    if (available) {
        return false;//TODO kernel PANIC. SHOULD HAVE VERIFIED, THE TASK WILL BE LOST;
    }

    //Create the initializer for the new task;
    const sequence_task_t init = {

            .task = {
                    .linked_element = EMPTY_LINKED_ELEMENT(),
                    .function = (func),
                    .args = (args),
                    .cleanup = (cleanup),
                    .task_type = SEQUENCE_TASK,
            },

            .sequence_id = sequence_id,

    };

    //Create a sequence task in the heap and initialize it;
    sequence_task_t *const task = (sequence_task_t *) kernel_malloc_copy(sizeof(sequence_task_t), &init);

    //Access to sequences is critical;
    kernel_enter_critical_section();

    //Add the task to the list;
    llist_insert_end((linked_list_t *) sequence, (linked_element_t *) task);

    //Leave the critical section;
    kernel_leave_critical_section();

    //Succeed;
    return true;

}


/*
 * sequence_insertion_available : this function returns the number of spaces available in the required sequence;
 */

bool sequence_insertion_available(uint8_t sequence_id) {

    //If sequences are not scheduler_initialised :
    if (!sequences_initialised) {

        //Error;
        return 0;//TODO ERROR

    }

    //If the sequence identifier is invalid :
    if (sequence_id >= nb_sequences) {
        return false;//TODO ERROR;
    }

    //Cache the required task sequence;
    sequence_t *sequence = sequences + sequence_id;

    //Access to sequences is critical;
    kernel_enter_critical_section();

    //A task is available if the sequence is unlocked and not empty;
    bool available = llist_insertion_available((const linked_list_t *const) sequence);

    //Leave the critical section;
    kernel_leave_critical_section();

    //Return the availability;
    return available;

}


/*
 * sequences_available : returns true if tasks are available in the required sequence (255 for task pool);
 */

bool sequences_available(uint8_t sequence_id) {

    //If sequences are not scheduler_initialised :
    if (!sequences_initialised) {

        //Error;
        return 0;//TODO ERROR

    }

    //If the sequence identifier is invalid :
    if (sequence_id >= nb_sequences) {
        return false;//TODO ERROR;
    }

    //Cache the required task sequence;
    sequence_t *sequence = sequences + sequence_id;

    //Access to sequences is critical;
    kernel_enter_critical_section();

    //A task is available if the sequence is unlocked and not empty;
    bool available = (sequence->state == UNLOCKED) && (sequence->tasks.nb_elements);

    //Leave the critical section;
    kernel_leave_critical_section();

    //Return the availability;
    return available;

}


/*
 * sequences_get_task : returns a task identifier from the task pool;
 */

task_t *sequences_get_task(uint8_t sequence_id) {

    //If sequences are not scheduler_initialised :
    if (!sequences_initialised) {

        //Error;
        return 0;//TODO ERROR

    }

    //If the sequence identifier is invalid :
    if (sequence_id >= nb_sequences) {
        return 0;//TODO ERROR;
    }

    //Cache the required sequence.;
    sequence_t *const sequence = sequences + sequence_id;


    //Access to sequences is critical;
    kernel_enter_critical_section();

    //Cache the sequences_initialised;
    sequence_state_t state = sequence->state;

    //Leave the critical section;
    kernel_leave_critical_section();

    //If the sequence is locked :
    if (state == LOCKED)
        return 0;//TODO USAGE FAULT;


    //Access to sequences is critical;
    kernel_enter_critical_section();

    //If the sequence exists and is unlocked, cache the first task of the buffer;
    task_t *task = (task_t *) llist_remove_begin((linked_list_t *) sequence);

    //If the sequence must be locked (index not zero):
    if (sequence_id) {

        //Mark the sequence locked;
        sequence->state = LOCKED;

    }

    //Leave the critical section;
    kernel_leave_critical_section();

    //Return the cached task's pointer;
    return task;

}


/*
 * sequences_remove_task : this function receives an executed task, unlocks its sequence, and destroys it;
 */

void sequences_remove_task(sequence_task_t *task) {

    //If sequences are not scheduler_initialised :
    if (!sequences_initialised) {

        //Error;
        return;//TODO ERROR

    }

    //Cache the sequence id;
    uint8_t sequence_id = task->sequence_id;

    //If the sequence identifier is invalid :
    if (sequence_id >= nb_sequences) {

        return;//TODO ERROR;

    }

    //Destroy the task;
    task_delete((task_t *) task);

    //Cache the required task sequence;
    sequence_t *sequence = sequences + sequence_id;

    //Access to sequences is critical;
    kernel_enter_critical_section();

    //Unlock the sequence;
    sequence->state = UNLOCKED;

    //Leave the critical section;
    kernel_leave_critical_section();

}


