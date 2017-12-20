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
 * Here are defined all function related to the hardware (serial, I2C, delay_us etc...), and toolchain
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

#define USB_HID

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

#define HL_EEPROM_FLAG



//----------------------------------------------------STRING------------------------------------------------------------

#define string_t String

#define str(t) String(t)

#define str_to_float(s) strtof(s, NULL);


//----------------------------------------------------EEPROM------------------------------------------------------------

#define EEPROM_read(i) EEPROM.read(i)

#define EEPROM_write(i, byte) EEPROM.write(i, byte)

//-----------------------------------------------------IO--------------------------------------------------------------

#define digital_write(i, v) digitalWriteFast(i, v)

#define digital_read(i) digitalReadFast(i)

#define analog_read(i) analogRead(i)

#define analog_write(i, v) analogWrite(i, v)

#define pin_mode_input(i) pinMode(i, INPUT);

#define pin_mode_output(i) pinMode(i, OUTPUT);


//-----------------------------------------------------MATH-------------------------------------------------------------


#define sqrt_float(f) sqrtf(f)
#define sqrt_long(l) sqrtf(l)

#define minimum(a, b) (((a)<(b)) ? (a) : (b))
#define maximum(a, b) (((a)<(b)) ? (b) : (a))


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


//--------------------------------------- Timers ----------------------------------------------

#define TICS_PER_UNIT(timer_index) ((float) ((float) F_BUS / (TIMER_##timer_index##_FREQUENCY) ))

#define MAX_UNIT(timer_index) ((float) (UINT32_MAX / TICS_PER_UNIT(timer_index)))

#include "Arduino.h"
#define UNIT_TO_TICS(unit_period, timer_index) ((uint32_t) (((uint32_t) (unit_period) > MAX_UNIT(timer_index)) ? UINT32_MAX - 1 : (TICS_PER_UNIT(timer_index) * (float) (unit_period)  - (float) 1.0)));

#define SET_INTERRUPT_RELOAD(reload, timer_index) {PIT_LDVAL##timer_index = reload;};

// /Period setting : WARNING, the period is expressed into timer unit, a subdivision of a microsecond
#define SET_INTERRUPT_PERIOD(period_timer_unit, timer_index) {SET_INTERRUPT_RELOAD(UNIT_TO_TICS(period_timer_unit, timer_index), timer_index);};


#define ENABLE_INTERRUPT(timer_index) PIT_TCTRL##timer_index |= PIT_TCTRL_TIE;
#define IS_LOOP_ENABLED(timer_index)  ((PIT_TCTRL##timer_index & PIT_TCTRL_TIE)&(PIT_TCTRL##timer_index & PIT_TCTRL_TEN))
#define DISABLE_INTERRUPT(timer_index) PIT_TCTRL##timer_index &= 5;
#define ENABLE_TIMER(timer_index) PIT_TCTRL##timer_index |= PIT_TCTRL_TEN;
#define DISABLE_TIMER(timer_index) PIT_TCTRL##timer_index &= 6;
#define TIMER_FLAG_GET(timer_index) PIT_TFLG##timer_index
#define TIMER_FLAG_RESET(timer_index) PIT_TFLG##timer_index = PIT_TFLG_TIF;
#define CLEAN_INTERRUPT(timer_index) {disable_timer_##timer_index(); disable_interrupt_##timer_index();};


//Define the period setters for all 4 timers;
#define set_int_period_0(period) SET_INTERRUPT_PERIOD(period, 0)
#define set_int_period_1(period) SET_INTERRUPT_PERIOD(period, 1)
#define set_int_period_2(period) SET_INTERRUPT_PERIOD(period, 2)
#define set_int_period_3(period) SET_INTERRUPT_PERIOD(period, 3)

//Define the reload converters for all 4 timers;
#define period_to_reload_0(period) UNIT_TO_TICS(period, 0)
#define period_to_reload_1(period) UNIT_TO_TICS(period, 1)
#define period_to_reload_2(period) UNIT_TO_TICS(period, 2)
#define period_to_reload_3(period) UNIT_TO_TICS(period, 3)

//Define the reload setters for all 4 timers;
#define set_int_reload_0(reload) SET_INTERRUPT_RELOAD(reload, 0)
#define set_int_reload_1(reload) SET_INTERRUPT_RELOAD(reload, 1)
#define set_int_reload_2(reload) SET_INTERRUPT_RELOAD(reload, 2)
#define set_int_reload_3(reload) SET_INTERRUPT_RELOAD(reload, 3)

//Enabling loop interrupts
#define enable_interrupt_0() ENABLE_INTERRUPT(0)
#define enable_interrupt_1() ENABLE_INTERRUPT(1)
#define enable_interrupt_2() ENABLE_INTERRUPT(2)
#define enable_interrupt_3() ENABLE_INTERRUPT(3)


//Disabling loops interrupts
#define disable_interrupt_0() DISABLE_INTERRUPT(0)
#define disable_interrupt_1() DISABLE_INTERRUPT(1)
#define disable_interrupt_2() DISABLE_INTERRUPT(2)
#define disable_interrupt_3() DISABLE_INTERRUPT(3)


//Enabling loop timers
#define enable_timer_0() ENABLE_TIMER(0)
#define enable_timer_1() ENABLE_TIMER(1)
#define enable_timer_2() ENABLE_TIMER(2)
#define enable_timer_3() ENABLE_TIMER(3)


//Disabling loop timers
#define disable_timer_0() DISABLE_TIMER(0)
#define disable_timer_1() DISABLE_TIMER(1)
#define disable_timer_2() DISABLE_TIMER(2)
#define disable_timer_3() DISABLE_TIMER(3)


//Controlling if loop is enabled
#define is_timer_loop_enabled_0() ENABLE_TIMER(0)
#define is_timer_loop_enabled_1() ENABLE_TIMER(1)
#define is_timer_loop_enabled_2() ENABLE_TIMER(2)
#define is_timer_loop_enabled_3() ENABLE_TIMER(3)


//get flags
#define timer_flag_0() TIMER_FLAG_GET(0)
#define timer_flag_1() TIMER_FLAG_GET(1)
#define timer_flag_2() TIMER_FLAG_GET(2)
#define timer_flag_3() TIMER_FLAG_GET(3)


//get flags
#define reset_timer_flag_0() TIMER_FLAG_RESET(0)
#define reset_timer_flag_1() TIMER_FLAG_RESET(1)
#define reset_timer_flag_2() TIMER_FLAG_RESET(2)
#define reset_timer_flag_3() TIMER_FLAG_RESET(3)


//Function setting
void set_interrupt_function_0(void (*f)());

void set_interrupt_function_1(void (*f)());

void set_interrupt_function_2(void (*f)());

void set_interrupt_function_3(void (*f)());


// Complete setups (long)
void setup_interrupt_0(void (*function)(void), uint32_t period_ms);

void setup_interrupt_1(void (*function)(void), uint32_t period_ms);

void setup_interrupt_2(void (*function)(void), uint32_t period_ms);

void setup_interrupt_3(void (*function)(void), uint32_t period_ms);


//Complete cleans : disables timers and interrupts
#define clean_interrupt_0() CLEAN_INTERRUPT(0)
#define clean_interrupt_1() CLEAN_INTERRUPT(1)
#define clean_interrupt_2() CLEAN_INTERRUPT(2)
#define clean_interrupt_3() CLEAN_INTERRUPT(3)


//------------------------------------------------PHYSICAL LINKS--------------------------------------------------------

//As we will create an adapter for serials, we will use a template, to simplify the code.
#define SERIAL_CLASS_ADAPTER(name, serial)\
class name {\
public:\
    static inline void begin() {(serial).begin(115200);}\
    static inline uint16_t available() {return (uint16_t)(serial).available();}\
    static inline void write(char c) {(serial).print(c);}\
    static inline char read() {return (char)(serial).read();}\
    static inline void send_str(string_t c) {(serial).print(c);}\
};

/*
 * We define here 4 serial adapters : the usb serial and serial 1, 2 and 3.
 */


SERIAL_CLASS_ADAPTER(usb_serial, Serial)

SERIAL_CLASS_ADAPTER(serial1, Serial1)

SERIAL_CLASS_ADAPTER(serial2, Serial2)

SERIAL_CLASS_ADAPTER(serial3, Serial3)


#endif //HDWGGABSTRACTION_H
