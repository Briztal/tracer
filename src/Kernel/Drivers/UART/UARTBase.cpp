//
// Created by root on 4/18/18.
//


#ifndef TRACER_UARTDRIVER_CPP
#define TRACER_UARTDRIVER_CPP

#include "UARTBase.h"

//-------------------------------- Driver Initialisation -----------------------------------

/*
 * Constructor : initialises the uart and the two buffers;
 */

UARTBase::UARTBase() : receptionBuffer(nullptr), transmissionBuffer(nullptr) {}


/*
 * Driver initialisation : initialises reception and transmission buffers with the provided sizes;
 */

void UARTBase::initialise(uint16_t rx_buffer_size, uint16_t tx_buffer_size) {

    //TODO CHANGE SIZES;

    //Initialise the reception buffer;
    receptionBuffer = new CircularBuffer<uint16_t>((uint8_t)rx_buffer_size);

    //Initialise the transmission buffer;
    transmissionBuffer = new CircularBuffer<uint16_t>((uint8_t)tx_buffer_size);

    //Reset the UART in the default state;
    reset();

}


/*
 * reset : resets the UART in the default state;
 */

void UARTBase::reset() {

    //Reset the default packet structure;
    uart_packet_config packet_config;
    configure_packet_format(packet_config);

    //Reset the default modem config;;
    uart_modem_config modem_config;
    configure_modem(modem_config);

    //Reset the default transmission configuration;
    uart_transmission_config transmission_config;
    configure_transmission(transmission_config);

    //Reset the default state;
    uart_state_config state_config;
    configure_state(state_config);

}


/*
 * Destructor : Deletes the couple of buffers;
 */

UARTBase::~UARTBase() {

    //Delete both buffers and complete;
    delete receptionBuffer;
    delete transmissionBuffer;

}


//-------------------------------- Transmission functions -----------------------------------

/*
 * send : this function enqueues the provided uint16_t in the transmission
 */

void UARTBase::send(uint16_t data) {

    //If the transmission buffer is full, we need to wait until the UART can send data;
    if (!transmissionBuffer->available_spaces()) {

        //Wait till the transmission is complete;
        while (!this->transmission_available());
        //TODO SLEEP FOR 1 MS;

        //Now that transmission is available, transmit_all as many data as possible;
        transmit_all();

    }

    //If the transmission queue has spaces available spaces :
    if (transmissionBuffer->available_spaces()) {

        //Enqueue the uint16_t;
        transmissionBuffer->insert_object(data);

    }

    //Enable the transmission interrupt as a new element is available in the transmission buffer
    enable_transmission_interrupt();

    //Transmit as many uint16_t as possible;
    transmit_all();

}


//-------------------------------- Reception functions -----------------------------------

/*
 * read : return a uint16_t received through the serial, if there is one available. Returns 0 if not;
 */

uint16_t UARTBase::read() {

    //Receive as many data as possible from the UART peripheral;
    receive_all();

    //If at least uint16_t has been received :
    if (receptionBuffer->available_elements()) {

        //Return the less recent uint16_t received;
        return receptionBuffer->get_and_discard_output();

        //Enable the reception interrupt, as a new space is available in the reception buffer;
        enable_reception_interrupt();

    }

    //If no data was received, return 0;
    return 0;

}


/*
 * available : returns the number of uint16_t that can be read;
 */

uint16_t UARTBase::available() {

    //Receive as many data as possible from the UART peripheral;
    receive_all();

    //Return the number of received uint16_t;
    return receptionBuffer->available_elements();

}


#endif