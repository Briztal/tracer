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
  along with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "kinetis.h"
#include "hardware_language_abstraction_teensy35.h"

/*
 * The Board Abstraction file.
 *
 * This file is made for : teensy35
 *
 * Here are defined all function related to the hardware (serial, I2C, delay etc...)
 *
 * Those will depend on the processor and the toolchain you use
 *
 * If you must implement functionalities over the base hardware functions, use an inline static function.
 *
 * If you only must alias a function, use a macro, it's faster.
 */

//--------------------------------------Microseconds_Steppers_Timer_Interrupt-------------------------------------------
void nop() {}

void (*us_function)(void) = nop;

void set_stepper_int_function(void (*f)()) {
    us_function = f;

}

void setup_stepper_interrupt(void (*function)(void), unsigned int period_us) {

    cli();

    set_stepper_int_period(period_us);

    set_stepper_int_function(function);

    enable_stepper_interrupt();

    enable_stepper_timer();

    sei();
}


void pit0_isr() {
    PIT_TFLG0 = 1;
    (*us_function)();
}



//--------------------------------Control_loops_Milliseconds_Timer_Interrupt--------------------------------------------

void (*t0_function)(void);

void (*t1_function)(void);


/* The function to enable a timer interrupt, calling the given function every given period
 * void en_timer_int_i(void (*function)(void), int period_ms);
 */

void enable_loop_interrupt_0(void (*function)(void), unsigned int period_ms) {

}

void en_timer_int_1(void (*function)(void), unsigned int period_ms) {

}
