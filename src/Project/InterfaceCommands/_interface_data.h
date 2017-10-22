
#ifndef TRACER_INTERFACE_DATA_H
#define TRACER_INTERFACE_DATA_H

#include <stdint.h>
#include <DataStructures/Node.h>

typedef struct terminal_interface_data_t {
    uint8_t arguments_index;
    Node * node;
} interface_data_t;


#endif