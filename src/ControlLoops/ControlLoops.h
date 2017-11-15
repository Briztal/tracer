/*
  ControlLoops.h - Part of TRACER

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
#ifndef TRACER_CONTROLLOOPS_H
#define TRACER_CONTROLLOOPS_H

#include <hardware_language_abstraction.h>

class ControlLoops {

    /*
    * Loop functions are routines that are called regularly with interrupts.
    *
    * They are defined by the function they call and the period related.
    *
    */

#define LOOP_FUNCTION(indice, name, period_ms) \
public:\
    /*enable function, enables the loop (interrupt and timer).*/\
    static inline void enable_##name(){\
        initialisation_function_##name();\
        setup_loop_interrupt_##indice(loop_function_##name, period_ms);\
    }\
    /*disable function, disables the loop (interrupt and timer).*/\
    static inline void disable_##name(){\
        finalisation_function_##name();\
        disable_loop_interrupt_##indice();\
    }\
    /*state function : returns true if the loop is enabled.*/\
    static inline bool is_active_##name() {\
        return is_loop_enabled_##indice();\
    }\
    \
private:\
    /*loop function : the function that is called when the interrupt occurs.*/\
    static void loop_function_##name();\
    /*initialisation_function : the function that is called when the loop is enabled.*/\
    static void initialisation_function_##name();\
    /*finalisation_function : the function that is called when the loop is disabled.*/\
    static void finalisation_function_##name();\


#include <config.h>

#undef LOOP_FUNCTION

};


#endif //TRACER_CONTROLLOOPS_H
