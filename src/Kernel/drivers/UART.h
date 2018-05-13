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

#include "stdint.h"

#include "stdbool.h"

/*
 * -------------------------------- Configuration data types -----------------------------------
 *
 *  Below are defined all different structures that are used to setup the UART;
 *
 */


/*
 * Number of possible data bits;
 */

typedef enum {
    EVEN_PARITY,
    ODD_PARITY
} parity_bit_t;


/*
 * The type of the transmission;
 */

typedef enum {
    HALF_DUPLEX,
    FULL_DUPLEX
} transmission_type_t;


/*
 * The transmission configuration structure;
 */

typedef struct {

    //The size of the reception software buffer;
    uint16_t sw_rx_buffer_size;

    //The size of the reception software buffer;
    uint16_t sw_tx_buffer_size;

    //The Number of data bits;
    uint8_t nb_data_bits;

    //Is the parity bit present ?
    bool parity_bit_enabled;

    //The type of the parity bit;
    parity_bit_t parity_type;

    //RTS enabled;
    bool rts_enabled;

    //CTS check before detection enabled;
    bool cts_enabled;

    //The transmission state;
    transmission_type_t transmission_type;

    //The Baudrate;
    uint32_t baudrate;

    //Is Tx enabled ?
    bool tx_enabled;

    //Is Rx enabled ?
    bool rx_enabled;

} UART_config_t;


#define UART_DEFAULT_CONFIG \
{\
    .sw_rx_buffer_size = 64,\
    .sw_tx_buffer_size = 64,\
    .nb_data_bits = 8,\
    .parity_bit_enabled = false,\
    .parity_type = EVEN_PARITY,\
    .rts_enabled = false,\
    .cts_enabled = false,\
    .transmission_type = FULL_DUPLEX,\
    .baudrate = 9600,\
    .tx_enabled = true,\
    .rx_enabled = true,\
}

typedef struct {

    //The UART data pointer;
    void *UART_data;


    //-------------------------- Configuration methods --------------------------

    //Initialise the UART;
    void (*initialise)(void *data, UART_config *);


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
