//
// Created by root on 3/16/18.
//

#include <Kernel/TaskScheduler/TaskScheduler.h>
#include "ArgumentLanguage.h"


//---------------------------------- Generic function for command execution  ----------------------------------


struct command_data_t {

    //The language instance to call command on;
    ArgumentLanguage *instance;

    //The command to execute;
    task_state_t (*command)(void);

    //The arguments string;
    string *arguments_string;

};


void ArgumentLanguage::schedule_command(task_state_t (*function)(void), const char *arguments) {

    //Create a new command_data_t in the heap;
    command_data_t *data = new command_data_t;

    //Create a new string in the heap to contain arguments;
    string *args_copy = new string(arguments);

    //Copy data in the command;
    data->instance = this;
    data->arguments_string = args_copy;
    data->command = function;

    /*
     * Schedule a type 255 (asap) task, to parse the required function,
     *  with data as content, the log function, and the output delimiter;
     */

    TaskScheduler::schedule_task(255, _execute_command, (void *) data, external_log, output_protocol);

    //Complete
    return;

}


/*
 * _execute_task : this function is called when a command must be executed;
 */

task_state_t ArgumentLanguage::_execute_command(void *data_pointer) {

    //Get the terminal interface data back
    command_data_t *data = (command_data_t *) data_pointer;

    //Parse the argument string;
    parseArguments(*data->arguments_string);

    //Execute the required instance's function, and get the execution state
    const task_state_t state = ((data->instance)->(*data->command))();

    //if the task mustn't be reprogrammed :
    if (state != reprogram) {

        //delete the arguments string;
        delete data->arguments_string;

        //Delete the data;
        delete data;

    }

    //Return the execution state.
    return state;

}
