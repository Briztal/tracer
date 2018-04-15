//
// Created by root on 4/11/18.
//

#include <DataStructures/Containers/UnorderedArray.h>

#include "TaskStorage.h"

#include "TaskSequencer/TaskSequencer.h"


#include "stdint.h"

namespace TaskStorage {


    //---------------------- Fields ----------------------

    //The task container;
    UnorderedArray<TaskData> taskContainer(255);//TODO CONFIGURABLE

};


//---------------------- Public Functions ----------------------


/*
 * availableSpaces : returns the number of spaces in the task array;
 */

uint8_t TaskStorage::availableSpaces() {

    //Return the number of unallocated elements of the task array;
    return taskContainer.getNbSpaces();

}


/*
 * addTask : adds the given task to the array;
 */

uint8_t TaskStorage::addTask(TaskData &task) {

    //Add the task to the container;
    return taskContainer.add(task);

}


/*
 * removeTask : deletes the given task of the array;
 */

void TaskStorage::removeTask(uint8_t task_id) {

    //Cache the task's pointer;
    TaskData *task = taskContainer.get(task_id);

    //Delete the task's content;
    free(task->args);

    //If the task comes from a sequence :
    if (task->task_origin == SEQUENCE_TASK) {

        //Notify the sequencer that one of its tasks is finished;
        TaskSequencer::unlockSequence(task->task_sub_type);

    }

    //Remove the task of the container;
    taskContainer.remove(task_id);

}


/*
 * getTask : returns a pointer to the required task;
 */

TaskData *TaskStorage::getTask(uint8_t task_id) {

    return taskContainer.get(task_id);

}


