//
// Created by root on 7/3/18.
//

#ifndef TRACER_STEP_EXECUTION_H
#define TRACER_STEP_EXECUTION_H

#include <stdint.h>
#include <data_structures/containers/llist.h>

/*
 * This library defines the staged computation structure;
 *
 * 	This structure eases things when you need to execute a process that comprises several stages, and
 * 	where each stage might need to be called a certain number of times;
 *
 */

/*
 * A stage function can be executed several times until it completes. Its return state will inform the stage
 * 	controller on its status.
 */

typedef enum {

	//The stage is complete, the next one can be executed;
	STAGE_COMPLETE,

	//The stage is aborted, the computation must restart from the beginning;
	STAGE_ABORT,

	//The stage's computation is not complete. Another computation is required;
	STAGE_RE_EXECUTE

} cnode_state_t;


//A stage function takes an instance, and returns a stage stage;
typedef cnode_state_t (*stage_function_t )(void *instance);


/*
 * A stage is composed of a function to call, and of an instance to pass.
 *
 * 	It has a maximal number of consecutive executions;
 */

typedef struct {

	//The eventual instance to pass to the function;
	void *instance;

	//The stage function, to call until completion or abortion;
	stage_function_t stage_function;

	//The stage's maximal number of consecutive executions;
	const uint8_t max_executions;

	//The stage's remaining number of executions;
	uint8_t remaining_executions;

	//Is the stage initialised ?
	volatile bool initialised;

} stage_t;


/*
 * The staged computation structure;
 *
 * 	A staged computation is composed of an array of stages, and of a current index;
 */

typedef struct {

	//The computation lock, to prevent multiple executions;
	volatile bool lock;

	//The number of stages of the computation;
	const size_t nb_stages;

	//The number of steps of the computation;
	size_t nb_steps;

	//The current stage index;
	size_t current_stage;

	//The stages array;
	stage_t *const stages;

} staged_execution_t;


//----------------------------------------- Staged executions public functions -----------------------------------------

//Initialise a previously allocated staged_execution;
void staged_execution_initialise(staged_execution_t * execution, size_t nb_stages);

//Delete a previously allocated staged_execution;
void staged_execution_delete(staged_execution_t * execution);



//Set a particular stage;
void staged_execution_set_stage(staged_execution_t *execution, size_t stage_index, void *instance,
								stage_function_t function, uint8_t max_executions);


//Execute a stage;
void staged_executions_execute(staged_execution_t *execution);


//Is the staged computation reset ?
bool staged_execution_is_reset(staged_execution_t *execution);


#endif //TRACER_STEP_EXECUTION_H
