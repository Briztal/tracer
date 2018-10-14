//
// Created by root on 9/10/18.
//

#ifndef TRACER_LOG_H
#define TRACER_LOG_H


#include <stdbool.h>

#include <stdint.h>

#include <stddef.h>

#include <util/macro/variadic.h>

//------------------------------------------------------- Config -------------------------------------------------------

//Uncomment this to disable log.
//#define DISABLE_LOG

//----------------------------------------------------- Kernel log -----------------------------------------------------

//Display a message in the log interface;
void _kernel_log(const char * str, const void ** args,  size_t args_size);


//------------------------------------------------------- Macros -------------------------------------------------------

//If logs are enabled :
#ifndef DISABLE_LOG


#define kernel_log(msg, ...) {\
		const void * __klog_args__ [] = VARIADIC_PACKAGE(__VA_ARGS__);\
		_kernel_log(msg, __klog_args__, VARIADIC_COUNT(__VA_ARGS__));\
	}

#define kernel_log_(msg) _kernel_log(msg, 0, 0);


//If logs are disabled :
#else

#define kernel_log(msg, ...)

#define kernel_log_(msg)

#endif


/*
 * ----------------------------------------------------- Log Debug -----------------------------------------------------
 */


//TODO REMOVE
/*
//Send a string over the debug interface, encoded by the log protocol. Implemented by the log protocol;
void debug_log(const char *msg);

//Convert an integer to its string expression in the required base, and send it;
void debug_log_base(uint32_t integer, uint8_t base);

//Convert an integer to its binary string expression and send it;
#define debug_log_bin(i) {debug_log_base(i, 2);}

//Convert an integer to its octal string expression and send it;
#define debug_log_oct(i) {debug_log_base(i, 8);}

//Convert an integer to its decimal string expression and send it;
#define debug_log_int(i) {debug_log_base(i, 10);}

//Convert an integer to its hexadecimal string expression and send it;
#define debug_log_hex(i) {debug_log_base(i, 16);}
*/

#endif //TRACER_LOG_H
