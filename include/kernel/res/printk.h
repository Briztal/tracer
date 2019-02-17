//
// Created by root on 9/10/18.
//

#ifndef TRACER_LOG_H
#define TRACER_LOG_H


#include <stdbool.h>

#include <stdint.h>

#include <stddef.h>

#include <variadic.h>

#include <stdio.h>


//----------------------------------------------------- Kernel log -----------------------------------------------------

//Print a string in the kernel log file;
//void printk(const char *str);
#define printk(str) _printkf(str, 0, 0);

#define printkf(msg, ...) {\
		const void * __klog_args__ [] = VARIADIC_PACKAGE(__VA_ARGS__);\
		_printkf(msg, __klog_args__, VARIADIC_COUNT(__VA_ARGS__));\
	}

//Display a message in the log if;
void _printkf(const char * str, const void ** args,  size_t args_size);


#endif //TRACER_LOG_H
