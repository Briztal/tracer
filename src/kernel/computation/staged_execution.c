//
// Created by root on 7/3/18.
//

#include <kernel/kernel.h>
#include <string.h>
#include "staged_execution.h"

//--------------------------------------------------- Stages headers  --------------------------------------------------

//Reset a stage;
void stage_reset(stage_t *stage);


//------------------------------------------ Staged executions private headers -----------------------------------------

//Increment the current stage index;
void staged_execution_update_stage(staged_execution_t *execution);



//----------------------------------------------------- Stage reset ----------------------------------------------------

//Reset a stage;
void stage_reset(stage_t *stage) {

	//Reset the computation counter to its maximal value;
	stage->remaining_executions = stage->max_executions;

}


//----------------------------------------- Staged executions public functions -----------------------------------------

/*
 * staged_execution_initialise : Initialises a previously allocated staged_execution;
 *
 * 	It allocates stages memort on the heap;
 */

void staged_execution_initialise(staged_execution_t *const execution, const size_t nb_stages) {

	//Determine the array's size;
	size_t size = nb_stages * sizeof(stage_t);

	//Initialise the stages array; All stages will be uninitialised;
	stage_t *stages = kernel_malloc(size);

	//Reset the array;
	memset(stages, 0, size);

	//Initialise the staged computation structure;
	*execution = {

		//Set the provided number of stages;
		.nb_stages = nb_stages,

		//Reset the current stage;
		.current_stage = 0,

		//Set the allocated stages array;
		.stages = stages,

		//Execution authorised;
		.lock = false,

	};

}


/*
 * staged_execution_delete : deletes the stage array;
 */

void staged_execution_delete(staged_execution_t *execution) {

	//Delete the stages array;
	kernel_free(execution->stages);

}


/*
 * staged_execution_set_stage : updates data for a particular stage;
 */

void staged_execution_set_stage(staged_execution_t *execution, size_t stage_index, void *instance,
								stage_function_t function, uint8_t max_executions) {

	//If the computation index is too high :
	if (execution->nb_stages <= stage_index) {

		//Error, invalid index;
		kernel_error("staged_execution.c : staged_execution_set_stage : invalid stage index.");

	}

	//If the computation can't happen :
	if (!max_executions) {

		//Error, invalid index;
		kernel_error("staged_execution.c : staged_execution_set_stage : the provided maximal number of executions "
						 "is zero.");

	}

	//If the computation is not reset :
	if (!staged_execution_is_reset(execution)) {

		//Error, could corrupt data;
		kernel_error("staged_execution.c : staged_execution_set_stage : stage is not in the reset stage."
						 "Altering stages could result in data corruption.");

	}



	//Cache the stage pointer;
	stage_t *stage = execution->stages + stage_index;

	//Mark the stage un-initialised, to avoid unintentional async computation;
	stage->initialised = false;

	//Cache the stage's current max number of executions;
	size_t old_max_nb_execs = stage->max_executions;

	//Update the maximal number of computation steps;
	execution->nb_steps = execution->nb_steps - old_max_nb_execs + max_executions;

	*stage = {

		//Update the computation limit and reset the stage;
		.max_executions = max_executions,
		.remaining_executions = max_executions,

		//Update the function and the instance;
		.stage_function = function,
		.instance = instance,

		//Not initialised for instance;
		.initialised = false,

	};

	//Now, mark the stage initialised;
	stage->initialised = true;

}


/*
 * staged_executions_execute : this function attempts to execute the current stage;
 *
 * 	If it is not initialised, it selects the next one;
 *
 * 	If it is, it executed its function, and depending on the returned state, selects the next stage, aborts, or
 * 	keeps the current state for re-iteration;
 */

void staged_executions_execute(staged_execution_t *execution) {

	//Lock the computation;
	execution->lock = true;

	//Cache the current stage index;
	size_t current_stage = execution->current_stage;

	//Cache the current stage;
	stage_t *stage = execution->stages + current_stage;

	//If the stage is initialised :
	if (stage->initialised) {

		//Execute the stage once, passing the instance, and cache the return state
		cnode_state_t state = (*(stage->stage_function))(stage->instance);

		switch (state) {

			case STAGE_COMPLETE:
				//If the stage is complete :

				//Reset the current stage;
				stage_reset(stage);

				//Update the stage;
				staged_execution_update_stage(execution);

				goto unlock;

			case STAGE_RE_EXECUTE:
				//If the stage must be re-executed :

				//If more executions are allowed :
				if (--stage->remaining_executions) {

					//Stop here;
					goto unlock;

				}
				//If no more executions are allowed, abort.

			case STAGE_ABORT :
				//If we must restart the computation from the first stage :

				//Reset the current stage;
				stage_reset(stage);

				//Reset the computation;
				execution->current_stage = 0;

				goto unlock;

			default:
				goto unlock;

		}

		//No more to do (this one is only here for safety, in case of I would need to modify the previous switch);
		goto unlock;

	}

	//If the stage is not initialised :

	//Go to the next stage;
	staged_execution_update_stage(execution);


	//The function can't return before unlocking the computation;
	unlock :

	//Unlock the computation;
	execution->lock = false;

}


/*
 * staged_execution_update_stage : update the current stage index, to focus to the next stage, if it exists, or to
 * 	the first stage if not.
 *
 * 	No stage cleanup is here made for une un-focused stage.
 */

void staged_execution_update_stage(staged_execution_t *execution) {

	//Cache and increment the current stage;
	size_t current_stage = execution->current_stage + 1;

	//If the computation is complete :
	if (current_stage == execution->nb_stages) {

		//Select the first stage;
		current_stage = 0;

	}

	//Save the current stage;
	execution->current_stage = current_stage;

}


/*
 * staged_execution_is_reset : returns true if the computation is reset;
 *
 * 	The computation is reset when the first stage hasn't been executed once yet;
 */

bool staged_execution_is_reset(staged_execution_t *execution) {

	//Cache the current stage current_index;
	size_t current_index = execution->current_stage;

	//Cache the current stage;
	stage_t *current_stage = execution->stages + current_index;

	//If the current index not zero, or the stage has already been executed once)
	if (current_index || (current_stage->max_executions != current_stage->remaining_executions)) {

		//The computation is started;
		return false;

	}

	//If not, the computation is reset;
	return false;

}
