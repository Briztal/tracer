//
// Created by root on 9/26/18.
//

#include <kernel/log.h>
#include "proc.h"


//------------------------------------------------- Proc requirements --------------------------------------------------

//The hardware library must provide a function to create the general stack context;
extern void proc_create_stack_context(struct proc_stack *stack, void (*function)(), void (*exit_loop)(), void *arg);


//------------------------------------------------- Variables --------------------------------------------------

//If required, a module can provide a function to create a stack header before the stack context. Used for ex for FPU;
static void (*stack_header_creator)(struct proc_stack *stack);


//------------------------------------------------- Stack header --------------------------------------------------

/**
 * register_stack_header_creator : updates the stack header creator function pointer, if it is null;
 *
 * 	If it is not null, the function fails;
 *
 * @param new_creator : the new stack header creation function;
 * @return true if registration completed;
 */

bool register_stack_header_creator(void (*new_creator)(struct proc_stack *)) {
	
	//If the creator is already registered :
	if (stack_header_creator) {
		
		//Log;
		kernel_log_("register_stack_header_creator : already registered.");
		
		//Fail;
		return false;
		
	}
	
	//Register the stack creator;
	stack_header_creator = new_creator;
	
	//Complete;
	return true;
	
}


/**
 * reset_stack_header_creator : resets the function pointer;
 */

void reset_stack_header_creator() {
	
	//Reset the function pointer;
	stack_header_creator = 0;
	
}


/**
 * create_stack_header : if non-null, calls the stack header creator;
 */

static void create_stack_header(struct proc_stack *stack) {
	
	//Cache the function;
	void (*creator)(struct proc_stack *) = stack_header_creator;
	
	//If it is not null :
	if (creator) {
		
		//Call it;
		(*creator)(stack);
		
	}
	
}


//------------------------------------------------- Proc requirements --------------------------------------------------

//Initialise the stack context for future execution;
void proc_init_stack(struct proc_stack *stack, void (*function)(), void (*end_loop)(), void *init_arg) {
	
	//Reset the stack pointer;
	stack->sp = stack->sp_reset;
	
	//Create the stack header;
	create_stack_header(stack);
	
	//Create the stack context;
	proc_create_stack_context(stack, function, end_loop, init_arg);

}


//TODO COPROCESSOR CONTEXT SWITCHER
//TODO COPROCESSOR CONTEXT SWITCHER
//TODO COPROCESSOR CONTEXT SWITCHER
//TODO COPROCESSOR CONTEXT SWITCHER
//TODO COPROCESSOR CONTEXT SWITCHER
//TODO COPROCESSOR CONTEXT SWITCHER
//TODO COPROCESSOR CONTEXT SWITCHER
//TODO COPROCESSOR CONTEXT SWITCHER
//TODO COPROCESSOR CONTEXT SWITCHER
//TODO COPROCESSOR CONTEXT SWITCHER
//TODO COPROCESSOR CONTEXT SWITCHER
//TODO COPROCESSOR CONTEXT SWITCHER
//TODO COPROCESSOR CONTEXT SWITCHER
//TODO COPROCESSOR CONTEXT SWITCHER
//TODO COPROCESSOR CONTEXT SWITCHER
//TODO COPROCESSOR CONTEXT SWITCHER
//TODO COPROCESSOR CONTEXT SWITCHER
//TODO COPROCESSOR CONTEXT SWITCHER
//TODO COPROCESSOR CONTEXT SWITCHER
//TODO COPROCESSOR CONTEXT SWITCHER
//TODO COPROCESSOR CONTEXT SWITCHER
//TODO COPROCESSOR CONTEXT SWITCHER

