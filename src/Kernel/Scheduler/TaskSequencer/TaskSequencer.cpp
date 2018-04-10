//
// Created by root on 3/23/18.
//

#include <Kernel/Interaction/Interaction.h>
#include <Kernel/Kernel.h>
#include "TaskSequencer.h"


//-------------------------------------------- Const variable init --------------------------------------------

/*
 * The number of task sequences is a constant, directly determinable by counting the number of times TASK_SEQUENCES
 *  is written in the configuration file.
 *
 *  Instead of asking the user to provide it, what could lead to errors, we will use a compiler constant
 *      determined directly using macro;
 *
 */

#define TASK_SEQUENCE(...) 1 +

const uint8_t NB_TASK_SEQUENCES =

#include "Config/kernel_config.h"

        0;

#undef TASK_SEQUENCE


namespace TaskSequencer {

    //Task sequences;
    Queue<task_t> *task_sequences[NB_TASK_SEQUENCES]{nullptr};

    //First tasks flags;
    bool processing_disabled[NB_TASK_SEQUENCES]{false};

    //First tasks flags;
    bool insertion_disabled[NB_TASK_SEQUENCES]{false};

    bool schedule_task(task_t *task);
}


/*
 * initialise_data : initialises the sequencer in a safe state;
 */

void TaskSequencer::initialise_data() {

    //First, delete all sequences;
    for (uint8_t sequences_id = NB_TASK_SEQUENCES; sequences_id--;) {

        //Delete the previous sequence;
        delete task_sequences[sequences_id];

    }

    //Then, recreate them;
    uint8_t sequence_id = 0;

#define TASK_SEQUENCE(size) task_sequences[sequences_id++] = new Queue<task_t>(size);

//TODO #include "Config/kernel_config.h"

#undef TASK_SEQUENCE

}


//--------------------------------------------------Type Verification---------------------------------------------------

bool TaskSequencer::check_sequence_type(uint8_t type) {

    if (type == 255) {
        return true;
    } else {
        return (type < NB_TASK_SEQUENCES);
    }

}


/*
 * process_task_sequences : this function processes task sequences.
 *
 *  It executes one task for each sequence, until no tasks are executable anymore.
 */

void TaskSequencer::process() {

    //For each task sequence :
    for (uint8_t sequence_id = 0; sequence_id < NB_TASK_SEQUENCES; sequence_id++) {

        //Cache the task sequence;
        Queue<task_t> *sequence = task_sequences[sequence_id];

        /* TODO
        //If the sequence can be processed :
        if (process[sequence_id]) {

            //If the sequence still has tasks to process
            if (sequence->available_objects()) {

                //Declare a flag
                bool queue_flag = false;

                //Get the task to execute
                task_t *task = sequence->get_reading_address(&queue_flag);

                //Integrity check
                if (!queue_flag) {

                    //Log
                    std_out("ERROR in TaskSequencer::process : "
                                    "the reading element is not allocated.");

                    //Emergency stop
                    Kernel::emergency_stop();

                    return;

                }


                //If the executed task must be reprogrammed
                if (!execute_task(task)) {

                    //Disable this sequence's processing
                    process[sequence_id] = false;


                } else {
                    //If the task was processed :

                    //Reset the flag
                    queue_flag = false;

                    //Go to the other task
                    sequence->discard(&queue_flag);

                    //Integrity check
                    if (!queue_flag) {

                        //Log
                        std_out("ERROR in Kernel::process_task_sequences : "
                                        "the discarded element was not allocated.");

                        //Emergency stop
                        Kernel::emergency_stop();

                        return;

                    }

                    //Enable the next processing
                    keep_processing = true;

                }

            } else {

                //Disable this sequence's processing
                process[sequence_id] = false;

            }

        }*/


    }

}


/*
 * schedule_task : this function adds the task, if it is possible, in :
 *  - the task pool if its type is 255;
 *  - a task sequence if its type corresponds to a task sequence.
 *
 *  WARNING :
 *
 *  I REPEAT, {\Color(red) \textit{\textbf{WARNING : }}}
 *      (If you got that one, you use latex, and you're a good person ! Thanks for reading my comments by the way ;-) )
 *
 *  If the task cannot be scheduled, (no space available, or bad sequence type), arguments_p are freed automatically.
 */

bool TaskSequencer::schedule_task(task_t *task) {

    /*
    if ((type == 255) && (pool_task_spaces)) {

        //copy the task;
        task_pool[pool_task_nb] = *task;

        //Increase the number of task in the pool;
        pool_task_nb++;

        //Decrease the number of data_spaces in the pool;
        pool_task_spaces--;

        //Succeed;
        return true;

    }


    //If the task's type corresponds to an existing sequence:
    if ((type < NB_TASK_SEQUENCES) && (task_sequences[type]->available_spaces())) {

        //First, cache the address of the current task sequence;
        Queue<task_t> *queue = task_sequences[type];

        //Initialise a flag;
        bool queue_flag = false;

        //Copy the task in the sequence;
        queue->insert_object(task, &queue_flag);


        //Copy the task in the sequence;
        *task_sequences[type]->get_insertion_address(&queue_flag) = *task;

        //Integriy check
        if (!queue_flag) {

            //Log
            std_out("ERROR in Kernel::schedule_task : failed to copy the task in the queue.");

            //Emergency stop
            Kernel::emergency_stop();

        } else {

            //Succeed;
            return true;

        }


    }

    //If the scheduling failed, automatically free arguments_p, to avoid memory leak;
    free(task->dynamic_args);


    //Fail;
    return false;

     */
}


/*
 * schedule_task : this function creates a task and adds it to the task pool.
 *
 *  It takes in content all parameters required to build a task, namely :
 *      - function : the function to parse;
 *      - dynamic_args : a void *, addressing the first byte of the content. Those may be dynamic or static.
 *      - auto_free : set if dynamic_args must be freed automatically by the scheduler (if they are on the heap, for example).
 *      - type : the type of the task.
 */

bool TaskSequencer::schedule_task(uint8_t type, task_state_t (*f)(void *), void *args) {

    /*
    //Create a task to contain the provided data;
    task_t task = task_t();

    //Set the function to execute;
    task.task = f;

    //Set the argument_t pointer;
    task.dynamic_args = args;

    //Set the type;
    task.type = type;

    //Get the communication pipe that the task will use;
    task.log_pipe = Interaction::getCommunicationPipe();

    //Call the scheduling function and return whether the task was successfully scheduled;
    return schedule_task(&task);
     */

}


/*
 * nb_spaces : this function returns the number of nb_spaces available in the task pool.
 */

/*
const uint8_t TaskSequencer::available_spaces(uint8_t type) {

    if (type == 255) {

        //If the type corresponds to the task pool : return the number of nb_spaces of the pool;
        return pool_task_spaces;

    } else {

        //If the type is not allocated, return zero;
        if (!check_sequence_type(type))
            return 0;

        //If the type corresponds to a sequence, return the number of nb_spaces in the concerned sequence;
        return task_sequences[type]->available_spaces();

    }
}

 */
