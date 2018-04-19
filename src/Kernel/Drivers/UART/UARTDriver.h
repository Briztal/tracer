//
// Created by root on 4/18/18.
//

#ifndef TRACER_UARTDRIVER_H
#define TRACER_UARTDRIVER_H

//Include the architecture library
#include "Kernel/Arch/arch.h"

//If the UART is supported :
#ifdef UART_SUPPORTED

//Include the UART class;
#include ARCH_PATH(UART)

/*
 * The base driver for UART.
 *
 *  It handles a simple UART that comprises hardware FIFOs for in/out buffering (FIFOs can be of size 1);
 */

#include <DataStructures/Containers/CircularBuffer.h>

#include "stdint.h"

class UARTDriver {


    //-------------------------------- Driver Initialisation -----------------------------------

public:

    //The constructor. Takes the UART instance in argument;
    explicit UARTDriver(arch::UART *);

    //Driver initialisation;
    void initialise(uint16_t rx_buffer_size, uint16_t tx_buffer_size);

    //The destructor;
    ~UARTDriver();


    //-------------------------------- Disabled constructors and operators -----------------------------------

private:

    //Disabled copy constructor;
    UARTDriver(const UARTDriver &) : uart(nullptr), receptionBuffer(nullptr), transmissionBuffer(nullptr) {}

    //Disabled move constructor;
    UARTDriver(UARTDriver &&) noexcept : uart(nullptr), receptionBuffer(nullptr), transmissionBuffer(nullptr) {}

    //Disabled copy assignment operator;
    void operator=(const UARTDriver &) {};

    //Disabled move assignment operator;
    void operator=(UARTDriver &&) noexcept {};


    //-------------------------------- Configuration data types -----------------------------------

public:

    //Number of possible data bits;
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

        //The number of parity bits;
        uint8_t nb_stop_bits = 1;

    };

    struct uart_modem_config {

        //RTS enabled;
        bool rts_enabled = false;

        //CTS check before detection enabled;
        bool cts_enabled = false;

    };

    //The type of the transmission;
    enum transmission_type_t {

        HALF_DUPLEX,
        FULL_DUPLEX
    };

    //The transmission config
    struct uart_transmission_config {

        //The transmission state;
        transmission_type_t transmission_type = FULL_DUPLEX;

    };


    //-------------------------------- Configuration functions -----------------------------------

    //Configure the packet format;
    inline void configure_packet_format(uart_packet_config &);

    //Configure the modem;
    inline void configure_modem(uart_modem_config &);

    //Configure the transmission;
    inline void configure_transmission(uart_transmission_config &);


    //-------------------------------- Transmission functions -----------------------------------

    //Send an uint16_t through the UART when possible;
    void send(uint16_t data);

    //Send a break through the serial;
    void send_break();

    //Transmit as many uint16_t as possible to the UART peripheral;
    void transmit();


    //-------------------------------- Reception functions -----------------------------------

    //Read an uint16_t received through the serial. If no data was received, returns 0;
    uint16_t read();

    //Return the number of uint16_t that can be read with the function above;
    uint16_t available();

    //Receive as many data as possible from the UART Peripheral;
    void receive();


    //-------------------------------- Fields -----------------------------------

    //The UART we monitor;
    arch::UART *uart;

    //TODO THESE BUFFER TAKE SPACE IN MEMORY DUE TO ALLOCATION FLAG. MAKE A LIGHTWEIGHT CONTAINER;
    //The reception buffer;
    CircularBuffer<uint16_t> *receptionBuffer;

    //The transmission buffer;
    CircularBuffer<uint16_t> *transmissionBuffer;

};

#endif //UART_SUPPORTED

#endif //TRACER_UARTDRIVER_H
