//
// Created by root on 5/5/18.
//

#ifndef TRACER_SCHEDULER_H
#define TRACER_SCHEDULER_H

#include "process.h"

//Initialise the Scheduling environment;
void scheduler_initialise();


//Select the process to be executed;
process_t* scheduler_select_process();

//Update a process after its preemption;
void scheduler_clean_process(process_t *);


#endif //TRACER_SCHEDULER_H
