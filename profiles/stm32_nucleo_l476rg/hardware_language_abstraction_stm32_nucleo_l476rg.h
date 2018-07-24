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

#ifndef HDWLABSTRACTION_H
#define HDWLABSTRACTION_H

/*
 * The Board Abstraction file_tree.
 *
 * This file_tree is made for : stm32 nucleo l476rg.
 *
 * Here are defined all function related to the hardware (serial, I2C, delay_us etc...)
 *
 * Those will depend on the processor and the toolchain you use
 *
 * If you must implement functions over the base hardware functions, use a function.
 *
 * If you only must alias a function, use a macro, it's faster.
 */

//---------------------------------------------------Includes-----------------------------------------------------------

/*
 * Include here all libraries related to the board itself
 */
#include <stdint.h>
#include <string>
#include <strings.h>
#include <stdlib.h>
#include <targets/TARGET_STM/TARGET_STM32L4/TARGET_STM32L476xG/device/stm32l476xx.h>
//------------------------------------------------HARDWARE_FLAGS--------------------------------------------------------


#define HL_8BITS_FLAG

#define HL_16BITS_FLAG

#define HL_32BITS_FLAG

#define HL_FPU_FLAG

#define HL_SERIAL_FLAG

#define HL_CONTROL_LOOPS_FLAG

#define HL_STEPPER_TIMER_FLAG

#define HL_IO_FLAG

#define HL_INTERRUPTS_FLAG

#define HL_DELAY_FLAG

#define HL_EEPROM_FLAG


//------------------------------------------------INITIALISATION--------------------------------------------------------


void hl_begin();


//----------------------------------------------------STRING------------------------------------------------------------

#define string_t std::string

#define str(t) (std::string) ""

#define str_to_float(s) 0


//----------------------------------------------------Storage------------------------------------------------------------

#define EEPROM_read(i) (*(uint8_t *)(DATA_E2_ADDR+i))

#define EEPROM_write(i, byte) *(uint8_t *)(DATA_E2_ADDR+i) = byte;

//TODO INITIALISE / UNLOCK Storage ??

//-----------------------------------------------------IO--------------------------------------------------------------

#define digital_write(i, v)

#define digital_read(i)

#define analog_read(i)

#define analog_write(i, v)

#define pin_mode_output(i)


//-----------------------------------------------------MATH-------------------------------------------------------------


#define sqrt_float(f) 1
#define sqrt_long(l) 1

#define min(a,b) (((a)<(b)) ? (a) : (b))
#define max(a,b) (((a)<(b)) ? (b) : (a))

//--------------------------------------------------INTERRUPTS----------------------------------------------------------

/*
 * An alias for external interrupt attachment and detachment
 */
#define attach_interrupt(pin, port, mode) attachInterrupt(pin, port, mode)

#define detach_interrupt(pin) detachInterrupt(pin);


//----------------------------------------------------Delay-------------------------------------------------------------

//Uncomment this line to provide delay_us support across LowLevel

/* The function to call to wait for a specified number of milliseconds
 *  void delay_ms(uint16_t time_ms);
 *//*
inline static void delay_ms(uint16_t time_ms){

}*/
#define delay_ms(ms)


/* The function to call wait for a specified number of microseconds
 * void delay_us(uint16_t time_us);
 */
/*inline static void delay_us(uint16_t time_us){

}*/
#define delay_us(us)


//--------------------------------------StepperTimer_Interrupt----------------------------------------------------

//For the steppers interrupt, the TIM2 (32 bis is used)

#define F_BUS 48000000

//The basic unit of the implemented timer is 1 us.
#define STEPPER_TIMER_FREQUENCY (float) 1000000.0

#define STEPPER_TIMER_TICS_PER_UNIT ((float) ((float) F_BUS / STEPPER_TIMER_FREQUENCY ))

#define STEPPER_TIMER_MAX_PERIOD ((float) (UINT32_MAX / STEPPER_TIMER_TICS_PER_UNIT))

//Period setting : WARNING, the period is expressed into timer unit, a subdivision of a microsecond
#define set_stepper_int_period(period_timer_unit)\
     {TIM2->ARR = (uint32_t) (((uint32_t)period_timer_unit > STEPPER_TIMER_MAX_PERIOD) ?\
        STEPPER_TIMER_MAX_PERIOD : (STEPPER_TIMER_TICS_PER_UNIT * (period_timer_unit - (float) 1.0)));};

//Enabling interrupt
#define enable_stepper_interrupt() {TIM2->DIER |= (uint32_t)64;} //1<<6


//Disabling interrupt
#define disable_stepper_interrupt() {TIM2->DIER &= ~(uint32_t)64;} //1<<6


//Enabling timer
#define enable_stepper_timer() {TIM2->CR1 |= (uint32_t)1;}


//Disabling timer
#define disable_stepper_timer() {TIM2->CR1 &= ((uint32_t)((uint16_t)~(uint16_t)1));}


//Flag management
#define stepper_int_flag (TIM2->SR & (uint32_t) 64)
#define stepper_int_flag_clear() {TIM2->SR &= (uint32_t)~(uint32_t)64;};


//Function setting
void set_stepper_int_function(void (*f)());


//Complete setup (long)
void setup_stepper_interrupt(void (*function)(void), uint32_t period_us);


//Complete clean
#define clean_stepper_interrupt() {disable_stepper_interrupt();disable_stepper_timer();}


//---------------------------------------Control_loops_Milliseconds_Timers----------------------------------------------

/*
 * The STM32 L053R8 has 16 bits timers : TIM1 TIM8 TIM3 TIM4 TIM6 TIM7.
 *
 *  All can be configured to use a custom 16-bits prescaler. We will use this prescaller, to count one milisecond,
 *      and set the auto-reload value to the loop period.
 */

//First, we will alias timers used for different loops :
#define L0_TIMER TIM6
#define L1_TIMER TIM7
#define L2_TIMER TIM3
#define L3_TIMER TIM4
#define L4_TIMER TIM15
#define L5_TIMER TIM16
#define L6_TIMER TIM17
#define L7_TIMER TIM1
#define L8_TIMER TIM8

#define F_BUS 48000000

//48000 tics for a millisecond
#define MS_TICS_PER_MS (uint32_t) (F_BUS / 1000)


//The timer counts in milliseconds.
#define MS_TIMER_MAX_PERIOD (uint32_t) (UINT16_MAX)


/*
 * Loops periods settings :
 *  The base unit for the period computation will be the uint32_t.
 */
#define SET_PERIOD_16_BITS(TIMER, period_ms)\
     {TIMER->ARR = ((uint32_t)period_ms > MS_TIMER_MAX_PERIOD) ?\
        MS_TIMER_MAX_PERIOD :  (uint16_t) ((MS_TICS_PER_MS) * (uint16_t)period_ms - 1);};
#define ENABLE_INTERRUPT(TIMER) {TIMER->DIER |= TIM_DIER_TIE;} //1<<6
#define DISABLE_INTERRUPT(TIMER) {TIMER->DIER &= ~TIM_DIER_TIE;} //1<<6
#define ENABLE_TIMER(TIMER) {TIMER->CR1 |= TIM_CR1_CEN;}
#define DISABLE_TIMER(TIMER) {TIMER->CR1 &= ~TIM_CR1_CEN}
#define GET_INTERRUPT_FLAG(TIMER) (TIMER->SR & TIM_SR_TIF)
#define CLEAR_INTERRUPT_FLAG(TIMER) {TIMER->SR &= ~TIM_SR_TIF};


//Set the loop period
#define set_loop_int_period_0(period_ms) SET_PERIOD_16_BITS(L0_TIMER, period_ms)
#define set_loop_int_period_1(period_ms) SET_PERIOD_16_BITS(L1_TIMER, period_ms)
#define set_loop_int_period_2(period_ms) SET_PERIOD_16_BITS(L2_TIMER, period_ms)
#define set_loop_int_period_3(period_ms) SET_PERIOD_16_BITS(L3_TIMER, period_ms)
#define set_loop_int_period_4(period_ms) SET_PERIOD_16_BITS(L4_TIMER, period_ms)
#define set_loop_int_period_5(period_ms) SET_PERIOD_16_BITS(L5_TIMER, period_ms)
#define set_loop_int_period_6(period_ms) SET_PERIOD_16_BITS(L6_TIMER, period_ms)
#define set_loop_int_period_7(period_ms) SET_PERIOD_16_BITS(L7_TIMER, period_ms)
#define set_loop_int_period_8(period_ms) SET_PERIOD_16_BITS(L8_TIMER, period_ms)


//Enabling loop interrupts
#define enable_loop_interrupt_0() ENABLE_INTERRUPT(L0_TIMER)
#define enable_loop_interrupt_1() ENABLE_INTERRUPT(L1_TIMER)
#define enable_loop_interrupt_2() ENABLE_INTERRUPT(L2_TIMER)
#define enable_loop_interrupt_3() ENABLE_INTERRUPT(L3_TIMER)
#define enable_loop_interrupt_4() ENABLE_INTERRUPT(L4_TIMER)
#define enable_loop_interrupt_5() ENABLE_INTERRUPT(L5_TIMER)
#define enable_loop_interrupt_6() ENABLE_INTERRUPT(L6_TIMER)
#define enable_loop_interrupt_7() ENABLE_INTERRUPT(L7_TIMER)
#define enable_loop_interrupt_8() ENABLE_INTERRUPT(L8_TIMER)

//Disabling loops interrupts
#define disable_loop_interrupt_0() DISABLE_INTERRUPT(L0_TIMER)
#define disable_loop_interrupt_1() DISABLE_INTERRUPT(L1_TIMER)
#define disable_loop_interrupt_2() DISABLE_INTERRUPT(L2_TIMER)
#define disable_loop_interrupt_3() DISABLE_INTERRUPT(L3_TIMER)
#define disable_loop_interrupt_4() DISABLE_INTERRUPT(L0_TIMER)
#define disable_loop_interrupt_5() DISABLE_INTERRUPT(L1_TIMER)
#define disable_loop_interrupt_6() DISABLE_INTERRUPT(L2_TIMER)
#define disable_loop_interrupt_7() DISABLE_INTERRUPT(L3_TIMER)
#define disable_loop_interrupt_8() DISABLE_INTERRUPT(L3_TIMER)


//Enabling loop timers
#define enable_loop_timer_0() ENABLE_TIMER(L0_TIMER)
#define enable_loop_timer_1() ENABLE_TIMER(L1_TIMER)
#define enable_loop_timer_2() ENABLE_TIMER(L2_TIMER)
#define enable_loop_timer_3() ENABLE_TIMER(L3_TIMER)
#define enable_loop_timer_4() ENABLE_TIMER(L4_TIMER)
#define enable_loop_timer_5() ENABLE_TIMER(L5_TIMER)
#define enable_loop_timer_6() ENABLE_TIMER(L6_TIMER)
#define enable_loop_timer_7() ENABLE_TIMER(L7_TIMER)
#define enable_loop_timer_8() ENABLE_TIMER(L8_TIMER)


//Disabling loops timers
#define disable_loop_timer_0() DISABLE_TIMER(L0_TIMER)
#define disable_loop_timer_1() DISABLE_TIMER(L1_TIMER)
#define disable_loop_timer_2() DISABLE_TIMER(L2_TIMER)
#define disable_loop_timer_3() DISABLE_TIMER(L3_TIMER)
#define disable_loop_timer_4() DISABLE_TIMER(L4_TIMER)
#define disable_loop_timer_5() DISABLE_TIMER(L5_TIMER)
#define disable_loop_timer_6() DISABLE_TIMER(L6_TIMER)
#define disable_loop_timer_7() DISABLE_TIMER(L7_TIMER)
#define disable_loop_timer_8() DISABLE_TIMER(L8_TIMER)


//Disabling loops timers
#define get_loop_int_flag_0() GET_INTERRUPT_FLAG(L0_TIMER)
#define get_loop_int_flag_1() GET_INTERRUPT_FLAG(L1_TIMER)
#define get_loop_int_flag_2() GET_INTERRUPT_FLAG(L2_TIMER)
#define get_loop_int_flag_3() GET_INTERRUPT_FLAG(L3_TIMER)
#define get_loop_int_flag_4() GET_INTERRUPT_FLAG(L4_TIMER)
#define get_loop_int_flag_5() GET_INTERRUPT_FLAG(L5_TIMER)
#define get_loop_int_flag_6() GET_INTERRUPT_FLAG(L6_TIMER)
#define get_loop_int_flag_7() GET_INTERRUPT_FLAG(L7_TIMER)
#define get_loop_int_flag_8() GET_INTERRUPT_FLAG(L8_TIMER)


//Disabling loops timers
#define clear_loop_int_flag_0() CLEAR_INTERRUPT_FLAG(L0_TIMER)
#define clear_loop_int_flag_1() CLEAR_INTERRUPT_FLAG(L1_TIMER)
#define clear_loop_int_flag_2() CLEAR_INTERRUPT_FLAG(L2_TIMER)
#define clear_loop_int_flag_3() CLEAR_INTERRUPT_FLAG(L3_TIMER)
#define clear_loop_int_flag_4() CLEAR_INTERRUPT_FLAG(L4_TIMER)
#define clear_loop_int_flag_5() CLEAR_INTERRUPT_FLAG(L5_TIMER)
#define clear_loop_int_flag_6() CLEAR_INTERRUPT_FLAG(L6_TIMER)
#define clear_loop_int_flag_7() CLEAR_INTERRUPT_FLAG(L7_TIMER)
#define clear_loop_int_flag_8() CLEAR_INTERRUPT_FLAG(L8_TIMER)


//Function setting
void set_loop_function_0(void (*f)());
void set_loop_function_1(void (*f)());
void set_loop_function_2(void (*f)());
void set_loop_function_3(void (*f)());
void set_loop_function_4(void (*f)());
void set_loop_function_5(void (*f)());
void set_loop_function_6(void (*f)());
void set_loop_function_7(void (*f)());
void set_loop_function_8(void (*f)());


// Complete setups (long)
void setup_loop_interrupt_0(void (*function)(void), uint32_t period_ms);
void setup_loop_interrupt_1(void (*function)(void), uint32_t period_ms);
void setup_loop_interrupt_2(void (*function)(void), uint32_t period_ms);
void setup_loop_interrupt_3(void (*function)(void), uint32_t period_ms);
void setup_loop_interrupt_4(void (*function)(void), uint32_t period_ms);
void setup_loop_interrupt_5(void (*function)(void), uint32_t period_ms);
void setup_loop_interrupt_6(void (*function)(void), uint32_t period_ms);
void setup_loop_interrupt_7(void (*function)(void), uint32_t period_ms);
void setup_loop_interrupt_8(void (*function)(void), uint32_t period_ms);


//Complete cleans : disables timers and interrupts
#define clean_loop_interrupt_0() {disable_loop_timer_0(); disable_loop_interrupt_0();};
#define clean_loop_interrupt_1() {disable_loop_timer_1(); disable_loop_interrupt_1();};
#define clean_loop_interrupt_2() {disable_loop_timer_2(); disable_loop_interrupt_2();};
#define clean_loop_interrupt_3() {disable_loop_timer_3(); disable_loop_interrupt_3();};
#define clean_loop_interrupt_4() {disable_loop_timer_4(); disable_loop_interrupt_4();};
#define clean_loop_interrupt_5() {disable_loop_timer_5(); disable_loop_interrupt_5();};
#define clean_loop_interrupt_6() {disable_loop_timer_6(); disable_loop_interrupt_6();};
#define clean_loop_interrupt_7() {disable_loop_timer_7(); disable_loop_interrupt_7();};
#define clean_loop_interrupt_8() {disable_loop_timer_8(); disable_loop_interrupt_8();};


//----------------------------------------------------Serial------------------------------------------------------------


class serial {
public:
    static inline void begin() {}
    static inline void send_byte(char c) {}
    static inline void send_str(string_t c) {}
    static inline int available() {return 0;}
    static inline char read() {return 1;}
};
#endif //HDWGGABSTRACTION_H
