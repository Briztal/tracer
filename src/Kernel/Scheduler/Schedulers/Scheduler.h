//
// Created by root on 4/13/18.
//

#ifndef TRACER_SCHEDULER_H
#define TRACER_SCHEDULER_H


class Scheduler {

public:

    //Initialise the Scheduling environment;
    virtual void initialise() = 0;

    //Add a task in the scheduler;
    virtual void addTask() = 0;

    //Select the thread to be executed;
    virtual void selectThread() = 0;

};


#endif //TRACER_SCHEDULER_H
