
#ifndef TRACER_ENDSTOPS_H
#define TRACER_ENDSTOPS_H

#include "stdint.h"

class Endstops {

public:

    //Initialise all GPIO in read_mode;
    static void initialise_hardware();

    //Initialise the states array;
    static void initialise_data();

    //Read the state of a particular endstop;
    static bool read_endstop_state(uint8_t index);

    //Copy all endstops states into the states array;
    static void read_endstops(uint8_t index);

    //Get a previously_copied state;
    static bool get_endstop_state(uint8_t index);

private:

    //The states array;
    static bool *const states;

};


#endif //TRACER_ENDSTOPS_H
