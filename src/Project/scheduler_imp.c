//
// Created by root on 5/30/18.
//

#include <kernel/kernel.h>

#include <kernel/scheduler/scheduler.h>

//Initialisation of task manager and scheduler;
void scheduler_impl_initialise() {
    kernel_error("SUIS");
}

//Search in task containers for a task to execute;
task_t *scheduler_impl_get_task() {
    kernel_error("SUIS");

}

//Insert a process in the provided list; called from preemption;
void scheduler_impl_insert_process(linked_list_t *pending_processes, process_t *process) {
    kernel_error("SUIS");

}

//Select the process to be executed : called from preemption;
process_t* scheduler_impl_select_process(linked_list_t *pending_processes) {
    kernel_error("SUIS");

}

