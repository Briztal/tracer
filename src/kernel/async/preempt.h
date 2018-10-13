//
// Created by root on 10/10/18.
//

#ifndef TRACER_PREEMPTION_H
#define TRACER_PREEMPTION_H

#include <stdint.h>

#include <stdbool.h>

#include <kernel/hard.h>


//---------------------------------------------------- Setup ----------------------------------------------------

//Initialise the preemption; Will update the handler, set the priority, and enable;
void preemption_init(void (*handler)(void), uint8_t priority);


//---------------------------------------------------- Operatuions ----------------------------------------------------

//Set the preemption exception pending;
#define preemption_set_pending() __preemption_set_pending();

//Set the preemption exception not pending;
#define preemption_clear_pending()__preemption_clear_pending();


#endif //TRACER_PREEMPTION_H
