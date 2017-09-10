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
 * This file is made for : teensy35.
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
#include "Arduino.h"
#include "EEPROM.h"

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


//------------------------------------------------INITIALISATION--------------------------------------------------------

void hl_begin();

//----------------------------------------------------STRING------------------------------------------------------------

#define string_t String

#define str String

//----------------------------------------------------EEPROM------------------------------------------------------------

#define EEPROM_read(i) EEPROM.read(i)

#define EEPROM_write(i, byte) EEPROM.write(i, byte)

//-----------------------------------------------------IO--------------------------------------------------------------

#define digital_write(i, v) digitalWriteFast(i, v)

#define digital_read(i) digitalReadFast(i)

#define analog_read(i) analoglRead(i)

#define analog_write(i, v) analogWrite(i, v)

#define pin_mode(i, mode) pinMode(i, mode)


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
static inline static void delay_ms(uint16_t time_ms){

}*/
#define delay_ms(ms) delay(ms)


/* The function to call wait for a specified number of microseconds
 * void delay_us(uint16_t time_us);
 */
/*static inline static void delay_us(uint16_t time_us){

}*/
#define delay_us(us) delayMicroseconds(us)

//--------------------------------------StepperTimer_Interrupt----------------------------------------------------

//The frequency of the timer :
#define STEPPER_TIMER_FREQUENCY (float) 1000000.0

#define STEPPER_TIMER_TICS_PER_UNIT ((float) ((float) F_BUS / STEPPER_TIMER_FREQUENCY ))

#define STEPPER_TIMER_MAX_PERIOD ((float) (UINT32_MAX / STEPPER_TIMER_TICS_PER_UNIT))

//Period setting : WARNING, the period is expressed into timer unit, a subdivision of a microsecond
#define set_stepper_int_period(period_timer_unit)\
     {PIT_LDVAL0 = (uint32_t) (((uint32_t)period_timer_unit > STEPPER_TIMER_MAX_PERIOD) ?\
        STEPPER_TIMER_MAX_PERIOD : (STEPPER_TIMER_TICS_PER_UNIT * (period_timer_unit - (float) 1.0)));};

//Enabling interrupt
#define enable_stepper_interrupt() PIT_TCTRL0 |= PIT_TCTRL_TIE;


//Disabling interrupt
#define disable_stepper_interrupt() PIT_TCTRL0 &= 5;


//Enabling timer
#define enable_stepper_timer() PIT_TCTRL0 |= PIT_TCTRL_TEN;


//Disabling timer
#define disable_stepper_timer() PIT_TCTRL0 &= 6;


//Flag management
#define stepper_int_flag PIT_TFLG0
#define stepper_int_flag_clear() PIT_TFLG0 = PIT_TFLG_TIF;


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

#define enable_loop_interrupt_0() PIT_TCTRL1 |= PIT_TCTRL_TIE;

#define enable_loop_interrupt_1() PIT_TCTRL2 |= PIT_TCTRL_TIE;

#define enable_loop_interrupt_2() PIT_TCTRL3 |= PIT_TCTRL_TIE;


//Disabling loops interrupts

#define disable_loop_interrupt_0() PIT_TCTRL1 &= 5;

#define disable_loop_interrupt_1() PIT_TCTRL2 &= 5;

#define disable_loop_interrupt_2() PIT_TCTRL3 &= 5;


//Enabling loop timers

#define enable_loop_timer_0() PIT_TCTRL1 |= PIT_TCTRL_TEN;

#define enable_loop_timer_1() PIT_TCTRL2 |= PIT_TCTRL_TEN;

#define enable_loop_timer_2() PIT_TCTRL3 |= PIT_TCTRL_TEN;


//Disabling loop timers

#define disable_loop_timer_0() PIT_TCTRL1 &= 6;

#define disable_loop_timer_1() PIT_TCTRL2 &= 6;

#define disable_loop_timer_2() PIT_TCTRL3 &= 6;


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


//------------------------------------------------PHYSICAL LINKS--------------------------------------------------------

class serial {
public:
    static inline void begin() {Serial.begin(115200);}
    static inline void send_byte(char c) {Serial.print(c);}
    static inline void send_str(string_t c) {Serial.print(c);}
    static inline int available() {return Serial.available();}
    static inline char read() {return (char)Serial.read();}
};


class serial1 {
public:
    static inline void begin() {Serial1.begin(115200);}
    static inline void send_byte(char c) {Serial1.print(c);}
    static inline void send_str(string_t c) {Serial1.print(c);}
    static inline int available() {return Serial1.available();}
    static inline char read() {return (char)Serial1.read();}
};



class serial2 {
public:
    static inline void begin() {Serial2.begin(115200);}
    static inline void send_byte(char c) {Serial2.print(c);}
    static inline void send_str(string_t c) {Serial2.print(c);}
    static inline int available() {return Serial2.available();}
    static inline char read() {return (char)Serial2.read();}
};



class serial3 {
public:
    static inline void begin() {Serial3.begin(115200);}
    static inline void send_byte(char c) {Serial3.print(c);}
    static inline void send_str(string_t c) {Serial3.print(c);}
    static inline int available() {return Serial3.available();}
    static inline char read() {return (char)Serial3.read();}
};


#endif //HDWGGABSTRACTION_H
