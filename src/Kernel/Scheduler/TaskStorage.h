//
// Created by root on 4/11/18.
//

#ifndef TRACER_TASKSTORAGE_H
#define TRACER_TASKSTORAGE_H


#include "TaskData.h"

#include "stdint.h"

namespace TaskStorage {

    //---------------------- Public Functions ----------------------

    //Add a task;
    uint8_t addTask(TaskData &task);

    //delete a task;
    void removeTask(uint8_t task_id);

    //Get a particular task;
    TaskData *getTask(uint8_t task_id);

    //Get the number of new tasks that can be stored;
    uint8_t availableSpaces();


};


#endif //TRACER_TASKSTORAGE_H
