//
// Created by root on 3/23/18.
//

#ifndef TRACER_TASKSEQUENCER_H
#define TRACER_TASKSEQUENCER_H


#include <Kernel/Scheduler/TaskScheduler/task_function_t.h>
#include <DataStructures/Containers/Queue.h>

namespace TaskSequencer {


    //Initialise the scheduler in a safe state;
    void initialise_data();

    //Verify that a type exists
    bool check_sequence_type(uint8_t type);

    //Execute every possible task on tasks sequences;
    void process();

    //Build and schedule a task;
    bool schedule_task(uint8_t type, task_state_t (*f)(void *), void *args);

};



/*
 * LOCK_SEQUENCE : This macro shall be used in your tasks to lock a given task_sequence.
 */

#define LOCK_SEQUENCE(type) {TaskScheduler::lock_sequence(type);}


#endif //TRACER_TASKSEQUENCER_H
