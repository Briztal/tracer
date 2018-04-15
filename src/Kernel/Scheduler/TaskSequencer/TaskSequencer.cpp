//
// Created by root on 3/23/18.
//

#include <Kernel/Interaction/Interaction.h>
#include <Kernel/Kernel.h>
#include <DataStructures/Containers/CircularBuffer.h>
#include <Kernel/Scheduler/TaskStorage.h>
#include "TaskSequencer.h"


//-------------------------------------------- Const variable init --------------------------------------------

/*
 * The number of task sequences is a constant, directly determinable by counting the number of times TASK_SEQUENCES
 *  is written in the configuration file.
 *
 *  Instead of asking the user to provide it, what could lead to errors, we will use a compiler constant
 *      determined directly using macro;
 *
 */

#define TASK_SEQUENCE(...) 1 +

const uint8_t NB_TASK_SEQUENCES =

#include "Config/kernel_config.h"

        0;

#undef TASK_SEQUENCE


namespace TaskSequencer {

    //Task Pool;
    CircularBuffer<uint8_t> taskPool(TASK_POOL_SIZE);

    //Task sequences;
    CircularBuffer<uint8_t> *sequences[NB_TASK_SEQUENCES]{nullptr};

    //Task Sequences lock;
    bool sequencesUnlocked[NB_TASK_SEQUENCES]{true};
}


/*
 * reset : initialises the sequencer in a safe state;
 */

void TaskSequencer::reset() {

    //First, delete all sequences;
    for (uint8_t sequences_id = NB_TASK_SEQUENCES; sequences_id--;) {

        //Delete the previous sequence;
        delete sequences[sequences_id];

    }

    //Then, recreate them;
    uint8_t sequence_id = 0;

#define TASK_SEQUENCE(size) \
        sequences[sequence_id] = new CircularBuffer<uint8_t>(size);\
        sequencesUnlocked[sequence_id++] = true;

#include "Config/kernel_config.h"

#undef TASK_SEQUENCE

}


//--------------------------------------------------Type Verification---------------------------------------------------



/*
 * TODO
 */

bool TaskSequencer::schedule_task(uint8_t sequence_id, TaskData &task) {

    //If no space is available, nothing to do;
    if (!TaskStorage::availableSpaces()){
        return false;//TODO KERNEL PANIC, SHOULD HAVE VERIFIED, THE TASK WILL BE LOST;
    }

    //If the task is to execute asap, and the task pool can contain tasks :
    if (sequence_id == 255) {

        //If the task pool doesn't have any space left, fail;//TODO KERNEL PANIC. SHOULD HAVE VERIFIED, THE TASK WILL BE LOST;
        if (!taskPool.available_spaces())
            return false;

        //Save the task in the TaskStorage and cache its index;
        uint8_t task_index = TaskStorage::addTask(task);

        //Copy the task;
        taskPool.insert_object(task_index);

        //Succeed;
        return true;

    }

    //If the task's type corresponds to an existing sequence:
    if (sequence_id < NB_TASK_SEQUENCES) {

        //First, cache the required task sequence;
        CircularBuffer<uint8_t> *sequence = sequences[sequence_id];

        //If the sequence is full, fail;
        if (!sequence->available_spaces())
            return false;//TODO KERNEL PANIC. SHOULD HAVE VERIFIED, THE TASK WILL BE LOST;

        //Save the task in the TaskStorage and cache its index;
        uint8_t task_index = TaskStorage::addTask(task);

        //Copy the task in the sequence;
        sequence->insert_object(task_index);

        //Succeed;
        return true;

    }

    //TODO KERNEL PANIC. INVALID SEQUENCE ID, THE TASK WILL BE LOST;
    //Fail;
    return false;

}


/*
 * nb_spaces : this function returns the number of spaces available in the task pool.
 */

uint8_t TaskSequencer::availableSpaces(uint8_t type) {

    //If no space is available, nothing to do;
    if (!TaskStorage::availableSpaces()){
        return 0;
    }


    if (type == 255) {

        //If the type corresponds to the task pool : return the number of nb_spaces of the pool;
        return taskPool.available_spaces();

    } else {
        //If the type corresponds to a sequence :

        //If the type is not allocated, return zero;
        if (type >= NB_TASK_SEQUENCES)
            return 0;

        //return the number of nb_spaces in the concerned sequence;
        return sequences[type]->available_spaces();

    }

}


/*
 * availableTasks : returns true if tasks are available in the required sequence (255 for task pool);
 */

uint8_t TaskSequencer::poolTasksNb() {

    return taskPool.available_elements();
}


/*
 * sequenceProcessable : returns true if tasks are available in the required sequence (255 for task pool);
 */

bool TaskSequencer::poolProcessable(uint8_t type) {

    //If the type is valid :
    if (type < NB_TASK_SEQUENCES) {

        //Return true if the sequence is unlocked and contains tasks;
        return sequencesUnlocked[type] && sequences[type]->available_elements();
    }

    //if not, return false for safety;
    return false;
}


/*
 * getPoolTask : returns a task identifier from the task pool;
 */

uint8_t TaskSequencer::getPoolTask() {
    return taskPool.get_and_discard_output();
}


/*
 * getPoolTask : returns a task identifier from the task pool;
 */

uint8_t TaskSequencer::getSequencerTask(uint8_t sequence_id) {

    //If the sequence_id is incorrect :
    if (sequence_id >= NB_TASK_SEQUENCES)
        return 0;//TODO USAGE FAULT;

    //If the sequence is locked :
    if (!sequencesUnlocked[sequence_id])
        return 0;//TODO USAGE FAULT;

    //If the sequence_id is correct, return the first task of the buffer;
    uint8_t id = sequences[sequence_id]->get_and_discard_output();

    //Mark the sequence locked;
    sequencesUnlocked[sequence_id] = false;

    //Return the task's identifier;
    return id;

}


/*
 * enableSequence : this function unlocks the given sequence;
 */
void TaskSequencer::unlockSequence(uint8_t sequence_id) {

    //If the sequence id is invalid, do nothing;
    if (sequence_id >= NB_TASK_SEQUENCES)
        return;

    //Unlock the sequence;
    sequencesUnlocked[sequence_id] = true;
}


