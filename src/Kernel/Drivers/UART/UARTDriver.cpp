//
// Created by root on 4/18/18.
//


#ifndef TRACER_UARTDRIVER_CPP
#define TRACER_UARTDRIVER_CPP

#include <Kernel/Arch/Processors/core_arm_cortex_m4f.h>
#include "UARTDriver.h"

//-------------------------------- Driver Initialisation -----------------------------------

/*
 * Constructor : initialises the uart and the two buffers;
 */

UARTDriver::UARTDriver() : started(false), receptionBuffer(nullptr), transmissionBuffer(nullptr) {}


/*
 * Driver initialisation : initialises reception and transmission buffers with the provided sizes;
 */

void UARTDriver::init(uart_config &config) {

    //First, disable interrupts;
    core_disable_interrupts();
    //TODO CHANGE SIZES;

    //Initialise the reception buffer;
    receptionBuffer = new CircularBuffer<uint16_t>((uint8_t)config.sw_rx_buffer_size);

    //Initialise the transmission buffer;
    transmissionBuffer = new CircularBuffer<uint16_t>((uint8_t)config.sw_tx_buffer_size);

    //Mark the driver started;
    started = false;

    //Finally, re-enable interrupts;
    core_enable_interrupts();

}


/*
 * stop : deletes buffers and marks the driver as not started;
 */

void UARTDriver::exit() {

    //Delete both buffers and complete;
    delete receptionBuffer;
    delete transmissionBuffer;

    //Mark the driver stopped;
    started = false;

}


/*
 * Destructor : Deletes the couple of buffers;
 */

UARTDriver::~UARTDriver() {

    //Delete both buffers and complete;
    delete receptionBuffer;
    delete transmissionBuffer;

}


//-------------------------------- Transmission functions -----------------------------------

/*
 * send : this function enqueues the provided uint16_t in the transmission;
 */

void UARTDriver::send(uint16_t data) {

    //TODO SEMAPHORE !!!!!!

    //If the driver is not started, Driver panic;
    if (!started)
        return;//TODO DRIVER PANIC;

    //If the transmission buffer is full, we need to wait until the UART can send data;
    if (!transmissionBuffer->available_spaces()) {

        //Wait till the transmission is complete;
        while (!this->transmission_available());
        //TODO SLEEP FOR 1 MS;

        //Disable the preemption
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

uint16_t UARTDriver::read() {

    //De-activate all interrupts;
    core_disable_interrupts();

    //If the driver is not started, Driver panic;
    if (!started)
        return 0;//TODO DRIVER PANIC;

    //Receive as many data as possible from the UART peripheral;
    receive_all();

    //If at least uint16_t has been received :
    if (receptionBuffer->available_elements()) {

        //Cache the less recent uint16_t received;
        uint16_t i =  receptionBuffer->get_and_discard_output();

        //Enable the reception interrupt, as a new space is available in the reception buffer;
        enable_reception_interrupt();

        //Return the popped element;
        return i;
    }

    //If no data was received, return 0;
    return 0;

}


/*
 * available : returns the number of uint16_t that can be read;
 */

uint16_t UARTDriver::available() {

    //If the driver is not started, Driver panic;
    if (!started)
        return 0;//TODO DRIVER PANIC;

    //Receive as many data as possible from the UART peripheral;
    receive_all();

    //Return the number of received uint16_t;
    return receptionBuffer->available_elements();

}


#endif