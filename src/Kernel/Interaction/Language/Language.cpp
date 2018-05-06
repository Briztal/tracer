//
// Created by root on 3/15/18.
//

#include <Kernel/Scheduler/TaskScheduler/TaskScheduler.h>
#include "Language.h"


struct language_command_t {

    //The instance on which to execute the function;
    Language *instance;

    //The function itself;
    //task_state_t (Language::*method)(void *);
    language_function function;

    //The eventual argument sequence;
    string *arguments_p;

};


/*
 * schedule_command : schedule a particular function;
 */

void Language::schedule_command(Language *instance, language_function function, const char *arguments_p) {

    //Create a data struct in the heap;
    language_command_t *data = new language_command_t();

    //Copy the instance and the member pointers in the heap;
    data->instance = instance;
    data->function = function;
    data->arguments_p = new string(arguments_p);

    //Schedule the execution of the command;
    //TODO TaskScheduler::sequences_add_task(0, execute_command, (void *)data);

}


/*
 * execute_command : unpacks data packed by the function beyond, and executed the function;
 */

task_state_t Language::execute_command(void *data_p) {

    //Cast the pointer to adequate type;
    language_command_t *data = (language_command_t *)data_p;

    //Parse Arguments;
    (data->instance)->parseArguments(data->arguments_p->data());

    //Execute the function;
    task_state_t return_state = (data->function)();

    //If the function mustn't be reprogrammed, delete arguments;
    if (return_state != reprogram) {

        delete data->arguments_p;
        delete data;

    }

    //Return the state of the command;
    return return_state;

}
