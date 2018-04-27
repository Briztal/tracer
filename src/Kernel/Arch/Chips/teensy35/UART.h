//
// Created by root on 4/19/18.

//TODO COPYRIGHT PJRC.COM LLC EN PLUS
//

#ifndef TRACER_TEENSY35_UART_H
#define TRACER_TEENSY35_UART_H

#include <Kernel/Drivers/UART/UARTBase.h>
#include "kinetis.h"

namespace teensy35 {


    enum fifo_size_t {
        size_1 = 0,
        size_4 = 1,
        size_8 = 2,
        size_16 = 3,
        size_32 = 4,
        size_64 = 5,
        size_128 = 6,
    };

    /*
     * First, we define the UART registers map. As all data related to a specific UART are in a continuous memory zone,
     *  We can assimilate them as a struct, whose address is known in memory;
     */

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

    } UARTRegisters;


    /*
     * Then, we define the UART data structure, that will contain a memory map, and a set of hardware constants;
     */
    typedef struct UARTData {

        //The registers struct;
        UARTRegisters *const registers;

        //The clock frequency;
        const uint32_t clockFrequency;

        //The maximum number of uint16_t in the tx hardware buffer;
        const uint8_t tx_max;

    };


    /*
     * Finally, we can define our UARTBase, that will inherit of the UARTBase class, and implements peripheral
     *  for hardware UARTS;
     *
     *  The teensy35's UART integrates a hardware buffer for both rx and tx canals, whose sizes depend on the module;
     *
     *  It implements a watermark functionality in those buffers. When the amount of data in a buffer is greater
     *      (or lesser) or equal than the watermark, an interrupt is triggered;
     *
     *
     *  The different types of status (not error) interrupts are :
     *
     *   - TI : transmission interrupts, triggers when the amount of data in tx buffer is lesser or equal than
     *      the tx watermark; The attached interrupt must transfer as many data as possible from the transmission
     *      buffer to the hardware buffer;
     *      This interrupt is disabled when no more data is available in the transmission buffer (to avoid a
     *      perpetual unnecessary call; Each time an element is added in the transmission buffer, it is re-enabled;
     *
     *   - RI : reception interrupt, triggers when the amount of data in rx buffer is greater than the TX watermark;
     *      The attached interrupt must transfer as many data as possible from the hardware rx buffer to the reception
     *      buffer;
     *      This interrupt is disabled when there are no more spaces available in the reception buffer. Each time
     *      a space is fred in the reception buffer (call of read method), it is re-enabled;
     *
     *
     *  HALF DUPLEX INTEGRATION :
     *      - TODO DOC
     *      - WHEN TRANSMISSION REQUIRED (element added in txbuffer) : enable IDLE INTERRUPT.
     *      - AT IDLE INTERRUPT : CHECK THAT HALF DUPLEX MODE, SET TXDIR, TRANSMISSION OF ALL POSSIBLE ELEMENTS IN THE BUFFER
     *      - TODO CHECK THAT TXWATER INTERRUPT IS CALLED.
     *      - TRANSMISSION COMPLETE INTERRUPT : CHECK THAT HALF DUPLEX MODE : RESET TXDIR;
     *
     */

    class UART : UARTBase {

        /*
         * -------------------------- Object Methods --------------------------
         *
         * As hardware peripheral exist in memory, they can't be copied, moved, destructed or assigned;
         */

    public:

        //Enabled Constructor;
        UART(UARTData *data, fifo_size_t rx_size, fifo_size_t tx_size, uint8_t status_interrupt_index, uint8_t error_interrupt_index, void (*status_interrupt_link)(), void (*error_interrupt_link)());


//TODO EVENTUALLY MERGE UARTDATA AND UARTDRIVER TO ECONOMISE A POINTER ACCESS

    private:

        //Disabled Copy Constructor;
        UART(const UART &) : data(nullptr) {};

        //Disabled Move Constructor;
        UART(UART &&) noexcept : data(nullptr) {};

        //Disabled Destructor;
        ~UART() final = default;

        //Disabled copy assignment;
        void operator=(const UART &) {};

        //Disabled move assignment;
        void operator=(UART &&) noexcept {};


        //-------------------------- Configuration methods --------------------------

    public:

        //Configure the UART packet format;
        void configure_packet_format(uart_packet_config &) final;

        //Configure the UART modem;
        void configure_modem(uart_modem_config &) final;

        //Configure the UART transmission;
        void configure_transmission(uart_transmission_config &) final;

        //Configure the UART state ;
        void configure_state(uart_state_config &) final;


        //-------------------------------- Interrupts enable functions -----------------------------------

    public:

        //Enable the reception interrupt
        void enable_reception_interrupt() final;

        //Enable the transmission interrupt
        void enable_transmission_interrupt() final;


        //-------------------------- Transmission methods --------------------------

    public:

        //How many uint16_t-s can we receive_all from the UART ?
        uint8_t transmission_available() final;

        //Transmit a uint16_t to the UART;
        void transmit_all() final;

        //Transmit a break through the UART;
        void transmit_break() final;


        //-------------------------- Reception methods --------------------------

    public:

        //Transmit a uint16_t to the UART;
        void receive_all() final;


        //-------------------------- Exceptions --------------------------

    public:

        //The function to be called at each interrupt (on this type of UART, interrupts are polled together);
        void interrupt();

        //The function to be called at each error (on this type of UART, errors are polled together);
        void error();


        //-------------------------- Fields --------------------------

    private:

        //A pointer to the UART data in memory;
        UARTData *data;

        //The maximum number of words in the tx buffer;
        uint8_t tx_max;

    };

}


//-------------------------- UARTS Declarations --------------------------

/*
 * The teensy35 comprises 6 UARTS;
 */

extern teensy35::UART UART0;
extern teensy35::UART UART1;
extern teensy35::UART UART2;
extern teensy35::UART UART3;
extern teensy35::UART UART4;
extern teensy35::UART UART5;


#endif //TRACER_TEENSY35_UART_H
