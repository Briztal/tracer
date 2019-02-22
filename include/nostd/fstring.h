
#ifndef TRACER_STDIO_H
#define TRACER_STDIO_H

#include <stddef.h>


void fdecode(void (*exec)(void *arg, const char *block, size_t size), void *arg, const char *str, const void **args, size_t args_size);


#endif /*TRACER_STDIO_H*/
