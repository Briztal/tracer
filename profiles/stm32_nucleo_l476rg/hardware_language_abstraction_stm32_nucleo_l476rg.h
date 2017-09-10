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
 * The Board Abstraction file.
 *
 * This file is made for : stm32 nucleo l476rg.
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


//----------------------------------------------------EEPROM------------------------------------------------------------

#define EEPROM_read(i) 0

#define EEPROM_write(i, byte)


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

//For the stepper interrupt, the TIM2 (32 bis is used)

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


//Period macros


#define MS_TICS_PER_MS (uint32_t) (F_BUS / 1000)

#define MS_TIMER_MAX_PERIOD (uint32_t) (UINT32_MAX / MS_TICS_PER_MS)


//Loops periods settings

#define set_loop_int_period_0(period_ms)\
     {PIT_LDVAL1 = ((uint32_t)period_ms > MS_TIMER_MAX_PERIOD) ?\
        MS_TIMER_MAX_PERIOD :  (uint32_t) ((MS_TICS_PER_MS) * (uint32_t)period_ms - 1);};

#define set_loop_int_period_1(period_ms)\
     {PIT_LDVAL2 = ((uint32_t)period_ms > MS_TIMER_MAX_PERIOD) ?\
        MS_TIMER_MAX_PERIOD :  (uint32_t) ((MS_TICS_PER_MS) * (uint32_t)period_ms - 1);};

#define set_loop_int_period_2(period_ms)\
     {PIT_LDVAL3 = ((uint32_t)period_ms > MS_TIMER_MAX_PERIOD) ?\
        MS_TIMER_MAX_PERIOD :  (uint32_t) ((MS_TICS_PER_MS) * (uint32_t)period_ms - 1);};


//Enabling loop interrupts

#define enable_loop_interrupt_0()

#define enable_loop_interrupt_1()

#define enable_loop_interrupt_2()


//Disabling loops interrupts

#define disable_loop_interrupt_0()

#define disable_loop_interrupt_1()

#define disable_loop_interrupt_2()


//Enabling loop timers

#define enable_loop_timer_0()

#define enable_loop_timer_1()

#define enable_loop_timer_2()


//Disabling loop timers

#define disable_loop_timer_0()

#define disable_loop_timer_1()

#define disable_loop_timer_2()


//Function setting

void set_loop_function_0(void (*f)());

void set_loop_function_1(void (*f)());

void set_loop_function_2(void (*f)());


// Complete setups (long)

void setup_loop_interrupt_0(void (*function)(void), uint32_t period_ms);

void setup_loop_interrupt_1(void (*function)(void), uint32_t period_ms);

void setup_loop_interrupt_2(void (*function)(void), uint32_t period_ms);


//Complete cleans : disables timers and interrupts

#define clean_loop_interrupt_0() {disable_loop_timer_0(); disable_loop_interrupt_0();};

#define clean_loop_interrupt_1() {disable_loop_timer_1(); disable_loop_interrupt_1();};

#define clean_loop_interrupt_2() {disable_loop_timer_2(); disable_loop_interrupt_2();};


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
