//
// Created by root on 3/16/18.
//

#ifndef TRACER_ARGUMENTLANGUAGE_H
#define TRACER_ARGUMENTLANGUAGE_H


#include <Kernel/TaskScheduler/_task_scheduler_data.h>

#include "Language.h"

/*
 * A language that accepts arguments has a supplementary layer of complexity regarding of the simple language,
 *  as the arguments string must be passed to the language function at runtime.
 *
 *  To do this, we introduce a function that will be called at each command execution, and that will take care of
 *      unpacking arguments, process them and call the command;
 */

class ArgumentLanguage : public Language {


    //---------------------------------- Generic functions for command execution  ----------------------------------

    //Execute the command related to a decoded message;
    task_state_t _execute_command(void *);


    //----------------------------- virtual function for arguments processing -----------------------------

    //Parse the arguments string;
    virtual void parseArguments(const string &s) = 0;


protected:
    //Schedule a command related to a decoded message, with an argument pointer;
    void schedule_command(task_state_t (*)(), const char *);
};


#endif //TRACER_ARGUMENTLANGUAGE_H
