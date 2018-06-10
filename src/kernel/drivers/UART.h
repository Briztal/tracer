/*
  UART.h Part of TRACER

  Copyright (c) 2017 Raphaël Outhier

  TRACER is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  TRACER is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  aint32_t with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/

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

#include "kernel/connection/connection.h"


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
 * The buffer watermark policy;
 */

typedef enum {
    STRICT,
    HALF_REMAINING
} watermark_policy_t;


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

    //The transmission sequences_initialised;
    transmission_type_t transmission_type;

    //The Baudrate;
    uint32_t baudrate;


    //The tx watermark policy;
    watermark_policy_t tx_water_policy;

    //The tx watermark policy;
    watermark_policy_t rx_water_policy;


    //TODO NOT NECESSARY, MAKE A STATE SET FUNCTION

    //Is Tx enabled ?
    bool tx_enabled;

    //Is Rx enabled ?
    bool rx_enabled;


    //The data cflux to receive from the UART; Must be constructed before;
    flux_t *rx_flux;

    //The data cflux to transmit to the UART; Must be constructed before;
    flux_t *tx_flux;


} UART_config_t;


/*
 * An inline function obtain the default configuration;
 */

inline void UART_default_config(UART_config_t *uart_config, flux_t *rx_flux, flux_t *tx_flux) {

    //Set the default configuration;
    *uart_config = (UART_config_t) {
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


typedef struct {

    //The driver part;
    driver_t driver;

    //The start method;
    void (*start)(const void *);

    //Size methods to pass to cflux
    size_t (*tx_size)(const void *);
    size_t (*rx_size)(const void *);

    //Processing methods to pass to cflux;
    array_processor tx_processor;
    array_processor rx_processor;

} UART_driver_t;


/*
 * UART_DRIVER : definition of an UART driver;
 */

#define UART_DRIVER(data_p, init_p, start_p, exit_p, txsize_p, rxsize_p, txproc_p, rxproc_p)\
    {.driver = DRIVER(data_p, init_p, exit_p), .start = (start_p), .tx_size = (txsize_p), .rx_size = (rxsize_p), .tx_processor = (txproc_p), .rx_processor = (rxproc_p)}


/*
 * KINETIS_UART_DRIVER_DECLARE : declares the required UART driver structure;
 */

#define KINETIS_UART_DECLARE(id)\
    extern UART_driver_t UART_##id;


#endif //TRACER_UARTDRIVER_H
