//
// Created by root on 8/29/18.
//

#ifndef TRACER_IC_H
#define TRACER_IC_H

#include <stdbool.h>

#include <stdint.h>


//Enable the interrupt control;
void ic_enable_interrupts();

//Disables the interrupt control;
void ic_disable_interrupts();

//Enables the required interrupt channel;
void ic_enable_interrupt(uint16_t channel);

//Disables the required interrupt channel;
void ic_disable_interrupt(uint16_t channel);

//Sets the required non-system interrupt in the pending state;
void ic_set_interrupt_pending(uint16_t channel);

//Sets the required non-system interrupt in the not-pending state;
void ic_clear_interrupt_pending(uint16_t channel);

//Sets the required non-system interrupt in the not-pending state;
void ic_is_interrupt_pending(uint16_t channel);

//Applies the provided priority to the required non-system interupt channel;
void ic_set_interrupt_priority(uint16_t channel, uint8_t priority);

//Returns the priority to the required non-system interupt channel;
uint8_t ic_get_interrupt_priority(uint16_t channel, uint8_t priority);

//Resets the handler of the required channel to 0, isr will return immediately;
void ic_reset_interrupt_handler(uint16_t channel);

//Assert if a handler is currently in execution;
bool ic_in_handler_mode();



#endif //TRACER_IC_H
