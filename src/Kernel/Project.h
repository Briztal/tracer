//
// Created by root on 5/2/18.
//

#ifndef TRACER_PROJECT_H
#define TRACER_PROJECT_H


/*
 * Here are defined all mandatory function to implement to make the project work;
 */

#include <Kernel/Scheduler/Schedulers/Scheduler.h>

namespace Project {

    /*
     * Creates and returns an instance of the adequate Scheduler class;
     */

    Scheduler *instantiate_scheduler();

};


#endif //TRACER_PROJECT_H
