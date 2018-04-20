//
// Created by root on 4/19/18.
//

#include "UART.h"

//-------------------------- Configuration methods --------------------------

/*
 * Packet format configuration : TODO;
 */

void teensy35::UART::configure_packet_format(uart_packet_config &) {

}

void teensy35::UART::configure_modem(uart_modem_config &) {

}

void teensy35::UART::configure_transmission(uart_transmission_config &) {

    //TODO FULL / HALF DUPLEX

    /*
     * BaudRate Configuration :
     *
     * The Baudrate is adjusted by two parameters, SBR and 
     */

}

void teensy35::UART::configure_state(uart_state_config &) {

}

//-------------------------- Transmission methods --------------------------


uint8_t teensy35::UART::transmission_available() {
    return 0;
}

void teensy35::UART::transmit(uint16_t) {

}

void teensy35::UART::transmit_break() {

}

//-------------------------- Reception methods --------------------------


uint8_t teensy35::UART::reception_available() {
    return 0;
}

uint16_t teensy35::UART::receive() {
    return 0;
}


//-------------------------- UARTS Definitions --------------------------

teensy35::UART *UART0 = ((teensy35::UART *)0x4006A000);
teensy35::UART *UART1 = ((teensy35::UART *)0x4006B000);
teensy35::UART *UART2 = ((teensy35::UART *)0x4006C000);
teensy35::UART *UART3 = ((teensy35::UART *)0x4006D000);
teensy35::UART *UART4 = ((teensy35::UART *)0x400EA000);
teensy35::UART *UART5 = ((teensy35::UART *)0x400EB000);