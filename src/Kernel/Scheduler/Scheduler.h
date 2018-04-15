//
// Created by root on 4/13/18.
//

#ifndef TRACER_SCHEDULER_H
#define TRACER_SCHEDULER_H


namespace Scheduler {

    //Initialise the Scheduling environment;
    void initialise();

    //Add a task in the scheduler;
    void addTask();

    //Select the thread to be executed;
    void selectThread();

};


#endif //TRACER_SCHEDULER_H
