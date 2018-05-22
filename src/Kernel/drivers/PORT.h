//
// Created by root on 5/21/18.
//

#ifndef TRACER_PORT_H
#define TRACER_PORT_H

/*
 * The port module manages the pins behaviour.
 *
 *  It manages :
 *  - Interrupts and DMA modes for pins interrupts;
 *  - Pin multiplexing;
 *  - Hardware configuration for both data directions;
 */

#include "stdint.h"

#include "Kernel/arch/arch_types.h"

/*
 * Types of interruptions;
 */

typedef enum {

    //No interruption enabled;
    PORT_NO_INTERRUPT,

    //DMA request on rising edge;
    PORT_DMA_RISING_EDGE,

    //DMA request on falling edge;
    PORT_DMA_FALLING_EDGE,

    //DMA request on rising or falling edge;
    PORT_DMA_EDGE,

    //Interrupt request when logical 0;
    PORT_INTERRUPT_0,

    //Interrupt request when logical 1;
    PORT_INTERRUPT_1,

    //Interrupt request on rising edge;
    PORT_INTERRUPT_RISING_EDGE,

    //Interrupt request on falling edge;
    PORT_INTERRUPT_FALLING_EDGE,

    //Interrupt request on rising or falling edge;
    PORT_INTERRUPT_EDGE,

} PORT_interrupt_t;


/*
 * The hardware data direction;
 */

typedef enum {

    //Pin is used to collect data;
    PORT_INPUT,

    //Pin is used to provide data;
    PORT_OUTPUT

} PORT_pin_direction_t;


/*
 * Different input hardware configurations;
 */

typedef enum {

    //High impedance configuration;
    PORT_HIGH_IMPEDANCE,

    //Pull-up configuration;
    PORT_PULL_UP,

    //Pull-down configuration;
    PORT_PULL_DOWN,

    //Hysteresis configuration;
    PORT_HYSTERSIS,

} PORT_input_mode_t;


/*
 * Different hardware output configurations;
 */

typedef enum {

    //Open drain configuration;
    PORT_OPEN_DRAIN,

    //Push-pull configuration;
    PORT_PUSH_PULL,

    //High current drive configuration;
    PORT_HIGH_DRIVE,

} PORT_output_mode_t;


/*
 * Different slew rates;
 */

typedef enum {

    //Low slew rate;
    PORT_LOW_RATE,

    //High slew rate;
    PORT_HIGH_RATE,

} PORT_slew_rate_t;


/*
 * The complete configuration for a pin;
 */

typedef struct {

    /*
     * Interruption;
     */

    //The type of interruption;
    PORT_interrupt_t interrupt_type;

    //The function to call in case of interrupt selected;
    void (*interrupt_function)(void);

    /*
     * Hardware configuration;
     */

    //The enabled multiplexing channel;
    uint8_t mux_channel;

    //The data direction;
    PORT_pin_direction_t direction;


    /*
     * Input hardware configuration
     */

    //The input mode;
    PORT_input_mode_t input_mode;

    //The input filter type;
    PORT_input_filter_data_t input_filter;


    /*
     * Output hardware configuration
     */

    //The output mode;
    PORT_output_mode_t output_mode;

    //The slew rate;
    PORT_slew_rate_t slew_rate;

} PORT_pin_config_t;


#include "Kernel/arch/arch_types.h"


//Get a pin's current configuration (avoid defaults mistakes);
void PORT_get_pin_config(PORT_data_t port, uint8_t bit, PORT_pin_config_t *);

//Set a pin's configuration. Config might have been queried before with the function behind;
void PORT_set_pin_configuration(PORT_data_t port, uint8_t bit, PORT_pin_config_t *);


#endif //TRACER_PORT_H
