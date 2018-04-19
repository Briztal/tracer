//
// Created by root on 4/18/18.
//


#ifndef TRACER_UARTDRIVER_CPP
#define TRACER_UARTDRIVER_CPP

#include "UARTDriver.h"

//-------------------------------- Driver Initialisation -----------------------------------

/*
 * Constructor : initialises the uart and the two buffers;
 */

UARTDriver::UARTDriver(arch::UART *uart) : uart(uart), receptionBuffer(nullptr), transmissionBuffer(nullptr) {}


/*
 * Driver initialisation : initialises reception and transmission buffers with the provided sizes;
 */

void UARTDriver::initialise(uint16_t rx_buffer_size, uint16_t tx_buffer_size) {

    //TODO CHANGE SIZES;
    //Initialise the reception buffer;
    receptionBuffer = new CircularBuffer((uint8_t)rx_buffer_size);

    //Initialise the transmission buffer;
    transmissionBuffer = new CircularBuffer((uint8_t)tx_buffer_size);

}


/*
 * Destructor : Deletes the couple of buffers;
 */

UARTDriver::~UARTDriver() {

    //Delete both buffers and complete;
    delete receptionBuffer;
    delete transmissionBuffer;

}


//-------------------------------- UART Configuration -----------------------------------

/*
 * configure_packet_format : delegates to the UART;
 */

void UARTDriver::configure_packet_format(UARTDriver::uart_packet_config &config) {
    uart->configure_packet_format(config);
}


/*
 * configure_modem : delegates to the UART;
 */

void UARTDriver::configure_modem(UARTDriver::uart_modem_config &config) {
    uart->configure_modem(config);

}


/*
 * configure_transmission : delegates to the UART;
 */

void UARTDriver::configure_transmission(UARTDriver::uart_transmission_config &) {
    uart->configure_transmission(config);

}


//-------------------------------- Transmission functions -----------------------------------

/*
 * send : this function enqueues the provided uint16_t in the transmission
 */

void UARTDriver::send(uint16_t data) {

    //If the transmission buffer is full, we need to wait until the UART can send data;
    if (!transmissionBuffer->available_spaces()) {

        //Wait till the transmission is complete;
        while (!uart->transmission_available());
        //TODO SLEEP FOR 1 MS;

        //Now that transmission is available, transmit as many data as possible;
        transmit();

    }

    //If the transmission queue has spaces available spaces :
    if (transmissionBuffer->available_spaces()) {

        //Enqueue the uint16_t;
        transmissionBuffer->insert_object(data);

    }

    //Transmit as many uint16_t as possible;
    transmit();


}


/*
 * transmit : this function transmits as many uint16_t as possible from the transmission buffer through the serial;
 */

void UARTDriver::transmit() {

    //While data can be transmitted :
    while(uart->transmission_available()) {

        //Get a uint16_t and transmit it;
        uart->transmit(transmissionBuffer->get_and_discard_output());

    }

}


/*
 * send_break : sends a break (special data) through the UART.
 *
 * The break happens immediately.
 */

void UARTDriver::send_break() {

    //Delegate the break to the uart
    uart->send_break();

}


//-------------------------------- Reception functions -----------------------------------

/*
 * read : return a uint16_t received through the serial, if there is one available. Returns 0 if not;
 */

uint16_t UARTDriver::read() {

    //Receive as many data as possible from the UART peripheral;
    receive();

    //If at least uint16_t has been received :
    if (receptionBuffer->available_elements()) {

        //Return the less recent uint16_t received;
        return receptionBuffer->get_and_discard_output();

    }

    //If no data was received, return 0;
    return 0;

}


/*
 * available : returns the number of uint16_t that can be read;
 */

uint16_t UARTDriver::available() {

    //Receive as many data as possible from the UART peripheral;
    receive();

    //Return the number of received uint16_t;
    return receptionBuffer->available_elements();

}


/*
 * receive : receive as many uint16_t as possible from the UART;
 */

void UARTDriver::receive() {

    //While data can be received by the UART peripheral :
    while(uart->reception_available()) {

        //Receive a uint16_t from the UART and insert in in the reception buffer;
        receptionBuffer->insert_object(uart->receive());

    }

}

#endif