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


#include "hardware_language_abstraction.h"

/*
 * The Board Abstraction file.
 *
 * This file is made for : Arduino Mega - atmega2560.
 *
 * Here are defined all function related to the hardware (serial, I2C, delay etc...)
 *
 * Those will depend on the processor and the toolchain you use
 *
 * If you must implement functionalities over the base hardware functions, use an inline static function.
 *
 * If you only must alias a function, use a macro, it's faster.
 */



//--------------------------------------Microseconds_Steppers_Timer_Interrupt----------------------------------------------------

void (*us_function)(void);

void set_stepper_int_function(void (*f)()) {
    us_function = f;

}

void setup_stepper_interrupt(void (*function)(void), unsigned int period_us){
    cli();
    /*
     * Provides a microseconds counter, with a resolution of 0.5 us.
     * -> period must not be greater than 32767; Maximum period of 32.7 ms;
     */

    enable_stepper_timer();

    //CLEAN
    TCCR5A = 0;
    TCCR5B = 0;

    //Set trigger value
    set_stepper_int_period(period_us);

    TCCR5B = (1 << CS51)| (1<<WGM52);    // 8 prescaler
    TIMSK5|= (1 << OCIE5A);  // enable timer compare interrupt

    us_function = function;

    enable_stepper_interrupt();

    sei();
}

ISR(TIMER5_COMPA_vect) {
    (*us_function)();
}


//--------------------------------Control_loops_Milliseconds_Timer_Interrupt--------------------------------------------

void (*t0_function)(void);
void (*t1_function)(void);


/* The function to enable a timer interrupt, calling the given function every given period
 * void en_timer_int_i(void (*function)(void), int period_ms);
 */

void enable_loop_interrupt_0(void (*function)(void), unsigned int period_ms){
    cli();

    //CLEAN
    TCCR3A = 0;
    TCCR3B = 0;

    //Set trigger value
    OCR3A = 62*period_ms+period_ms>>1;

    TCCR3B |= (1 << CS32);    // 256 prescaler
    TIMSK3 |= (1 << OCIE3A);  // enable timer compare interrupt

    t0_function = function;

    sei();
}

void en_timer_int_1(void (*function)(void), unsigned int period_ms){
    cli();

    //CLEAN
    TCCR4A = 0;
    TCCR4B = 0;

    //Set trigger value
    OCR4A = 62*period_ms+period_ms>>1;

    TCCR4B |= (1 << CS42);    // 256 prescaler
    TIMSK4 |= (1 << OCIE4A);  // enable timer compare interrupt

    t1_function = function;

    sei();
}

ISR(TIMER3_COMPA_vect) {
    (*t0_function)();
}

ISR(TIMER4_COMPA_vect) {
    (*t1_function)();
}
