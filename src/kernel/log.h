//
// Created by root on 9/10/18.
//

#ifndef TRACER_LOG_H
#define TRACER_LOG_H


#include <stdbool.h>

#include <stdint.h>

#include <stddef.h>


#define SELECT(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _N, ...) _N

#define PACKAGE_RIGHT ARGS_PACKAGE_10, ARGS_PACKAGE_9, ARGS_PACKAGE_8, ARGS_PACKAGE_7, ARGS_PACKAGE_6, ARGS_PACKAGE_5, ARGS_PACKAGE_4, ARGS_PACKAGE_3, ARGS_PACKAGE_2, ARGS_PACKAGE_1, ARGS_PACKAGE_0
#define COUNT_RIGHT 10,9,8,7,6,5,4,3,2,1,0,

//const void * cast
#define CVC(x) (const void *)(x)
#define ARGS_PACKAGE_0()
#define ARGS_PACKAGE_1(a) 								{CVC(a)}
#define ARGS_PACKAGE_2(a, b) 							{CVC(a), CVC(b)}
#define ARGS_PACKAGE_3(a, b, c) 						{CVC(a), CVC(b), CVC(c)}
#define ARGS_PACKAGE_4(a, b, c, d) 						{CVC(a), CVC(b), CVC(c), CVC(d)}
#define ARGS_PACKAGE_5(a, b, c, d, e) 					{CVC(a), CVC(b), CVC(c), CVC(d), CVC(e)}
#define ARGS_PACKAGE_6(a, b, c, d, e, f)				{CVC(a), CVC(b), CVC(c), CVC(d), CVC(e), CVC(f)}
#define ARGS_PACKAGE_7(a, b, c, d, e, f, g) 			{CVC(a), CVC(b), CVC(c), CVC(d), CVC(e), CVC(f), CVC(g)}
#define ARGS_PACKAGE_8(a, b, c, d, e, f, g, h) 			{CVC(a), CVC(b), CVC(c), CVC(d), CVC(e), CVC(f), CVC(g), CVC(h)}
#define ARGS_PACKAGE_9(a, b, c, d, e, f, g, h, i)		{CVC(a), CVC(b), CVC(c), CVC(d), CVC(e), CVC(f), CVC(g), CVC(h), CVC(i)}
#define ARGS_PACKAGE_10(a, b, c, d, e, f, g, h, i, j) 	{CVC(a), CVC(b), CVC(c), CVC(d), CVC(e), CVC(f), CVC(g), CVC(h), CVC(i), CVC(j)}

#define S(...) SELECT(__VA_ARGS__)

//Determine the appropriate packaging macro;
#define ARGS_PACKAGE_F(...) 	S(__VA_ARGS__, PACKAGE_RIGHT)

//Determine the number of arguments;
#define COUNT(...) 				S(__VA_ARGS__, COUNT_RIGHT)


//Package a variable number of arguments; Determines the appropriate packager and calls it;
#define ARGS_PACKAGE(...) ARGS_PACKAGE_F(__VA_ARGS__) (__VA_ARGS__)



//Display a message in the log interface;
void _kernel_log(const char *str, const void **args, size_t args_size);


#define kernel_log(msg, ...) {\
		const void *__klog_args__[] = ARGS_PACKAGE(__VA_ARGS__);\
		_kernel_log(msg, __klog_args__, COUNT(__VA_ARGS__));\
	}

#define kernel_log_(msg) _kernel_log(msg, 0, 0);



/*
 * ----------------------------------------------------- Log Debug -----------------------------------------------------
 */


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


#endif //TRACER_LOG_H
