
#ifndef TRACER_UARTSPECS_H
#define TRACER_UARTSPECS_H

#include "stdint.h"

/*
 * -------------------------------- Configuration data types -----------------------------------
 *
 *  Below are defined all different structures that are used to setup the UART;
 *
 *
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
 * Packet configuration structure;
 */

struct uart_packet_config {

    //The Number of data bits;
    uint8_t nb_data_bits = 8;

    //Is the parity bit present ?
    bool parity_bit_enabled = false;

    //The type of the parity bit;
    parity_bit_t parity_type = EVEN_PARITY;


};


/*
 * The UART modem configuration structure;
 */

struct uart_modem_config {

    //RTS enabled;
    bool rts_enabled = false;

    //CTS check before detection enabled;
    bool cts_enabled = false;

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

struct uart_transmission_config {

    //The transmission state;
    transmission_type_t transmission_type = FULL_DUPLEX;

    //The Baudrate;
    uint32_t baudrate = 9600;

};


/*
 * The transmission configuration structure;
 */

struct uart_state_config {

    //Is Tx enabled ?
    bool tx_enabled = false;

    //Is Rx enabled ?
    bool rx_enabled = false;

};


/*
 * The UART Peripheral Class is specific to the architecture on which we deploy the code.
 *
 * It is a class, that must at least match the following structure :

 namespace [your_architecture] {

    class UART {

        //-------------------------- Configuration methods --------------------------

    public:

        //Configure the UART packet format;
        void configure_packet_format(uart_packet_config &);

        //Configure the UART modem;
        void configure_modem(uart_modem_config &);

        //Configure the UART transmission;
        void configure_transmission(uart_transmission_config &);

        //Configure the UART state ;
        void configure_state(uart_state_config &);


         //-------------------------- Transmission methods --------------------------

    public:

         //How many uint16_t-s can we transmit to the UART ?
         uint8_t transmission_available();

         //Transmit a uint16_t to the UART;
         void transmit(uint16_t);

         //Transmit a break through the UART;
         void transmit_break();


         //-------------------------- Reception methods --------------------------

    public:

         //How many uint16_t-s can we receive from the UART ?
         uint8_t reception_available();

         //Transmit a uint16_t to the UART;
         uint16_t receive();

    };

 }

 */

#endif //TRACER_UARTSPECS_H
