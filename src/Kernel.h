//
// Created by root on 09/12/17.
//

#ifndef TRACER_KERNEL_H
#define TRACER_KERNEL_H


#include "setjmp.h"

class Kernel {


    //------------------------------------------- Entry Point -------------------------------------------

public:

    /*
     * start : this function is called once, by main only. It is the project's entry point.
     *
     *  It will call initialise_data, and then call iterate indefinitely.
     */

    static void start();

private:

    //The start flag, set only once, at the entry point. Prevents start to be called a second time.
    static bool started;


    //------------------------------------------- Initialisation -------------------------------------------

private:

    /*
     * initialise : this function is called once, by start only. It is the project initialisation function.
     *
     *  As its name suggests, it will initialise_data every module of the code.
     */

    static void initialise();


    //------------------------------------------- Main Loop -------------------------------------------

private:

    /*
     * run : this function is the main loop of the project.
     *
     *  It calls a Scheduler iteration , and eventually executes user defined routines.
     */

    static void run();


    //------------------------------------------- Emergency Stop -------------------------------------------

public:

    /*
     * emergency_stop : this function stops the code, and long-jumps to the entry point.
     *
     *  Any unhandled exception in the code calls this function, to avoid any unplanned behaviour.
     */

    static void emergency_stop();

    static jmp_buf restoration_point;


    //------------------------------------------- Logo Display -------------------------------------------

private:

    /*
     * The following function displays a logo at the initialisation of the code.
     *
     * //TODO I'll add some macros, to accelerate the version / author customisation.
     */

    static void display_logo();


};


#endif //TRACER_KERNEL_H
