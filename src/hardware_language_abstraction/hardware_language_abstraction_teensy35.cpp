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
#include "hardware_language_abstraction_teensy35.h"

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

//------------------------------------------------INITIALISATION--------------------------------------------------------

//Initialisation function : sets the correct priotity, and disables the timer and interrupt

#define init_stepper_interrupt() {SIM_SCGC6 |= SIM_SCGC6_PIT; __asm__ volatile("nop");PIT_MCR = 1;\
    NVIC_SET_PRIORITY(IRQ_PIT_CH0, 0);NVIC_ENABLE_IRQ(IRQ_PIT_CH0); clean_stepper_interrupt();}


//Initialisation function : set the correct priorities, enable interrupts, and disables timers and interrupts

#define init_loop_interrupts() {SIM_SCGC6 |= SIM_SCGC6_PIT; __asm__ volatile("nop");PIT_MCR = 1;\
    NVIC_SET_PRIORITY(IRQ_PIT_CH1, 16);NVIC_ENABLE_IRQ(IRQ_PIT_CH1);\
    NVIC_SET_PRIORITY(IRQ_PIT_CH2, 32);NVIC_ENABLE_IRQ(IRQ_PIT_CH2);\
    NVIC_SET_PRIORITY(IRQ_PIT_CH3, 48);NVIC_ENABLE_IRQ(IRQ_PIT_CH3);\
    clean_loop_interrupt_0();clean_loop_interrupt_1();clean_loop_interrupt_2();}

void hl_begin() {

    init_stepper_interrupt();

    init_loop_interrupts();

}

//--------------------------------------Microseconds_Steppers_Timer_Interrupt-------------------------------------------
void nop() {}


//Trigger function

void (*us_function)(void) = nop;


//Function set

void set_stepper_int_function(void (*f)()) {
    us_function = f;

}

//Complete setup

void setup_stepper_interrupt(void (*function)(void), uint32_t period_us) {

    cli();

    set_stepper_int_period(period_us);

    set_stepper_int_function(function);

    enable_stepper_interrupt();

    enable_stepper_timer();

    sei();
}


//Implementation of interrupt function, calling trigger function

void pit0_isr() {
    PIT_TFLG0 = 1;
    (*us_function)();
}

//--------------------------------Control_loops_Milliseconds_Timer_Interrupt--------------------------------------------

//Trigger functions

void (*t0_function)(void);

void (*t1_function)(void);

void (*t2_function)(void);


//Function setting

void set_loop_function_0(void (*f)()) {
    t0_function = f;
}

void set_loop_function_1(void (*f)()) {
    t1_function = f;
}

void set_loop_function_2(void (*f)()) {
    t2_function = f;
}


//Complete setup functions : sets period and function, and enable interrupts and timers.

void setup_loop_interrupt_0(void (*function)(void), uint32_t period_ms) {

    cli();

    set_loop_int_period_0(period_ms);

    set_loop_function_0(function);

    enable_loop_interrupt_0();

    enable_loop_timer_0();

    sei();

}

void setup_loop_interrupt_1(void (*function)(void), uint32_t period_ms) {

    cli();

    set_loop_int_period_1(period_ms);

    set_loop_function_1(function);

    enable_loop_interrupt_1();

    enable_loop_timer_1();

    sei();

}

void setup_loop_interrupt_2(void (*function)(void), uint32_t period_ms) {

    cli();

    set_loop_int_period_2(period_ms);

    set_loop_function_2(function);

    enable_loop_interrupt_2();

    enable_loop_timer_2();

    sei();

}


//Implementation of interrupt functions, calling trigger functions

void pit1_isr() {
    PIT_TFLG1 = 1;
    (*t0_function)();
}

void pit2_isr() {
    PIT_TFLG2 = 1;
    (*t1_function)();
}

void pit3_isr() {
    PIT_TFLG3 = 1;
    (*t2_function)();
}


