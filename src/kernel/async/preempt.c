//
// Created by root on 10/10/18.
//

#include <kernel/panic.h>
#include "preempt.h"


//The kernel preemption manager;
static struct preemption_mgr *mgr = 0;

//---------------------------------------------------- Registration ----------------------------------------------------

//Register the preemption manager;
void preemption_register(struct preemption_mgr *manager) {
	
	//If the manager is already initialised :
	if (mgr) {
		
		//Panic. The preemption manager can only be registered once;
		kernel_panic("preemption_register : already registered;");
		
	}
	
	//Initialise the manager;
	mgr = manager;
	
}


//Is the preemption manager registered;
bool preemption_is_registered() {
	
	return (bool) mgr;
	
}


//---------------------------------------------------- Setup ----------------------------------------------------


//Initialise the preemption; Will update the handler, set the priority, and enable;
void preemption_init(void (*handler)(void), uint8_t priority) {
	
	//If the manager is not registered :
	if (!mgr) {
		
		//Panic. The preemption manager must be registered;
		kernel_panic("preemption_init : not registered;");
		
	}
	
	//Update the handler;
	(*mgr->set_handler)(handler);
	
	//Update the priority;
	(*mgr->set_priority)(priority);
	
	//Update the handler;
	(*mgr->enable)();
	
}


//Set the preemption exception pending;
void preemption_set_pending() {
	
	//Cache the manager;
	struct preemption_mgr *manager = mgr;
	
	//If the manager is not null :
	if (manager) {
		
		//Set the manager pending;
		(*manager->set_pending)();
		
	}
	
}


//Set the preemption exception not pending;
void preemption_clear_pending() {
	
	//Cache the manager;
	struct preemption_mgr *manager = mgr;
	
	//If the manager is not null :
	if (manager) {
		
		//Clear the manager pending;
		(*manager->clear_pending)();
		
	}
	
}

