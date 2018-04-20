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
 * Packet format configuration : sets :
 *  - The number of data bits;
 *  - The parity bit (enables of disables);
 *  - The parity type (Even or Odd) if it is enabled;
 */

void teensy35::UART::configure_packet_format(uart_packet_config &config) {

    //TODO MSB FIRST
    //TODO STOP BITS 1 OR 2 FIRST BYTE OF BAUDRATE REGISTER

    //To avoid implicit double pointer access, we will cache data;
    UART_DATA *ptr = data;

    /*
     * Number of bits; This peripheral only supports 8 or 9 data bits;
     *
     * The number of bits is defined by bit 4 of C1;
     */

    //Check the number of bits :
    switch(config.nb_data_bits) {

        case 8:
            //8 data bits, bit 4 cleared;
            CLEAR_BIT(ptr->C1, 4, 8);
            break;

        case 9:
            //9 data bits, bit 4 set;
            CLEAR_BIT(ptr->C1, 4, 8);
            break;

        default:
            //Other number, not suported, driver panic;
            //TODO DRIVER PANIC;
            break;

    }

    /*
     * Parity enable and type : Parity (enable and type) is defined by (resp) bits 1 and 0 of C1;
     */

    //By default, disable the 10-th bit by clearing bit 5 of C4. Will be re-enabled if necessary;
    CLEAR_BIT(ptr->C4, 5, 8);

    //If the parity bit must be enabled :
    if (config.parity_bit_enabled) {

        //Bit 1 of C1 is set;
        SET_BIT(ptr->C1, 1, 8);

        //We must set the parity type too;

        //If the parity is of type EVEN (default) :
        if (config.parity_type == EVEN_PARITY) {

            //Bit 0 of C1 is cleared;
            CLEAR_BIT(ptr->C1, 1, 8);

        } else {

            //If the parity is odd, bit 0 of C1 is set;
            SET_BIT(ptr->C1, 1, 8);

        }

        //Finally, if the parity is enabled and there are 9 data bits, the 10-th bit must be declared;
        if (config.nb_data_bits == 9) {

            //Set M10, bit 5 of C4;
            SET_BIT(ptr->C4, 5, 8);

        }

    } else {

        //Parity bit disabled, bit 1 of C1 is cleared;
        CLEAR_BIT(ptr->C1, 1, 8);

    }

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

        //TODO IF HALF DUPLEX, TXDIR MUST BE MANAGED CORRECTLY;

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
        w
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