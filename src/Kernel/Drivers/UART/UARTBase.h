//
// Created by root on 4/18/18.
//

#ifndef TRACER_UARTDRIVER_H
#define TRACER_UARTDRIVER_H

//Include the architecture library
#include "Kernel/Arch/arch.h"

//If the UART is supported :
#ifdef UART_SUPPORTED

/*
 * The base driver for UART.
 *
 *  It handles a simple UART that comprises hardware FIFOs for in/out buffering (FIFOs can be of size 1);
 */

//Int types;
#include "stdint.h"

//Buffers to contain temporary data;
#include <DataStructures/Containers/CircularBuffer.h>


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


class UARTBase {

    //-------------------------------- Driver Initialisation -----------------------------------

public:

    //The constructor. Takes the UART instance in argument;
    explicit UARTBase();

    //Driver initialisation;
    void initialise(uint16_t rx_buffer_size, uint16_t tx_buffer_size);

    //Driver reset : resets the UART in the default state;
    void reset();

    //The destructor;
    virtual ~UARTBase();


    //-------------------------------- Disabled constructors and operators -----------------------------------

private:

    //Disabled copy constructor;
    UARTBase(const UARTBase &) : receptionBuffer(nullptr), transmissionBuffer(nullptr) {}

    //Disabled move constructor;
    UARTBase(UARTBase &&) noexcept : receptionBuffer(nullptr), transmissionBuffer(nullptr) {}

    //Disabled copy assignment operator;
    void operator=(const UARTBase &) {};

    //Disabled move assignment operator;
    void operator=(UARTBase &&) noexcept {};


    //-------------------------------- Buffer access functions -----------------------------------

public:

    //Send an uint16_t through the UART when possible;
    void send(uint16_t data);

    //Read an uint16_t received through the serial. If no data was received, returns 0;
    uint16_t read();

    //Return the number of uint16_t that can be read with the function above;
    uint16_t available();


    //-------------------------------- Fields -----------------------------------

protected:

    //TODO THESE BUFFER TAKE SPACE IN MEMORY DUE TO ALLOCATION FLAG. MAKE A LIGHTWEIGHT CONTAINER;
    //The reception buffer;
    CircularBuffer<uint16_t> *receptionBuffer;

    //The transmission buffer;
    CircularBuffer<uint16_t> *transmissionBuffer;


    /*
     * -------------------------------- Virtual Methods -----------------------------------
     */

    //-------------------------- Configuration methods --------------------------

public:

    //Configure the UART packet format;
    virtual void configure_packet_format(uart_packet_config &) = 0;

    //Configure the UART modem;
    virtual void configure_modem(uart_modem_config &) = 0;

    //Configure the UART transmission;
    virtual void configure_transmission(uart_transmission_config &) = 0;

    //Configure the UART state ;
    virtual void configure_state(uart_state_config &) = 0;


    //-------------------------------- Interrupts enable functions -----------------------------------

public:

    //Enable the reception interrupt
    virtual void enable_reception_interrupt() = 0;

    //Enable the transmission interrupt
    virtual void enable_transmission_interrupt() = 0;


    //-------------------------- Transmission methods --------------------------

public:

    //How many uint16_t-s can we receive_all from the UART ?
    virtual uint8_t transmission_available() = 0;

    //Transmit as many uint16_t as possible to the UART peripheral;
    virtual void transmit_all() = 0;

    //Transmit a break through the UART;
    virtual void transmit_break() = 0;


    //-------------------------- Reception methods --------------------------

public:

    //Receive as many data as possible from the UART Peripheral;
    virtual void receive_all() = 0;


};

#endif //UART_SUPPORTED

#endif //TRACER_UARTDRIVER_H
