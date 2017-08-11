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
 * This file is made for : Arduino Mega - atmega2560.
 *
 * Here are defined all function related to the hardware (serial, I2C, delay etc...)
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

//------------------------------------------------INITIALISATION--------------------------------------------------------

void hl_begin();

//----------------------------------------------------EEPROM------------------------------------------------------------

#define EEPROM_read(i) EEPROM.read(i)

#define EEPROM_write(i, byte) EEPROM.write(i, byte)

//-----------------------------------------------------IO--------------------------------------------------------------

#define digital_write(i, v) digitalWriteFast(i, v)

#define digital_read(i) digitalReadFast(i)

#define analog_read(i) analoglRead(i)

#define analog_write(i, v) analogWrite(i, v)

#define pin_mode(i, mode) pinMode(i, mode)


//-----------------------------------------------------IO--------------------------------------------------------------

/*
 * An alias for external interrupt attachment and detachment
 */
#define attach_interrupt(pin, port, mode) attachInterrupt(pin, port, mode)

#define detach_interrupt(pin) detachInterrupt(pin);

//----------------------------------------------------Delay-------------------------------------------------------------

//Uncomment this line to provide delay support across LowLevel
#define HL_DELAY


#ifdef HL_DELAY
/* The function to call to wait for a specified number of milliseconds
 *  void delay_ms(uint16_t time_ms);
 *//*
inline static void delay_ms(uint16_t time_ms){

}*/
#define delay_ms(ms) delay(ms)


/* The function to call wait for a specified number of microseconds
 * void delay_us(uint16_t time_us);
 */
/*inline static void delay_us(uint16_t time_us){

}*/
#define delay_us(us) delayMicroseconds(us)

#endif

//--------------------------------------StepperTimer_Interrupt----------------------------------------------------

//Uncomment this line to provide a microseconds resolution timer.
#define HL_STEPPER_TIMER

#ifdef HL_STEPPER_TIMER


//Period Macros

#define US_TICS_PER_MS (uint32_t) (F_BUS / 1000000)

#define US_TIMER_MAX_PERIOD (uint32_t) (UINT32_MAX / US_TICS_PER_MS)


//Period setting

#define set_stepper_int_period(period_us)\
     {PIT_LDVAL0 = ((uint32_t)period_us > US_TIMER_MAX_PERIOD) ?\
        US_TIMER_MAX_PERIOD :  (uint32_t) ((US_TICS_PER_MS) * (uint32_t)period_us - 1);};


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

void setup_stepper_interrupt(void (*function)(void), uint16_t period_us);


//Complete clean

#define clean_stepper_interrupt() {disable_stepper_interrupt();disable_stepper_timer();}

#endif

//---------------------------------------Control_loops_Milliseconds_Timers----------------------------------------------


//Uncomment this line to provide Milliseconds timers for control loops
#define HL_CONTROL_LOOP_TIMERS

#ifdef HL_CONTROL_LOOP_TIMERS


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

#endif


//----------------------------------------------------Serial------------------------------------------------------------

//Uncomment this line to provide Serial communication across the code
#define HL_SERIAL


#ifdef HL_SERIAL

/* The function to call to initialise the serial at a given baudrate
 * void serial_begin(baudrate b);
 */
/*inline static void serial_begin(baudrate b){

}*/
#define serial_begin(b) Serial.begin(b)

/* The function to call to send_packet a byte on the serial
 * void serial_send_byte(char c);
 */
/*inline static void serial_echo_byte(char c){

}*/
#define serial_echo_byte(c) Serial.print(c)

/* The function to call to send_packet a zero terminated (char *) on the serial
 * void serial_echo_str(char *s);
 */
/*inline static void serial_echo_str(char *s){

}*/
#define serial_echo_str Serial.print


/* The function to call to check if bytes are elements in the serial buffer
 * bool serial_available(void)
 */
/*inline static bool serial_available(void){

}*/
#define serial_available() Serial.available()


/* The function to call to read a byte from serial buffer
 * char serial_read(void)
 */
/*inline static char serial_read(){

}*/
#define serial_read() Serial.read()

#endif

#endif //HDWGGABSTRACTION_H
