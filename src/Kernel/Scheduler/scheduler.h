//
// Created by root on 5/5/18.
//

#ifndef TRACER_SCHEDULER_H
#define TRACER_SCHEDULER_H

#include "process.h"

//Initialise the Scheduling environment;
scheduler_initialise();

//Select the thread to be executed;
process_t* scheduler_select_process();


#endif //TRACER_SCHEDULER_H
