/*
  hardware_language_abstraction.cpp - Part of TRACER

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

#include "kinetis.h"
#include "hardware_language_abstraction.h"

/*
 * The Board Abstraction file.
 *
 * This file is made for : teensy35
 *
 * Here are defined all function related to the hardware (serial, I2C, delay_us etc...)
 *
 * Those will depend on the processor and the toolchain you use
 *
 * If you must implement functionalities over the base hardware functions, use an inline static function.
 *
 * If you only must alias a function, use a macro, it's faster.
 */


//--------------------------------Control_loops_Milliseconds_Timer_Interrupt--------------------------------------------

#define CONTROL_LOOP_CODE(timer_index)\
/*Trigger functions*/\
void (*t##timer_index##_function)(void);\
/*Complete setup functions : sets period and function, and enable interrupts and timers.*/\
void setup_interrupt_##timer_index(void (*function)(void), uint32_t period_ms) {\
    cli();\
    set_int_period_##timer_index(period_ms);\
    set_interrupt_function_##timer_index(function);\
    enable_interrupt_##timer_index();\
    enable_timer_##timer_index();\
    sei();\
}\
/*//Function setting*/\
void set_interrupt_function_##timer_index(void (*f)()) {\
    t##timer_index##_function = f;\
}\
/*Implementation of interrupt functions, calling trigger functions*/\
void pit##timer_index##_isr() {\
    PIT_TFLG##timer_index = 1;\
    (*t##timer_index##_function)();\
}\


CONTROL_LOOP_CODE(0)

CONTROL_LOOP_CODE(1)

CONTROL_LOOP_CODE(2)

CONTROL_LOOP_CODE(3)



