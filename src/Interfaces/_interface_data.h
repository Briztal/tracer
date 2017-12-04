
#ifndef TRACER_INTERFACE_DATA_H
#define TRACER_INTERFACE_DATA_H

#include <stdint.h>
#include <Interfaces/TerminalInterface/TerminalNode.h>

/*
 * The interface data : this structure contains all required data to execute an Interface function, wheather it
 *  may come from TerminalInterface, GCodeInterface, ProgramInterface, or any interface,
 *      as Interfaces share a same structure.
 *
 */
struct interface_data_t {

    /*
     * function : the function to execute. This function could have been directly scheduled,
     *  but the cast from (void *) to argument structure would have been made in the function, making it a little bit
     *  complex. Instead, this cast is made by a function in the Interface class, and the (char *) is given to this
     *  function.
     */

    task_state_t (*function)(char *);


    /*
     * argument_index : the index of the argument sequence in the argument_t storage.
     */

    uint8_t arguments_index;

};


/*
 *  A simple struct that will be used in the arguments parsing for GCode and Terminal Interfaces.
 */

typedef struct argument_t {
    //The identfier of an argument
    char identifier;

    //A pointer to the argument's value (string)
    char *arg;
};


#endif