//
// Created by root on 17/10/17.
//

#include "interface.h"


#include <TaskScheduler/TaskScheduler.h>

void initialise_interfaces() {

#ifdef ENABLE_GCODE_INTERFACE
    GI::init();
#endif

#ifdef ENABLE_PROGRAM_INTERFACE
    TI::init();
#endif

#ifdef ENABLE_TERMINAL_INTERFACE
    UI::init();
#endif

}

void read_interfaces() {

    if (!TaskScheduler::available_spaces(0))
        return;

#ifdef ENABLE_TERMINAL_INTERFACE
    UI::read_serial();
#endif

    if (!TaskScheduler::available_spaces(0))
        return;

#ifdef ENABLE_PROGRAM_INTERFACE
    TI::read_data();
#endif

    if (!TaskScheduler::available_spaces(0))
        return;

#ifdef ENABLE_GCODE_INTERFACE
    GI::read_data();
#endif

}
