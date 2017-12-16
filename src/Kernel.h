/*
  Kernel.h Part of TRACER

  Copyright (c) 2017 Raphaël Outhier

  TRACER is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  TRACER is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  aint32_t with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/

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
     * initialise_hardware : this function is called once, by start only. It is the hardware initialisation function.
     *
     *  As its name suggests, it will initialise the hardware for all interfaces.
     */

    static void initialise_hardware();


    /*
     * initialise_data : this function is called after the restoration point. It resets every module's processing
     *
     *  environment.
     */

    static void initialise_data();



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

};


#endif //TRACER_KERNEL_H
