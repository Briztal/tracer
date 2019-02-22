/*
  atm.h Part of nostd

  Copyright (c) 2018 Raphaël Outhier

  nostd is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  nostd is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  aint32_t with nostd.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef NOSTD_ATM_H
#define NOSTD_ATM_H

#include <stdbool.h>

#include <stddef.h>
#include <mem/queue.h>

#include "tree.h"

#include "list.h"

/*
 * atm_task_type : describes the type of an atm task, depending on if it can spawn new tasks;
 */

enum atm_task_type {

    /*The task can start new tasks;*/
        ATM_TASK_SPAWNER,

    /*The task can't start new tasks;*/
        ATM_TASK_NON_SPAWNER,

};


/*
 * atm_function : can contain all different forms of atm functions;
 */

union atm_function {

    /*A spawner function;*/
    bool (*f_spawner)(void *args, size_t args_size);

    /*A non spawner function;*/
    bool (*f_non_spawner)(void *args, size_t args_size, struct atm_exec *exec);

};

/*
 * atm_task : contains data to operate on a task : storage refs, task description and access signals;
 */

struct atm_task {

    /*Ready and idle tasks are (resp) referenced in linked lists;*/
    struct list_head *ex_head;

    /*Tasks are referenced in a tree;*/
    struct tree_head t_tree;

    /*The task environment the task relates to;*/
    struct atm_entry_point *t_entry;

    /*The type of the task;*/
    enum atm_task_type t_type;

    /*The function the task must execute;*/
    union atm_function;

};

/*From a task, spawn a task that has the ability to spawn tasks;*/
void atm_spawn_task_spawner(struct atm_task *src_task, bool (*f_spawner)(void *, size_t, struct atm_task *));

/*From a task, spawn a task that doesn't have the ability to spawn tasks;*/
void atm_spawn_task_non_spawner(struct atm_task *src_task, bool (*f_non_spawner)(void *, size_t));




/*The execution options type;*/
#define env_opt_t uint8_t

/*
 * atm_env_desc : contains all constant data that describe a task execution environment;
 */

struct atm_env_desc {

    /*Execution options;*/
    env_opt_t exec_options;

    /*The maximal number of concurrent entries;*/
    const size_t ed_max_entries;

    /*The size of the struct containing resources the task function requires;*/
    const size_t ep_res_size;

    /*The size of the struct containing args the task function requires;*/
    const size_t ep_args_size;

    /*A function in charge of locking resources the task func requires; Asserts if resources are successfully locked;*/
    bool (*const ep_res_locker)(void *res, size_t res_size);

    /*The task function; Takes a resource struct, and args struct, and and a atm task ref;*/
    bool (*const ep_task)(void *res, size_t res_size, void *args, size_t args_size, struct atm_task *exec);

    /*Size of the args queue list;*/
    const size_t ed_args_queue_size;

};


/*If set, only one instance of the entry point can be executed at the time;*/
#define ATM_EXEC_OPT_ONCE_AT_TIME ((env_opt_t) (1 << 0))

/**
 * atm_env : a task execution environment; composed of :
 * 	- a function, the entry point, that takes an argument blob of a determined size, and can spawn tasks;
 * 	- an arguments queue, that can store a determined number of arguments for the entry points;
 * 	- a determined number of tasks;
 */

struct atm_env {

    /*The entry descriptor;*/
    struct atm_env_desc en_descriptor;

    /*The arguments queue;*/
    struct queue en_args_queue;

    /*The array of tasks;*/
    struct atm_task *const en_tasks;

    /*The first idle task;*/
    struct atm_task *en_idle_task;

    /*The first ready task;*/
    struct atm_task *en_ready_task;

};


/*
 * When the execution function is called, several cases can occur;
 */

enum atm_execution_status {

    /*No task was ready, nothing was executed;*/
        EXEC_NO_TASK_READY,

    /*A function different of the entry function was executed;*/
        EXEC_TASK_EXECUTED,

    /*The entry function was executed;*/
        EXEC_ENTRY_EXECUTED,

};


/*
 * atm_task_manager : references a set of entry points;
 */

struct atm_task_manager {

    /*The number of execution environments;*/
    const size_t tm_nb_envs;

    /*The array of execution environments;*/
    struct atm_env *const tm_envs;

};

/*Construct a task manager from a set of environment descriptors;*/
bool atm_ctor(struct atm_task_manager *atm, size_t nb_env, const struct atm_env_desc *descriptors);

/*Enqueue arguments for a given environment;*/
bool atm_enqueue(const struct atm_task_manager *atm, size_t env_index, const void *args, size_t args_size);

/*Enqueue arguments for a given environment only if the args queue is empty;*/
bool atm_enqueue_if_none(const struct atm_task_manager *atm, size_t env_index, const void *args, size_t args_size);

/*Execute a task in a given environment;*/
enum atm_execution_status atm_execute(const struct atm_task_manager *atm, size_t env_index);

/*Destruct a task manager;*/
bool atm_dtor(struct atm_task_manager *atm);

#endif /*NOSTD_ATM_H*/
