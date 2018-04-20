//
// Created by root on 4/19/18.
//

#include "UART.h"


/*
 * SET : will set [data]'s bits that are set to 1 in [mask]. [data] is of size [size];
 */

#define SET(data, mask, size) (data) |= (uint##size##_t)(mask)


/*
 * CLEAR : will clear [data]'s bits that are set to 1 in [mask]. [data] is of size [size];
 */

#define CLEAR(data, mask, size) (data) &= ~(uint##size##_t)(mask)


/*
 * SET_BIT : will set the [bit_id]-th LS bit of [data]. [data] is of size [size];
 */

#define SET_BIT(data, bit_id, size) SET(data, 1 << (bit_id), size)


/*
 * CLEAR_BIT : will clear the [bit_id]-th LS bit of [data]. [data] is of size [size];
 */

#define CLEAR_BIT(data, bit_id, size) CLEAR(data, 1 << (bit_id), size)


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


/*
 * configure_modem : enables or disables CTS or RTS support;
 */

void teensy35::UART::configure_modem(uart_modem_config &config) {

    /*
     * Rx_RTS_enable and Tx_Cts_enable are (resp) bits 3 and 0 of MODEM;
     */

    //If RTS must be enabled :
    if (config.rts_enabled) {

        //Set bit 3 of MODEM;
        SET_BIT(data->MODEM, 3, 8);

    } else {
        //If RTS must be disabled, clear bit 3 of MODEM;
        CLEAR_BIT(data->MODEM, 3, 8);

    }

    //If CTS must be enabled :
    if (config.cts_enabled) {

        //Set bit 0 of MODEM;
        SET_BIT(data->MODEM, 0, 8);

    } else {

        //If CTS must be disabled, clear bit 0 of MODEM;
        CLEAR_BIT(data->MODEM, 0, 8);

    }

}


/*
 * configure_transmission : this function will setup the transmission configuration according to the required :
 *
 *  - Mode (full or half duplex);
 *  - BaudRate;
 *
 */

void teensy35::UART::configure_transmission(uart_transmission_config &config) {

    //First, cache the data pointer to avoid permanent implicit double pointer access;
    UART_DATA *ptr = data;


    /*
     * First, we will configure the transmission type : Full or Half duplex;
     */

    if (config.transmission_type == FULL_DUPLEX) {

        //To achieve a full-duplex configuration, the MSB of C1 must be cleared;
        CLEAR_BIT(ptr->C1, 7, 8);

    } else {

        //To achieve a half duplex configuration, we must set bits 5 and 7 of C1 : OR with 10100000 (160)
        SET(ptr->C1, 160, 8);

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
    uint8_t BRFA = (uint8_t) Kb & (uint8_t) 32;

    /*
     * Now we can update SBR, located in the two Baudrate registers;
     */

    //Copy the first 8 bits of SBR in BDL;
    ptr->BDL = (uint8_t) SBR;

    //Clear the first 5 bits 00011111 (31) of BDH :
    CLEAR(ptr->BDH, 31, 8);

    //Copy the 5 MSB of SBR in the 5 LSB of BDH;
    SET(ptr->BDH, (SBR >> 8), 8);

    /*
    * Now we can update BRFA, located in the 5 first bits of C4;
    */

    //Clear the 5 first bits 00011111 (31) of C4;
    CLEAR(ptr->C4, 31, 8);

    //Copy the new BRFA;
    SET(ptr->C4, BRFA, 8);

}


/*
 * configure_state : enables or disables the receiver or the transmitter;
 */

void teensy35::UART::configure_state(uart_state_config &config) {

    /*
     * Rx state : RxE is the bit 2 of C2;
     */

    //If rx must be enabled :
    if (config.rx_enabled) {

        //Set bit 2 of C2;
        SET_BIT(data->C2, 2, 8);

    } else {
        //If rx must be disabled, clear bit 2 of C2;
        CLEAR_BIT(data->C2, 2, 8);

    }


    /*
     * Tx state : TxE is the bit 3 of C2;
     */

    //If tx must be enabled :
    if (config.tx_enabled) {

        //Set bit 3 of C2;
        SET_BIT(data->C2, 3, 8);

    } else {

        //If rx must be disabled, clear bit 3 of C2;
        CLEAR_BIT(data->C2, 3, 8);

    }

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

teensy35::UART *UART0 = ((teensy35::UART *) 0x4006A000);
teensy35::UART *UART1 = ((teensy35::UART *) 0x4006B000);
teensy35::UART *UART2 = ((teensy35::UART *) 0x4006C000);
teensy35::UART *UART3 = ((teensy35::UART *) 0x4006D000);
teensy35::UART *UART4 = ((teensy35::UART *) 0x400EA000);
teensy35::UART *UART5 = ((teensy35::UART *) 0x400EB000);