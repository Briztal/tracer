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


#include <config.h>

#ifdef ENABLE_CONTROL_LOOPS

#include <hardware_language_abstraction.h>

//Define a macro to implement a loop class;
#define GENERATE_LOOP(loop_index, timer_index)\
class Loop##loop_index {\
    public:\
    static inline void start(void (*loop_f)(), float period_ms) {setup_interrupt_##timer_index(loop_f, period_ms);}\
    static inline void stop() {disable_interrupt_##timer_index();}\
    static inline bool active() {return (bool) is_timer_loop_enabled_##timer_index();}\
};

//Implement all loop classes;
#include "Config/hl_config.h"

#undef GENERATE_LOOP

#endif //TRACER_CONTROLLOOPS_H

#endif