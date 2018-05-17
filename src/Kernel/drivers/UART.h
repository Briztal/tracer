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

#include "driver.h"

#include "Kernel/connection/connection.h"


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


//The constructor for the driver configuration;
#define DRIVER_CONFIG(type) {.driver_type = (type)}


/*
 * The UART configuration structure;
 */

typedef struct {

    //Composes the driver config;
    driver_config_t driver_config;

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


    //TODO NOT NECESSARY, MAKE A STATE SET FUNCTION

    //Is Tx enabled ?
    bool tx_enabled;

    //Is Rx enabled ?
    bool rx_enabled;


    //The data flux to receive from the UART; Must be constructed before;
    cflux_t *rx_flux;

    //The data flux to transmit to the UART; Must be constructed before;
    cflux_t *tx_flux;


} UART_config_t;


/*
 * An inline function obtain the default configuration;
 */

inline void UART_default_config(UART_config_t *uart_config, connection_flux_t *rx_flux, connection_flux_t *tx_flux) {

    //Set the default configuration;
    *uart_config = {
            .driver_config = DRIVER_CONFIG(UART),
            .sw_rx_buffer_size = 64,
            .sw_tx_buffer_size = 64,
            .nb_data_bits = 8,
            .parity_bit_enabled = false,
            .parity_type = EVEN_PARITY,
            .rts_enabled = false,
            .cts_enabled = false,
            .transmission_type = FULL_DUPLEX,
            .baudrate = 9600,
            .tx_enabled = true,
            .rx_enabled = true,
            .rx_flux = (rx_flux),
            .tx_flux = (tx_flux),
    };
}

#endif //TRACER_UARTDRIVER_H
