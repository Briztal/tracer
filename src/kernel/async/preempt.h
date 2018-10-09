//
// Created by root on 10/10/18.
//

#ifndef TRACER_PREEMPTION_H
#define TRACER_PREEMPTION_H

#include <stdint.h>

#include <stdbool.h>


//------------------------------------------------------ Manager -------------------------------------------------------

/**
 * A preemption manager is a set of function pointers that setup the preemption;
 */

struct preemption_mgr {
	
	//Update the preemption handler;
	void (*set_handler)(void (*)(void));
	
	//Update the preemption priority;
	void (*set_priority)(uint8_t);
	
	//Enable the preemption exception;
	void (*enable)();
	
	//Enable the preemption exception;
	void (*disable)();
	
	//Set the exception pending;
	void (*set_pending)();
	
	//Clear the exception pending;
	void (*clear_pending)();
	
};


//---------------------------------------------------- Registration ----------------------------------------------------

//Register the preemption manager;
void preemption_register(struct preemption_mgr *);

//Is the preemption manager registered;
bool preemption_is_registered();


//---------------------------------------------------- Setup ----------------------------------------------------

//Initialise the preemption; Will update the handler, set the priority, and enable;
void preemption_init(void (*handler)(void), uint8_t priority);

//Set the preemption exception pending;
void preemption_set_pending();

//Set the preemption exception not pending;
void preemption_clear_pending();


#endif //TRACER_PREEMPTION_H
