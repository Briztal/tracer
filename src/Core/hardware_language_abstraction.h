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
  along with TRACER.  If not, see <http://www.gnu.org/licenses/>.

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
 * If you must implement functionalities over the base hardware functions, use an inline static function.
 *
 * If you only must alias a function, use a macro, it's faster.
 */

//---------------------------------------------------Includes-----------------------------------------------------------

/*
 * Include here all libraries related to the board itself
 */

#include "Arduino.h"
#include <setjmp.h>


//----------------------------------------------------String------------------------------------------------------------
//Uncomment this line to provide String support across LowLevel
#define HL_STRING

/* The string class name
 * It can vary from a board to another, as minimalist boards like Arduino do not have access to standard library
 */
#define str String

/* The string conversion function name
 * It can vary from a board to another for the same reasons than upper
 */
#define to_str String

//----------------------------------------------------Delay-------------------------------------------------------------

//Uncomment this line to provide delay support across LowLevel
#define HL_DELAY

/* The function to call wait for a specified number of milliseconds
 *  void delay_ms(void);
 *//*
inline static void delay_ms(){

}*/
#define delay_ms delay


/* The function to call wait for a specified number of microseconds
 * void delay_us(void);
 */
/*inline static void delay_us(){

}*/
#define delay_us delayMicroseconds


//--------------------------------------StepperTimer_Interrupt----------------------------------------------------

//Uncomment this line to provide a microseconds resolution timer.
#define HL_TIMER_INTERRUPT_US


#define set_stepper_int_period(period_us) OCR5A = period_us<<1;

#define enable_stepper_interrupt TIMSK5 = (1 << OCIE5A);  // enable timer compare interrupt

#define disable_stepper_interrupt TIMSK5 = 0;

#define enable_stepper_timer PRR1 &= B11011111;

#define disable_stepper_timer PRR1 |= B00100000;

void set_stepper_int_function(void (*f)());

void en_timer_int_us(void (*function)(void), unsigned int period_us);

//--------------------------------------Milliseconds_Timer_Interrupt----------------------------------------------------


//Uncomment one of those lines to provide the concerned timer
#define HL_TIMER_INTERRUPT_0
#define HL_TIMER_INTERRUPT_1


/* The function to enable a timer interrupt, calling the given function every given period
 * void en_timer_int_i(void (*function)(void), int period_ms);
 */

//TODO CALCULER LES LIMITES D'ATTENTES DES TIMERS

void en_timer_int_0(void (*function)(void), unsigned int period_ms);

void en_timer_int_1(void (*function)(void), unsigned int period_ms);



/* The function to disable a timer interrupt
 * void dis_timer_int_i();
 */
inline static void dis_timer_int_0() {
    cli();TIMSK3 = 0;sei();

}

inline static void dis_timer_int_1(){
    cli();TIMSK4 = 0;sei();
}



//----------------------------------------------------Serial------------------------------------------------------------

//Uncomment this line to provide Serial communication across the code
#define HL_SERIAL

/* The function to call to initialise the serial at a given baudrate
 * void serial_begin(baudrate b);
 */
/*inline static void serial_begin(baudrate b){

}*/
#define serial_begin Serial.begin

/* The function to call to send_packet a byte on the serial
 * void serial_send_byte(char c);
 */
/*inline static void serial_echo_byte(char c){

}*/
#define serial_echo_byte Serial.print

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
#define serial_available Serial.available


/* The function to call to read a byte from serial buffer
 * char serial_read(void)
 */
/*inline static char serial_read(){

}*/
#define serial_read Serial.read


#endif //HDWGGABSTRACTION_H
