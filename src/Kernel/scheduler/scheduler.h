//
// Created by root on 5/5/18.
//

#ifndef TRACER_SCHEDULER_H
#define TRACER_SCHEDULER_H

#include "process.h"

//Initialise the Scheduling environment;
void scheduler_initialise();

//Update a process;
void scheduler_update_process(process_t *);

//Select the thread to be executed;
process_t* scheduler_select_process();


#endif //TRACER_SCHEDULER_H
