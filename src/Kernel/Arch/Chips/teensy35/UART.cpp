//
// Created by root on 4/19/18.
//

#include "UART.h"

/*
 * Constructor : initialises the data pointer and the frequency;w
 */

teensy35::UART::UART(teensy35::UART::UART_DATA *data, uint32_t uart_frequency)
        : data(data), clockFrequency(uart_frequency) {}

//-------------------------- Configuration methods --------------------------

/*
 * Packet format configuration : TODO;
 */

void teensy35::UART::configure_packet_format(uart_packet_config &) {

}

void teensy35::UART::configure_modem(uart_modem_config &) {

}

void teensy35::UART::configure_transmission(uart_transmission_config &config) {

    //First, cache the data pointer to avoid permanent implicit double pointer access;
    UART_DATA *ptr = data;


    /*
     * First, we will configure the transmission type : Full or Half duplex;
     */

    if (config.transmission_type == FULL_DUPLEX) {

        //To achieve a full-duplex configuration, the MSB of C1 must be reset : AND with the compl of 10000000 (128);
        ptr->C1 &= ~128;

    } else {

        //To achieve a half duplex configuration, we must set bits 5 and 7 of C1 : OR with 10100000 (160)
        ptr->C1 |= 160;

    }



    /*
     * BaudRate Configuration :
     *
     * The Baudrate is adjusted by two parameters, SBR (Coarse adjust) and BRFD (Fine adjust),
     *  that verify :
     *
     *      Kb = 2 * UART_CLOCK / BaudRate
     *
     *      SBR (uint13_t) = Kb / 32 (integer division);
     *      BRFA (uint5_t) = Kb % 32;
     */

    //First, determine Kb;
    uint32_t Kb = ((uint32_t) ((uint32_t) (clockFrequency << 1)) / config.baudrate);

    //Then, determine SBR. A division by 32 is a shift by 5;
    uint16_t SBR = (uint16_t) (Kb >> 5);

    //Finally, determine SBR. A modulo by 32 is a AND by 32;
    uint8_t BRFA = (uint8_t) Kb & (uint8_t)32;

    /*
     * Now we can update SBR, located in the two Baudrate registers;
     */

    //Copy the first 8 bits of SBR in BDL;
    data->BDL = (uint8_t) SBR;

    //Reset the first 5 bits of BDH with an AND with 11100000 (224);
    data->BDH &= (uint8_t)224;

    //Copy the 5 MSB of SBR in the 5 LSB of BDH;
    data->BDH |= (uint8_t) (SBR >> 8);

    /*
    * Now we can update BRFA, located in the 5 first bits of C4;
    */

    //Reset BFRA with an AND with 11100000 (224);
    data->C4 &= (uint8_t)224;

    //Copy the new BRFA;
    data->C4 |= BRFA;

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