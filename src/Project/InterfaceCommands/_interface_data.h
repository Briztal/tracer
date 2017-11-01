
#ifndef TRACER_INTERFACE_DATA_H
#define TRACER_INTERFACE_DATA_H

#include <stdint.h>
#include <Interfaces/TerminalInterface/TerminalNode.h>

typedef struct terminal_interface_data_t {
    uint8_t arguments_index;
    TerminalNode * node;
} interface_data_t;


#endif