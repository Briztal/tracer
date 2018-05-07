//
// Created by root on 3/23/18.
//

#include <Kernel/Interaction/Interaction.h>
#include <Kernel/Kernel.h>
#include <DataStructures/Containers/CircularBuffer.h>
#include <Kernel/Scheduler/tasks.h>
#include <DataStructures/Containers/circular_buffer.h>
#include <DataStructures/Containers/container.h>
#include "sequences.h"


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

    //The sequence's state;
    sequence_state_t state;

    //The tasks buffer;
    cbuffer_t tasks;

} sequence_t;


//Task sequences;
container_t sequences = EMPTY_CONTAINER(sequence_t *);


/*
 * sequences_add_sequence : adds a sequence to the container;
 */

void sequences_add_sequence(container_index_t sequence_size) {

    //First, we must create a sequence in the heap;

    //Allocate data;
    void *ptr = malloc(sizeof(sequence_t));

    //If the allocation failed :
    if (!ptr) {

        //fail; TODO ERROR;
        return;

    }

    //If the allocation succeeded, let's cache a casted copy for readability. Compiler optimised;
    sequence_t *sequence = (sequence_t *) ptr;

    //Initialise the sequence;
    *sequence = {
            .tasks = EMPTY_CBUFFER(task_t *),
            .state = UNLOCKED,
    };

    //Resize the task buffer;
    cbuffer_resize(&sequence->tasks, sequence_size);

    //Add the sequence to the sequences container;
    container_append_element(&sequences, &sequence);

}



//------------------------------------------------- Sequence query --------------------------------------------------

sequence_t *sequences_query_sequence(uint8_t sequence_id) {

    //If the index is invalid, the container library will generate an error;
    return *(sequence_t **) container_get_element(&sequences, sequence_id);

}


//-------------------------------------------------Type Verification ---------------------------------------------------


/*
 * TODO
 */

bool sequences_add_task(uint8_t sequence_id, task_t *task) {

    //Query the appropriate sequence;
    sequence_t *sequence = sequences_query_sequence(sequence_id);

    //If the sequence is full, fail;
    if (!sequence->tasks.nb_spaces)
        return false;//TODO KERNEL PANIC. SHOULD HAVE VERIFIED, THE TASK WILL BE LOST;

    //Copy the pointer in the cbuffer and validate it;
    cbuffer_insert(&sequence->tasks, &task);

    //Succeed;
    return true;


    //TODO KERNEL PANIC. INVALID SEQUENCE ID, THE TASK WILL BE LOST;
    //Fail;
    return false;

}


/*
 * nb_spaces : this function returns the number of spaces available in the required sequence;
 */

container_index_t sequences_available_spaces(uint8_t sequence_id) {

    //If the index is invalid, return zero for safety;
    if (sequence_id >= sequences.nb_elements)
        return 0;//TODO ERROR ?

    //Cache the required task sequence;
    sequence_t *sequence = sequences_query_sequence(sequence_id);

    //If the sequence is full, fail;
    return (sequence->tasks.nb_spaces);

}


/*
 * sequenceProcessable : returns true if tasks are available in the required sequence (255 for task pool);
 */

bool sequences_available_task(uint8_t sequence_id) {

    //If the type is not allocated, return zero;
    if (sequence_id >= sequences.nb_elements)
        return 0;//TODO ERROR ?

    //Cache the required task sequence;
    sequence_t *sequence = *(sequence_t **) container_get_element(&sequences, sequence_id);

    //A task is available if the sequence is unlocked and not empty;
    return (sequence->state == UNLOCKED) && (sequence->tasks.container.nb_elements - sequence->tasks.nb_spaces);

}


/*
 * getPoolTask : returns a task identifier from the task pool;
 */

task_t *sequences_get_task(uint8_t sequence_id) {

    //Get the required sequence. An invalid index will generate an error;
    sequence_t *sequence = sequences_query_sequence(sequence_id);

    //If the sequence is locked :
    if (!sequence->state == LOCKED)
        return 0;//TODO USAGE FAULT;

    //If the sequence exists and is unlocked, cache the first task of the buffer;
    task_t *task = cbuffer_read_output(&sequence->tasks, 0);

    //Discard the cached task;
    cbuffer_discard_output(&sequence->tasks);

    //If the sequence must be locked (index not zero):
    if (sequence_id) {

        //Mark the sequence locked;
        sequence->state = LOCKED;

    }

    //Return the cached task's pointer;
    return task;

}


/*
 * sequence_unlock : this function unlocks the given sequence;
 */
void sequence_unlock(uint8_t sequence_id) {

    //If the type is not allocated, return zero;
    if (sequence_id >= sequences.nb_elements)
        return ;//TODO ERROR ?

    //Cache the required task sequence;
    sequence_t *sequence = *(sequence_t **) container_get_element(&sequences, sequence_id);

    //Unlock the sequence;
    sequence->state = UNLOCKED;

}


