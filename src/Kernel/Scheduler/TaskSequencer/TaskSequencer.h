//
// Created by root on 3/23/18.
//

#ifndef TRACER_TASKSEQUENCER_H
#define TRACER_TASKSEQUENCER_H


#include <DataStructures/Containers/Queue.h>
#include <Kernel/Scheduler/TaskData.h>


namespace TaskSequencer {


    //--------------------- Initialisation ---------------------

    //Reset the sequencer;
    void reset();


    //--------------------- Tasks add ---------------------

    //Get the number of task that can be inserted in a particular sequence;
    uint8_t availableSpaces(uint8_t type);

    //Build and schedule a task;
    bool schedule_task(uint8_t type, TaskData &task);


    //--------------------- Tasks execution ---------------------

    //Verify that tasks are available in the required sequences;
    bool availableTasks(uint8_t type);

    //Remove the current task of the given sequence;
    void unlockSequence(uint8_t sequence_id);


    //Determine the number of tasks in the task pool;
    uint8_t poolTasksNb();

    //Get a task from the task pool;
    uint8_t getPoolTask();


    //Determine if the required sequence can be processed;
    bool poolProcessable(uint8_t type);

    //Get the first task of the given sequence;
    uint8_t getSequencerTask(uint8_t sequence_id);


};


#endif //TRACER_TASKSEQUENCER_H
