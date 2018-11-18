//
// Created by root on 9/10/18.
//

#ifndef TRACER_LOG_H
#define TRACER_LOG_H


#include <stdbool.h>

#include <stdint.h>

#include <stddef.h>

#include <macro/variadic.h>

#include <stdio.h>

//----------------------------------------------------- Kernel log -----------------------------------------------------

//Print a string in the kernel log file;
//void printk(const char *str);
#define printk(str) _printkf(str, 0, 0);

//Display a message in the log if;
void _printkf(const char * str, const void ** args,  size_t args_size);

//Print a string in the kernel log file;
//void fprintk(FILE *file, const char *str);
#define fprintk(file, str) _printkf(str, 0, 0);

//Display a message in the log if;
//void _fprintkf(FILE *file, const char * str, const void ** args,  size_t args_size);


//------------------------------------------------------- Macros -------------------------------------------------------


#define printkf(msg, ...) {\
		const void * __klog_args__ [] = VARIADIC_PACKAGE(__VA_ARGS__);\
		_printkf(msg, __klog_args__, VARIADIC_COUNT(__VA_ARGS__));\
	}


#define fprintkf(file, msg, ...) {\
		const void * __klog_args__ [] = VARIADIC_PACKAGE(__VA_ARGS__);\
		_printkf(msg, __klog_args__, VARIADIC_COUNT(__VA_ARGS__));\
	}


#endif //TRACER_LOG_H
