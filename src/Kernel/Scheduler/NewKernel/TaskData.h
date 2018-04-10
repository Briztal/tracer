//
// Created by root on 4/9/18.
//

#ifndef TRACER_TASKDATA_H
#define TRACER_TASKDATA_H


class TaskData {

public:

    //Default Constructor;
    TaskData() : function(nullptr), args(nullptr), termination(nullptr) {};

    //Constructor;
    TaskData(void (*function)(void *), void *args, void (*termination)(void)) :
            function(function), args(args), termination(termination) {};

    //Copy constructor;
    TaskData(TaskData &src) : function(src.function), args(src.args), termination(src.termination) {};

public:

    //The task's function;
    void (*function)(void *args);

    //The function's arguments;
    void *args;

    //The termination function to execute after the task's function;
    void (*termination)();

};


#endif //TRACER_TASKDATA_H
