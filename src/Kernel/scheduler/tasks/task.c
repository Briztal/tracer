//
// Created by root on 4/11/18.
//

#include "task.h"

#include <malloc.h>


//---------------------- Private Functions ----------------------

//Delete a task's content;
void task_delete(task_t *);


//---------------------- Public Functions ----------------------

/*
 * task_create : allocates the memory zone for the task, and fill it with the provided data;
 */

task_t *task_create(void (*func)(void *), void *args, void (*cleanup)(), task_type_t origin) {

    //First, let's allocate a memory zone in the heap to contain our struct;
    void *data = malloc(sizeof(task_t));

    //Then, if the malloc failed, error;
    if (!data) {

        //Error TODO KERNEL PANIC;
        return 0;

    }

    //To ease readability, cache a copy of our pointer casted to the correct type. Optimised by the compiler;
    task_t *task_p = (task_t *)data;

    //Now, we can initialise our struct;
    *task_p = {
            .function = func,
            .args = args,
            .cleanup = cleanup,
            .task_type = origin,
            .sequence_id = 0,//Will zero all union's data;
    };

    //Finally, we can return our task;
    return task_p;
}


/*
 * task_cleanup : Cleans up the task data, depending on its origin;
 */

void task_cleanup(task_t *task_p) {

    //We must execute some specific code for each type of tasks;
    switch (task_p->task_type) {

        case SEQUENCE_TASK:

            /*
             * Sequence tasks are temporary. After their execution, them and their arguments must be fred;
             */

            //First, notify the sequencer that one of its task is finished;
            //TODO TaskSequencer::notify;

            task_delete(task_p);

            //nothing else to do;
            return;

        case PERSISTENT_TASK:

            /*
             * Synchronous and event tasks are stored in their respective containers;
             *  They may still exist after their execution. We will only delete it if it has been marked deletable;
             */

            if (task_p->persistent_task_state == CLOSED) {

                //Delete the task;
                task_delete(task_p);

            } else {

                //If the task has reached the cleanup function without being deletable, it is stored;
                task_p->persistent_task_state = STORED;
            }

            //nothing else to do;
            return;

    }

}


/*
 * task_delete : delete entirely a task's content; Must be heap-stored;
 */

void task_delete(task_t *task_p) {

    //Free arguments;
    free(task_p->args);

    //Free the task itself;
    free(task_p);

}
