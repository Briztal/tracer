//
// Created by root on 4/10/18.
//

#ifndef TRACER_SYSTICK_H
#define TRACER_SYSTICK_H


#include <stdint.h>

#include <stdbool.h>

/*
 * The systick is the reference clock for software;
 *
 * The library receives ticks, supposedly every millisecond, and updates its internal counter;
 *
 * Every time its receives a systick, it verifies if it must trigger the context switch, and does it if so;
 */

    //---------------------- System tick ----------------------

    //Receive a system systick;
    void systick_tick();


    //---------------------- Context Switch ----------------------

    //Update the current task's duration;
    void systick_set_process_duration(uint16_t ms);


    //---------------------- Time reference ----------------------

    //Get the millisecond reference;
    uint32_t systick_milliseconds();

    //Wait till time has reached the given limit;
    void systick_sleep(uint32_t delta_t);

    //The systick_milliseconds reset : returns true if millis must be reset to 0 all across the code, to avoid an overflow;
    bool systick_time_reset_required();

    //Reset the ms counter;
    void systick_reset_time();


#endif //TRACER_SYSTICK_H
