//
// Created by root on 4/11/18.
//

#include "task.h"

#include <malloc.h>
#include <string.h>
#include <kernel/kernel.h>


//---------------------- Public Functions ----------------------



/*
 * task_delete : delete entirely a task's content; Must be heap-stored;
 */

void task_delete(task_t *task_p) {

    //Free arguments;
    kernel_free((void *) task_p->args);

    //Free the task itself;
    kernel_free(task_p);

}
