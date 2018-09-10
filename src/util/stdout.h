//
// Created by root on 9/10/18.
//

#ifndef TRACER_TMP_OUT_H
#define TRACER_TMP_OUT_H

#include <stdbool.h>

#include <stdint.h>

#include <stddef.h>


void stdout(void (*print)(char), const char *str, const uint32_t *args, size_t args_size);


#endif //TRACER_TMP_OUT_H
