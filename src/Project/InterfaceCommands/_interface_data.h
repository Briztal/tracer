
#ifndef TRACER_INTERFACE_DATA_H
#define TRACER_INTERFACE_DATA_H

#include <stdint.h>
#include <Interfaces/TerminalInterface/TerminalNode.h>

/*
 * The terminal interface data : this structure contains all required data to execute a TerminalCommand function.
 *
 */
typedef struct terminal_interface_data_t {

    /*
     * terminal_command : the function to execute. This function could have been directly scheduled,
     *  but the cast from (void *) to argument structure would have been made in the function, making it a little bit
     *  complex. Instead, this cast is made by a function in TerminalInterface, and the (char *) is given to this
     *  function.
     */

    task_state_t (*terminal_command)(char *);


    /*
     * argument_index : the index of the argument sequence in the argument storage.
     */

    uint8_t arguments_index;

} terminal_interface_data_t;




//TODO COMMENT WHEN THOSE INTERFACES WILL BE REBUILT

typedef struct program_interface_data_t {
    uint8_t arguments_index;
} program_interface_data_t;

typedef struct gcode_interface_data_t {
    uint8_t arguments_index;
} gcode_interface_data_t;



#endif