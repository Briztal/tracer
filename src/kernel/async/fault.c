//
// Created by root on 10/3/18.
//

#include <panic.h>
#include <debug/printk.h>
#include "fault.h"
#include "except.h"


//--------------------------------------------------- Fault analyser ---------------------------------------------------

//The fault analyser. Static, and null at init;

/**
 * fault_analyser : pointer to the fault analyser; Null at init;
 *
 * @param type : the probable type of the fault. Indicative;
 * @return true if the fault is minor, false if no recover can be done;
 */

static enum fault_type (*fault_analyser)(uint32_t type) = 0;


//------------------------------------------------ Fault handling config -----------------------------------------------

/**
 * fault_init_analyser : initialises the fault analyser. If already initialised, panic;
 *
 * @param analyser : the new fault analysis function;
 */

void kernel_init_fault_analyser(enum fault_type (*analyser)(uint32_t type)) {
	
	//If the analyser is already initialised :
	if (fault_analyser) {
		
		//Panic. The fault analyser is unique;
		kernel_panic("fault_init_analyser : analyser already initialised;");
		
	}
	
	//If not, initialise the analyser;
	fault_analyser = analyser;
	
}


//--------------------------------------------------- Fault handling ---------------------------------------------------

//Place the kernel in a safe state, call the fault analyser, and recover, if possible. If not, panic;
void __krnl_handle_fault(uint32_t type) {
	
	//Enter a critical section;
	critical_section_enter();
	
	//Log;
	printkf("Fault of type %d detected !", type);
	
	//Cache the fault analyser;
	enum fault_type (*analyser)(uint32_t) = fault_analyser;
	
	//If the analyser is null :
	if (!analyser) {
		
		printk("No analyser.");
		
		return;
		
	}
	
	//The analyser is initialised;
	
	//Log;
	printk("Executing the fault analyser ...");
	
	//Execute the fault analyser;
	bool minor_fault = (*analyser)(type);
	
	//If the fault is minor :
	if (minor_fault) {
		
		//Log;
		printk("Minor fault detected. Attempting to recover ...");
		printk("No recovery procedure implemented. Goodbye.");
		
	} else {
		
		//If the fault is major :
		printk("Major fault detected. No possible recovery. Goodbye.");
		
	}
	
	while(1);
	
}
