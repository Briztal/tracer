/*
  hardware_language_abstraction.h - Part of TRACER

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

/*
 * hl_config : this file_tree configures the hardware abstraction layer.
 *
 *  In this file_tree, you must :
 *
 *      - Configure timer interrupts;
 *      - Set interrupt priorities;
 */


/*
 * Timer interrupt frequencies :
 *
 *  For every timer you want to use (timers are indexed from 0 to n-1, n being the number of timers),
 *      please define a macro names TIMER_N_FREQUENCY (replace N with the index)
 *      with the desired interrupt frequency;
 *
 *  You must set these frequencies accordingly with timer allocation, as module may require a particular frequency :
 *      - the steppers module requires a 1MHz frequency;
 *      - the servo module requires a 1MHz frequency;
 *      - control loops require a 1KHz frequency;
 *
 */

#define TIMER_STEPPER_FREQUENCY 1000000
#define TIMER_SERVO_FREQUENCY 1000000
#define TIMER_LOOP_FREQUENCY 1000

//Timer 0 and 1 will count on microseconds, at 1MHz
#define TIMER_0_FREQUENCY TIMER_STEPPER_FREQUENCY
#define TIMER_1_FREQUENCY TIMER_SERVO_FREQUENCY

//Timer 2 and 3 will count on milliseconds, at 1KHz
#define TIMER_2_FREQUENCY TIMER_LOOP_FREQUENCY
#define TIMER_3_FREQUENCY TIMER_LOOP_FREQUENCY



/*
 * Timer allocations :
 *
 *  As some modules require the use of a timer, you must manually link functions they use to the
 *      appropriate timer functions (macros are advised);
 *
 *  Even if all functions are not required by all modules, I advise you to implement all functions,
 *      for safety. You should simply copy the next section, and replace [name] and [index] with (resp)
 *      the module name and the timer index.
 *
 *  Module names are :
 *      - machine_control for the machine_control module;
 *      - servo for the servo module;
 *      - loop_i for the i-th control loop of the control-loops module;

 Code section to copy :


#define [name]_period_to_reload(period) period_to_reload_[index](period);
#define set_[name]_int_period(period) set_int_period_[index](period);
#define set_[name]_int_reload(reload) set_int_reload_[index](reload);
#define enable_[name]_interrupt() enable_interrupt_[index]();
#define disable_[name]_interrupt() disable_interrupt_[index]();
#define enable_[name]_timer() enable_timer_[index]();
#define disable_[name]_timer() disable_timer_[index]();
#define [name]_int_flag() timer_flag_[index]()
#define [name]_int_flag_clear() reset_timer_flag_[index]();
#define set_[name]_int_function(f) set_interrupt_function_[index](f);
#define setup_[name]_interrupt(f, period) setup_interrupt_[index](f, period);
#define clean_[name]_interrupt() clean_interrupt_[index]();
#define is_[name]_loop_enabled() is_timer_loop_enabled_[index]();

 */


/*
 * The steppers timer will be attached to timer zero.
 */

#define stepper_period_to_reload(period) period_to_reload_0(period);
#define set_stepper_int_period(period) set_int_period_0(period);
#define set_stepper_int_reload(reload) set_int_reload_0(reload);
#define enable_stepper_interrupt() enable_interrupt_0();
#define disable_stepper_interrupt() disable_interrupt_0();
#define enable_stepper_timer() enable_timer_0();
#define disable_stepper_timer() disable_timer_0();
#define stepper_int_flag() timer_flag_0()
#define stepper_int_flag_clear() reset_timer_flag_0();
#define set_stepper_int_function(f) set_interrupt_function_0(f);
#define setup_stepper_interrupt(f, period) setup_interrupt_0(f, period);
#define clean_stepper_interrupt() clean_interrupt_0();
#define is_stepper_loop_enabled() is_timer_loop_enabled_0();


/*
 * The servo timer will be attached to timer one.
 */

#define servo_period_to_reload(period) period_to_reload_1(period);
#define set_servo_int_period(period) set_int_period_1(period);
#define set_servo_int_reload(reload) set_int_reload_1(reload);
#define enable_servo_interrupt() enable_interrupt_1();
#define disable_servo_interrupt() disable_interrupt_1();
#define enable_servo_timer() enable_timer_1();
#define disable_servo_timer() disable_timer_1();
#define servo_int_flag() timer_flag_1()
#define servo_int_flag_clear() reset_timer_flag_1();
#define set_servo_int_function(f) set_interrupt_function_1(f);
#define setup_servo_interrupt(f, period) setup_interrupt_1(f, period);
#define clean_servo_interrupt() clean_interrupt_1();
#define is_servo_loop_enabled() is_timer_loop_enabled_1();





//-------------------------------------------------------- Loops -------------------------------------------------------


/*
 * If unallocated timers remain, you can generate loop classes with them.
 *
 * To do this, use the following line :
 *
 *  GENERATE_LOOP(loop_index, timer_index)
 *
 *      - loop_index : the index of the class (use 1 and class Loop1 will be generated);
 *      - timer_index : the index of the timer;
 */

#ifdef GENERATE_LOOP

GENERATE_LOOP(0, 2)

GENERATE_LOOP(1, 3)

#endif


//------------------------------------------------INITIALISATION--------------------------------------------------------


/*
 * Interrupts initialisation function : set the correct priorities, enable interrupts,
 *  and disables timers and interrupts
 */

#define init_interrupts() {\
    SIM_SCGC6 |= SIM_SCGC6_PIT; __asm__ volatile("nop");PIT_MCR = 1;\
    NVIC_SET_PRIORITY(IRQ_PIT_CH0, 0);NVIC_ENABLE_IRQ(IRQ_PIT_CH0);\
    NVIC_SET_PRIORITY(IRQ_PIT_CH1, 16);NVIC_ENABLE_IRQ(IRQ_PIT_CH1);\
    NVIC_SET_PRIORITY(IRQ_PIT_CH2, 32);NVIC_ENABLE_IRQ(IRQ_PIT_CH2);\
    NVIC_SET_PRIORITY(IRQ_PIT_CH3, 48);NVIC_ENABLE_IRQ(IRQ_PIT_CH3);\
    clean_interrupt_0();clean_interrupt_1();clean_interrupt_2();clean_interrupt_3();}


#define hl_init() init_interrupts();


