//
// Created by root on 4/19/18.

//TODO COPYRIGHT PJRC.COM LLC EN PLUS
//

#ifndef TRACER_TEENSY35_UART_H
#define TRACER_TEENSY35_UART_H

#include "Kernel/Drivers/UART/UARTSpecs.h"

#include "kinetis.h"

namespace teensy35 {

    class __attribute__ UART {

        typedef struct __attribute__ ((packed)) {
            volatile uint8_t BDH;
            volatile uint8_t BDL;
            volatile uint8_t C1;
            volatile uint8_t C2;
            volatile uint8_t S1;
            volatile uint8_t S2;
            volatile uint8_t C3;
            volatile uint8_t D;
            volatile uint8_t MA1;
            volatile uint8_t MA2;
            volatile uint8_t C4;
            volatile uint8_t C5;
            volatile uint8_t ED;
            volatile uint8_t MODEM;
            volatile uint8_t IR;
            volatile uint8_t unused1;
            volatile uint8_t PFIFO;
            volatile uint8_t CFIFO;
            volatile uint8_t SFIFO;
            volatile uint8_t TWFIFO;
            volatile uint8_t TCFIFO;
            volatile uint8_t RWFIFO;
            volatile uint8_t RCFIFO;
            volatile uint8_t unused2;
            volatile uint8_t C7816;
            volatile uint8_t IE7816;
            volatile uint8_t IS7816;
            union {
                volatile uint8_t WP7816T0;
                volatile uint8_t WP7816T1;
            };
            volatile uint8_t WN7816;
            volatile uint8_t WF7816;
            volatile uint8_t ET7816;
            volatile uint8_t TL7816;
            volatile uint8_t unused3;
            volatile uint8_t C6;
            volatile uint8_t PCTH;
            volatile uint8_t PCTL;
            volatile uint8_t B1T;
            volatile uint8_t SDTH;
            volatile uint8_t SDTL;
            volatile uint8_t PRE;
            volatile uint8_t TPL;
            volatile uint8_t IE;
            volatile uint8_t WB;
            volatile uint8_t S3;
            volatile uint8_t S4;
            volatile uint8_t RPL;
            volatile uint8_t RPREL;
            volatile uint8_t CPW;
            volatile uint8_t RIDT;
            volatile uint8_t TIDT;

        } UART_DATA;

        /*
         * -------------------------- Object Methods --------------------------
         *
         * As hardware peripheral exist in memory, they can't be copied, moved, destructed or assigned;
         */

        //Enabled Constructor;
        UART(UART_DATA *, uint32_t uart_frequency);

    private:

        //Disabled Copy Constructor;
        UART(const UART &) {};

        //Disabled Move Constructor;
        UART(UART &&) noexcept {};

        //Disabled Destructor;
        ~UART() {};

        //Disabled copy assignment;
        void operator=(const UART &) {};

        //Disabled move assignment;
        void operator=(UART &&) noexcept {};


        //-------------------------- Configuration methods --------------------------

    public:

        //Configure the UART packet format;
        void configure_packet_format(uart_packet_config &);

        //Configure the UART modem;
        void configure_modem(uart_modem_config &);

        //Configure the UART transmission;
        void configure_transmission(uart_transmission_config &);

        //Configure the UART state ;
        void configure_state(uart_state_config &);


        //-------------------------- Transmission methods --------------------------

    public:

        //How many uint16_t-s can we transmit to the UART ?
        uint8_t transmission_available();

        //Transmit a uint16_t to the UART;
        void transmit(uint16_t);

        //Transmit a break through the UART;
        void transmit_break();


        //-------------------------- Reception methods --------------------------

    public:

        //How many uint16_t-s can we receive from the UART ?
        uint8_t reception_available();

        //Transmit a uint16_t to the UART;
        uint16_t receive();


        //-------------------------- Reception methods --------------------------

    private:

        //A pointer to the UART data in memory;
        UART_DATA *data;

        //The clock frequency;
        uint32_t clockFrequency;
    };


}

//-------------------------- UARTS Declarations --------------------------

/*
 * The teensy35 comprises 6 UARTS;
 */

extern teensy35::UART *UART0;
extern teensy35::UART *UART1;
extern teensy35::UART *UART2;
extern teensy35::UART *UART3;
extern teensy35::UART *UART4;
extern teensy35::UART *UART5;


#endif //TRACER_TEENSY35_UART_H
