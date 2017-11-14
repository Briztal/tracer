
#ifndef TRACER_INTERFACE_DATA_H
#define TRACER_INTERFACE_DATA_H

#include <stdint.h>
#include <Interfaces/TerminalInterface/TerminalNode.h>

typedef struct terminal_interface_data_t {
    uint8_t arguments_index;
    TerminalNode * node;
} terminal_interface_data_t;

typedef struct program_interface_data_t {
    uint8_t arguments_index;
} program_interface_data_t;

typedef struct gcode_interface_data_t {
    uint8_t arguments_index;
} gcode_interface_data_t;



#endif