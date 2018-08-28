//
// Created by root on 8/28/18.
//

#ifndef TRACER_CORE_H
#define TRACER_CORE_H

#include <stdint.h>

//#define ENABLE_CORE_ERRORS


/*
 * Communication;
 */


//Light the debug led high;
extern void core_led_high();

//Turn off the debug led;
extern void core_led_low();

//Wait a certain number of milliseconds;
extern void core_delay_ms(uint32_t ms);

//Wait a certain number of milliseconds;
extern void core_delay_us(uint32_t us);



/*
 * Log;
 */

//Send a log message;
void core_log(const char *log);



/*
 * Error;
 */

//Halt and send a message continuously;
void _core_error(const char *msg);


/*
 * If core errors are enabled, link core_error to _core_error. If not, link it to nothing, strings will not be
 * 	generated in .rodata;
 */

#ifdef ENABLE_CORE_ERRORS
#define core_error(s) _core_error(s);
#else
#define core_error(s)
#endif


#endif //TRACER_CORE_H
