//
// Created by root on 10/3/18.
//

#ifndef TRACER_FAULT_H
#define TRACER_FAULT_H

#include <stdbool.h>

#include <stdint.h>

/*
 * The kernel can handle faults. A fault will place the kernel in a safe state, where it can analyse the fault,
 * 	and decide what to do.
 * 	If the fault is minor, the kernel will attempt to recover, depending on where the fault has
 * 	happened. If the fault is major, the kernel will stop and panic;
 *
 * 	The fault analysis depends on the platform, and so, must be implemented and registered by a module;
 * 	If no analyser is present, the kernel will panic.
 *
 * 	The fault analyser is in charge of finding the fault, transmitting all information required to correct it, and
 * 	return true or false, depending on if the fault is minor or major;
 */

//------------------------------------------------ Fault types -----------------------------------------------

enum fault_type {
	
	NO_FAULT = 2,
	
	RECOVERABLE_FAULT = 1,
	
	NON_RECOVERABLE_FAULT = 0,
	
};

#define FAULT_ESCALATE(var ,f) {if ((var) < (f)) (var) = (f);}


//------------------------------------------------ Fault handling config -----------------------------------------------

//Update the fault analyser; Takes the fault code in parameter (indicative data),
// and returns true if the fault is minor, or false if the fault is major;
void kernel_init_fault_analyser(enum fault_type (*analyser)(uint32_t type));


//--------------------------------------------------- Fault handling ---------------------------------------------------

//Place the kernel in a safe state, call the fault analyser, and recover, if possible. If not, panic;
void kernel_handle_fault(uint32_t type);


#endif //TRACER_FAULT_H
