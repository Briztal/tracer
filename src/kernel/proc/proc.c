//
// Created by root on 9/26/18.
//

#include <kernel/async/interrupt.h>
#include <kernel/async/preempt.h>
#include <kernel/log.h>
#include "proc.h"


//--------------------------------------------- Context switching variables --------------------------------------------

//The current process in execution;
static struct prc *current_process = 0;

//The remaining number of threads that must stop before the next process gets executed; protected by the spinlock below;
static uint8_t nb_active_threads;



//------------------------------------------------- Proc requirements --------------------------------------------------

//The hardware library must provide a function to create the general stack context;
extern void proc_create_stack_context(struct proc_stack *stack, void (*function)(), void (*exit_loop)(), void *arg);

//The context switcher; Should be set as the preemption handler for a preemption to occur;
extern void proc_context_switcher();


/**
 * proc_enter_thread_mode : this function initialises threads in a safe state. It never returns.
 * 	The preemption environment must have been initialised before;
 *
 * 	- initialises all threads in their lowest privilege state;
 * 	- updates exception stacks;
 * 	- calls the provided function, that should trigger the preemption;
 * 	- make all threads run an infinite loop;
 * 	- execute the preemption call;
 * 	- enable interrupts;
 * 	- loop;
 *
 * @param exception_stacks : processor stacks that must be used in case of interrupt;
 */

extern void proc_enter_thread_mode(struct proc_stack **exception_stacks);

//------------------------------------------------- Variables --------------------------------------------------

//If required, a module can provide a function to create a stack header before the stack context. Used for ex for FPU;
static void (*stack_header_creator)(struct proc_stack *stack);



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


static struct prc **proc_create_init_stacks() {
	
	//TODO KERNEL COMPILATION FLAGS WITH NUMBER OF THREADS, MONOTASK FLAG, AND OTHER COMPILATION SETTINGS;
}


/**
 * kernel_enter_multitasked_mode : setups the kernel for multi-tasked mode.
 */

void proc_start_execution() {
	
	//Disable all interrupts;
	exceptions_disable();
	
	
	
	//Initialise the number of active threads;
	nb_active_threads = proc_nb_threads;
	
	//Initialise the preemption;
	preemption_init(&proc_context_switcher, KERNEL_PREMPTION_PRIORITY);
	
	//Log;
	kernel_log_("preemption initialised");
	
	
	/*
	 * Syscalls;
	 */
	
	/*
	//Set the syscall handler; Same prio as preemption;
	syscall_set_handler(kernel_syscall_handler);

	//Set the syscall exception priority to the lowest possible;
	syscall_set_priority(KERNEL_PREMPTION_PRIORITY);

	//Enable the syscall exception;
	syscall_enable();
	 */
	
	
	/*
	 * Start execution;
	 */
	
	//Log;
	kernel_log_("\nKernel initialisation sequence complete. Entering thread mode ...\n");
	
	
	//Enter thread mode and un-privilege, provide the kernel stack for interrupt handling;
	//Interrupts will be enabled at the end of the function;
	proc_enter_thread_mode(kernel_memory->stacks);
	
}


//-------------------------------------------------- Context Switching -------------------------------------------------

/**
 * kernel_get_new_stack : called by threads to get a new stack pointer, providing the current one, and the index of the
 * 	thread;
 * @param thread_id : the thread's index;
 * @param sp : the previous stack pointer;
 * @return the the new stack pointer;
 */

//TODO SYSCALL
extern bool prc_process_terminated;


void *proc_switch_context(const volatile uint8_t thread_id, void *volatile sp) {
	
	//In order to know when new stack pointers can be safely queried, a static variable will be used;
	static volatile bool process_updated;
	
	//As the process can be updated when all threads are stopped, the flag can be cleared when a thread enters;
	process_updated = false;
	
	//Cache the current process;
	struct prc *crp = current_process;
	
	//If the current process is not null :
	if (crp) {
		
		//Save the stack pointer;
		crp->prog_mem->stacks[thread_id]->sp = sp;
	}
	
	//Lock TODO MULTITHREAD PATCH;
	//spin_lock(sp);
	
	//Decrement the active threads counter; if it was null before :
	if (!(nb_active_threads--)) {
		
		//Error;
		kernel_panic("krnl.c : proc_switch_context : more entries than existing threads;");
		
	}
	
	//If no more threads have to stop :
	if (!nb_active_threads) {
		
		//TODO
		//TODO IN SYSCALL;
		
		if (prc_process_terminated) {
			
			//Terminate the current process;
			sched_terminate_prc();
			
			//Clear termination flag;
			prc_process_terminated = false;
			
		}
		
		//Commit changes to the scheduler;
		sched_commit();
		
		//Get the first process;
		current_process = sched_get();
		
		//Update the duration until next preemption;
		sysclock_set_process_duration(current_process->desc.activity_time);
		
		//Update the number of active threads;
		nb_active_threads = proc_nb_threads;
		
		//Update the flag;
		process_updated = true;
		
	}
	
	
	//Unlock TODO MULTITHREAD PATCH;
	//spin_unlock();
	
	
	/*
	 * The stop position. All threads except the last will enter and loop here, until the last updates the current
	 * process;
	 */
	
	while (!process_updated);
	
	//Return the appropriate stack pointer;
	return current_process->prog_mem->stacks[thread_id]->sp;
	
}


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

