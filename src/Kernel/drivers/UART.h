//
// Created by root on 4/18/18.
//

#ifndef TRACER_UARTDRIVER_H
#define TRACER_UARTDRIVER_H

/*
 * The base driver for UART.
 *
 *  It handles a simple UART that comprises hardware FIFOs for in/out buffering (FIFOs can be of size 1);
 */

//Int types;
#include "stdint.h"

/*
 * -------------------------------- Configuration data types -----------------------------------
 *
 *  Below are defined all different structures that are used to setup the UART;
 *
 */


/*
 * Number of possible data bits;
 */

enum parity_bit_t {
    EVEN_PARITY,
    ODD_PARITY
};


/*
 * The type of the transmission;
 */

enum transmission_type_t {
    HALF_DUPLEX,
    FULL_DUPLEX
};


/*
 * The transmission configuration structure;
 */

typedef struct {

    //The size of the reception software buffer;
    uint16_t sw_rx_buffer_size = 64;

    //The size of the reception software buffer;
    uint16_t sw_tx_buffer_size = 64;

    //The Number of data bits;
    uint8_t nb_data_bits = 8;

    //Is the parity bit present ?
    bool parity_bit_enabled = false;

    //The type of the parity bit;
    parity_bit_t parity_type = EVEN_PARITY;

    //RTS enabled;
    bool rts_enabled = false;

    //CTS check before detection enabled;
    bool cts_enabled = false;

    //The transmission state;
    transmission_type_t transmission_type = FULL_DUPLEX;

    //The Baudrate;
    uint32_t baudrate = 9600;

    //Is Tx enabled ?
    bool tx_enabled = true;

    //Is Rx enabled ?
    bool rx_enabled = true;

} UART_config;


typedef struct {

    //The UART data pointer;
    void *UART_data;


    //-------------------------- Configuration methods --------------------------

    //Initialise the UART;
    void (*initialise)(void *data, uart_config *);


    //-------------------------------- Interrupts enable functions -----------------------------------

    //Enable the reception interrupt
    void (*enable_reception_interrupt)(void *UART_data);

    //Enable the transmission interrupt
    void (*enable_transmission_interrupt)(void *UART_data);


    //-------------------------- Transmission methods --------------------------

    //How many uint16_t-s can we receive_all from the UART ?
    uint8_t (*transmission_available)(void *UART_data);

    //Transmit as many uint16_t as possible to the UART peripheral;
    void (*transmit_all)(void *UART_data);

    //Transmit a break through the UART;
    void (*transmit_break)(void *UART_data);


    //-------------------------- Reception methods --------------------------

    //Receive as many data as possible from the UART Peripheral;
    void (*receive_all)(void *UART_data);

} UART_driver_t;


#endif //TRACER_UARTDRIVER_H
