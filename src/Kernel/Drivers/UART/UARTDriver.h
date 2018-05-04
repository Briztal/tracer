//
// Created by root on 4/18/18.
//

#ifndef TRACER_UARTDRIVER_H
#define TRACER_UARTDRIVER_H

//Include the architecture library
#include "Kernel/Arch/arch.h"

/*
 * The base driver for UART.
 *
 *  It handles a simple UART that comprises hardware FIFOs for in/out buffering (FIFOs can be of size 1);
 */

//Int types;
#include "stdint.h"

//Buffers to contain temporary data;
#include <DataStructures/Containers/CircularBuffer.h>
#include <Kernel/Drivers/Driver.h>


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

struct uart_config {

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

};


class UARTDriver {

    //-------------------------------- Driver Initialisation -----------------------------------

public:

    //The constructor;
    UARTDriver();

    //Driver start;
    virtual void init(uart_config &);

    //Driver stop;
    virtual void exit();

    //The destructor;
    virtual ~UARTDriver();


    //-------------------------------- Disabled constructors and operators -----------------------------------

private:

    //Disabled copy constructor;
    UARTDriver(const UARTDriver &) : receptionBuffer(nullptr), transmissionBuffer(nullptr) {}

    //Disabled move constructor;
    UARTDriver(UARTDriver &&) noexcept : receptionBuffer(nullptr), transmissionBuffer(nullptr) {}

    //Disabled copy assignment operator;
    void operator=(const UARTDriver &) {};

    //Disabled move assignment operator;
    void operator=(UARTDriver &&) noexcept {};


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

    //Is the driver started ?
    bool started;

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
    virtual void start(uart_config &) = 0;


    //-------------------------------- Interrupts enable functions -----------------------------------

protected :

    //Enable the reception interrupt
    virtual void enable_reception_interrupt() = 0;

    //Enable the transmission interrupt
    virtual void enable_transmission_interrupt() = 0;


    //-------------------------- Transmission methods --------------------------

protected:

    //How many uint16_t-s can we receive_all from the UART ?
    virtual uint8_t transmission_available() = 0;

    //Transmit as many uint16_t as possible to the UART peripheral;
    virtual void transmit_all() = 0;

    //Transmit a break through the UART;
    virtual void transmit_break() = 0;


    //-------------------------- Reception methods --------------------------

protected:

    //Receive as many data as possible from the UART Peripheral;
    virtual void receive_all() = 0;


};

#endif //UART_SUPPORTED

#endif //TRACER_UARTDRIVER_H
