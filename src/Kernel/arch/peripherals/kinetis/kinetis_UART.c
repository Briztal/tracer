//
// Created by root on 4/19/18.
//

#include <Kernel/arch/arch.h>

#include "kinetis_UART.h"


//TODO RECEIVE OVERRUN

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



//Configure the packet format;
void initialise_hardware(kinetis_UART_data_t *);

//Configure the packet format;
void configure_packet_format(kinetis_UART_data_t *data, UART_config_t *);

//Configure the state;
void configure_state(kinetis_UART_data_t *data, UART_config_t *);

//Configure the state;
void configure_modem(kinetis_UART_data_t *data, UART_config_t *);

//Configure the transmission layer;
void configure_transmission_layer(kinetis_UART_data_t *data, UART_config_t *);


/*
 * Constructor : initialises the data pointer and the frequency;w
 */


void initialise_hardware(kinetis_UART_data_t *data) {

    //Before anything, let's cache the memory pointer;
    kinetis_UART_memory_t *registers = data->memory;

    /*
     * First, we will configure the peripheral hardware;
     */

    //Set both FIFOs enabled, and their size set with provided values,
    registers->PFIFO = (uint8_t) UART_PFIFO_TXFE | (uint8_t) UART_PFIFO_RXFE |
            (uint8_t) UART_PFIFO_TXFIFOSIZE(tx_size) | (uint8_t) UART_PFIFO_RXFIFOSIZE(rx_size);

    //The rx watermark will be set as the half of the rx buffer size;
    switch (rx_size) {
        case size_1 :
            registers->RWFIFO = 1;
            break;
        case size_4 :
            registers->RWFIFO = 2;
            break;
        case size_8 :
            registers->RWFIFO = 4;
            break;
        case size_16 :
            registers->RWFIFO = 8;
            break;
        case size_32 :
            registers->RWFIFO = 16;
            break;
        case size_64 :
            registers->RWFIFO = 32;
            break;
        case size_128 :
            registers->RWFIFO = 64;
            break;
    }

    //The rx watermark will be set as the half of the rx buffer size;
    switch (tx_size) {
        case size_1 :
            registers->TWFIFO = 0;
            break;
        case size_4 :
            registers->TWFIFO = 2;
            break;
        case size_8 :
            registers->TWFIFO = 4;
            break;
        case size_16 :
            registers->TWFIFO = 8;
            break;
        case size_32 :
            registers->TWFIFO = 16;
            break;
        case size_64 :
            registers->TWFIFO = 32;
            break;
        case size_128 :
            registers->TWFIFO = 64;
            break;
    }

    /*
     * Interrupts to enable are :
     *  - transmission complete; TODO INTERRUPT SUPPORT FOR HALF DUPLEX
     *  - transmission buffer empty (below water);
     *  - reception buffer empty (beyond water);
     *  - idle line; TODO INTERRUPT SUPPORT FOR HALF DUPLEX
     *
     *  All those are located in the C2 register;
     */

    //Enable all interrupts,
    SET(registers->C2, UART_C2_TIE | UART_C2_RIE, 8);

    //Cache the status interrupt index;
    const uint8_t status_id = data->status_interrupt_id;

    //Set the status interrupt priority;
    core_set_interrupt_priority(status_id, DRIVER_STARUS_INTERRUPT_PRIORITY);

    //Eventually de-activate a pending status interrupt;
    core_clear_interrupt_pending(status_id);

    //Set the provided interrupt link as the interrupt function;
    core_set_interrupt_handler(status_id, status_interrupt_link);

    //Enable the status interrupt;
    core_enable_interrupt(status_id);


    /*
     * Errors supported are :
     *  - framing errors (locking) - read and forget last element;
     *  - receiver overrun (locking) - receiver overflow (same ones) -> receiver flush;
     *  - receiver underflow (misalignment) -> receiver flush;
     *  - transmitter overflow (misalignment) -> transmitter flush;
     *  - noise flag - parity error (informative) -> read and forget last element;
     */

    //Enable framing, overrun, noise and parity errors to generate interrupt;
    SET(registers->C3, UART_C3_FEIE | UART_C3_ORIE | UART_C3_NEIE | UART_C3_PEIE, 8);

    //Enable reciver overflow, receiver underflow, transmitter overflow to generate interrupts;
    SET(registers->CFIFO, UART_CFIFO_RXOFE | UART_CFIFO_RXUFE | UART_CFIFO_TXOFE, 8);


    //Cache the error interrupt index;
    const uint8_t error_id = data->error_interrupt_id;

    //Set the error interrupt priority;
    core_set_interrupt_priority(error_id, DRIVER_ERROR_INTERRUPT_PRIORITY);

    //Eventually de-activate a pending status interrupt;
    core_clear_interrupt_pending(error_id);

    //Set the provided interrupt link as the interrupt function;
    core_set_interrupt_handler(error_id, error_interrupt_link);

    //Enable the status interrupt;
    core_enable_interrupt(error_id);

}


//-------------------------- Configuration methods --------------------------

/*
 * configure_packet_format : this function configures the UART. Config is divided into several private functions
 *  that are all called;
 */

void kinetis_UART_init(kinetis_UART_data_t *data, UART_config_t *config, connection_flux_t *output_stream) {

    configure_packet_format(data, config);
    configure_state(data, config);
    configure_modem(data, config);
    configure_transmission_layer(data, config);

}


/*
 * Packet format configuration : sets :
 *  - The number of data bits;
 *  - The parity bit (enables of disables);
 *  - The parity type (Even or Odd) if it is enabled;
 */

void kinetis_UART_configure_packet_format(kinetis_UART_data_t *data, UART_config_t *config) {

    //TODO MSB FIRST
    //TODO STOP BITS 1 OR 2 FIRST BYTE OF BAUDRATE REGISTER

    //To avoid implicit double pointer access, we will cache data;
    kinetis_UART_memory_t *const registers = data->memory;
    
    /*
     * Number of bits; This peripheral only supports 8 or 9 data bits;
     *
     * The number of bits is defined by bit 4 of C1;
     */

    //Check the number of bits :
    switch (config->nb_data_bits) {

        case 8:
            //8 data bits, bit 4 cleared;
            CLEAR_BIT(registers->C1, 4, 8);
            break;

        case 9:
            //9 data bits, bit 4 set;
            CLEAR_BIT(registers->C1, 4, 8);
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
    CLEAR_BIT(registers->C4, 5, 8);

    //If the parity bit must be enabled :
    if (config->parity_bit_enabled) {

        //Bit 1 of C1 is set;
        SET_BIT(registers->C1, 1, 8);

        //We must set the parity type too;

        //If the parity is of type EVEN (default) :
        if (config->parity_type == EVEN_PARITY) {

            //Bit 0 of C1 is cleared;
            CLEAR_BIT(registers->C1, 1, 8);

        } else {

            //If the parity is odd, bit 0 of C1 is set;
            SET_BIT(registers->C1, 1, 8);

        }

        //Finally, if the parity is enabled and there are 9 data bits, the 10-th bit must be declared;
        if (config->nb_data_bits == 9) {

            //Set M10, bit 5 of C4;
            SET_BIT(registers->C4, 5, 8);

        }

    } else {

        //Parity bit disabled, bit 1 of C1 is cleared;
        CLEAR_BIT(registers->C1, 1, 8);

    }

}


/*
 * configure_modem : enables or disables CTS or RTS support;
 */

void kinetis_UART_configure_modem(kinetis_UART_data_t *data, UART_config_t *config) {

    kinetis_UART_memory_t *const registers = data->memory;
    /*
     * Rx_RTS_enable and Tx_Cts_enable are (resp) bits 3 and 0 of MODEM;
     */

    //If RTS must be enabled :
    if (config->rts_enabled) {

        //Set bit 3 of MODEM;
        SET(registers->MODEM, UART_MODEM_RXRTSE, 8);

    } else {
        //If RTS must be disabled, clear bit 3 of MODEM;
        CLEAR(registers->MODEM, UART_MODEM_RXRTSE, 8);

    }

    //If CTS must be enabled :
    if (config->cts_enabled) {

        //Set bit 0 of MODEM;
        SET(registers->MODEM, UART_MODEM_TXCTSE, 8);

    } else {

        //If CTS must be disabled, clear bit 0 of MODEM;
        CLEAR(registers->MODEM, UART_MODEM_TXCTSE, 8);

    }

}


/*
 * configure_transmission_layer : this function will setup the transmission configuration according to the required :
 *
 *  - Mode (full or half duplex);
 *  - BaudRate;
 *
 */

void kinetis_UART_configure_transmission_layer(kinetis_UART_data_t *data, UART_config_t *config) {

    //First, cache the data pointer to avoid permanent implicit double pointer access;
    kinetis_UART_memory_t *registers = data->memory;


    /*
     * First, we will configure the transmission type : Full or Half duplex;
     */

    if (config->transmission_type == FULL_DUPLEX) {

        //To achieve a full-duplex configuration, the MSB of C1 must be cleared;
        CLEAR(registers->C1, UART_C1_LOOPS, 8);

        //TODO IF HALF DUPLEX, TXDIR MUST BE MANAGED CORRECTLY;

    } else {

        //To achieve a half duplex configuration, we must set bits 5 and 7 of C1 : OR with 10100000 (160)
        SET(registers->C1, UART_C1_LOOPS | UART_C1_RSRC, 8);

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



    //TODO USE MACROS IN KINETIS.H

    //First, determine Kb;
    uint32_t Kb = ((uint32_t) ((uint32_t) (data->clockFrequency << 1)) / config->baudrate);

    //Then, determine SBR. A division by 32 is a shift by 5;
    uint16_t SBR = (uint16_t) (Kb >> 5);

    //Finally, determine SBR. A modulo by 32 is a AND by 32;
    uint8_t BRFA = (uint8_t) Kb & (uint8_t) 32;

    /*
     * Now we can update SBR, located in the two Baudrate memory;
     */

    //Copy the first 8 bits of SBR in BDL;
    registers->BDL = (uint8_t) SBR;

    //Clear the first 5 bits 00011111 (31) of BDH :
    CLEAR(registers->BDH, 31, 8);

    //Copy the 5 MSB of SBR in the 5 LSB of BDH;
    SET(registers->BDH, (SBR >> 8), 8);


    /*
    * Now we can update BRFA, located in the 5 first bits of C4;
    */

    //Clear the 5 first bits 00011111 (31) of C4;
    CLEAR(registers->C4, 31, 8);

    //Copy the new BRFA;
    SET(registers->C4, BRFA, 8);

}


/*
 * configure_state : enables or disables the receiver or the transmitter;
 */

void kinetis_UART_configure_state(kinetis_UART_data_t *data, UART_config_t *config) {

    //First, cache the data pointer to avoid permanent implicit double pointer access;
    kinetis_UART_memory_t *registers = data->memory;

    /*
     * Rx state : RxE is the bit 2 of C2;
     */

    //If rx must be enabled :
    if (config->rx_enabled) {

        //Set bit 2 of C2;
        SET(registers->C2, UART_C2_RE, 8);

    } else {

        //If rx must be disabled, clear bit 2 of C2;
        CLEAR(registers->C2, UART_C2_RE, 8);

    }


    /*
     * Tx state : TxE is the bit 3 of C2;
     */

    //If tx must be enabled :
    if (config->tx_enabled) {

        //Set the TE bit;
        SET(registers->C2, UART_C2_TE, 8);

    } else {

        //If rx must be disabled, clear bit 3 of C2;
        CLEAR(registers->C2, UART_C2_TE, 8);

    }

}


//-------------------------------- Interrupts enable functions -----------------------------------

/*
 * enable_reception_interrupt : enables the reception interrupts;
 */


void kinetis_UART_enable_rx_int(kinetis_UART_data_t *data) {

    //Set the RIE bit in C2 to enable the transmission interrupt;
    SET(data->memory->C2, UART_C2_RIE, 8);

}


/*
 * enable_transmission_interrupt : enables the transmission interrupts;
 */

void kinetis_UART_enable_tx_int(kinetis_UART_data_t *data) {

    //Set the TIE bit in C2 to enable the transmission interrupt;
    SET(data->memory->C2, UART_C2_TIE, 8);

}


//-------------------------- Transmission methods --------------------------

/*
 * transmission_available : this function will return true if a uint16_t can be transmitted to the UART;
 */

uint8_t kinetis_UART_tx_available(kinetis_UART_data_t *data) {

    //Return the maximum number of element minus the current number of elements present in the tx buffer;
    return ((uint8_t)data->tx_max - (uint8_t)data->memory->TCFIFO);

}


/*
 * transmit : this function is called during the interrupt routine;
 *
 *  It is called when the number of elements in the transmission buffer is lower (or equal) than the watermark.
 *
 *  This function must transfer the maximum amount of data from the software buffer to the hardware buffer;
 *
 *  If it succeeds to transfer all data, it disables the transmission interrupt;
 */

void kinetis_UART_transmit_all(kinetis_UART_data_t *data) {

    //As checking the packet mode takes more processing time than just send the 9-th bit, we won't check it.
    kinetis_UART_memory_t *registers = data->memory;

    //Cache the number of spaces in the hardware buffer;
    uint8_t hw_buffer_spaces = transmission_available();

    //If no data can be enqueued, complete;
    if (!hw_buffer_spaces)
        return;

    //Cache the number of elements in the software buffer;
    uint8_t sw_buffer_elements = transmissionBuffer->available_elements();

    //If no data has to be transmitted :
    if (!sw_buffer_elements) {

        //Disable the transmission interrupt;
        CLEAR(registers->C2,  UART_C2_TIE, 8);

        //Complete;
        return;

    }

    //For each element to transfer to the hardware buffer :
    for (;hw_buffer_spaces--;) {

        //If the current element is the last one that can be transferred, read S1 to clear the interrupt;
        if (!hw_buffer_spaces) {

            //TODO VOLATILE CHECK
            //Read S1;
            registers->S1;

        }

        //Get an element from the buffer;
        uint16_t element = transmissionBuffer->get_and_discard_output();

        //Decrease the number of elements contained in the buffer;
        sw_buffer_elements--;

        //If we must send the 9-th bit (ie the 9-th bit is set, b100000000 = 256
        if (element & (uint16_t) 256) {

            //Set T8, bit 6 of C3;
            SET(registers->C3, UART_C3_T8, 8);

        } else {

            //Set T8, bit 6 of C3;
            CLEAR(registers->C3, UART_C3_T8, 8);

        }

        //Now, copy the 8 first bits in D to complete the transmission;
        registers->D = (uint8_t) element;

        //If no more data has to be transmitted :
        if (!sw_buffer_elements) {

            //Disable the transmission interrupt;
            CLEAR(registers->C2,  UART_C2_TIE, 8);

            //Complete;
            return;

        }

    }

}


/*
 * transmit_break : this function is not supported for instance;
 */

void kinetis_UART_transmit_break() {}


//-------------------------- Reception methods --------------------------

/*
 * kinetis_UART_reception_available : determine how many chars are available;
 */

size_t kinetis_UART_reception_available(kinetis_UART_data_t *data) {

    //Cache the number of uint16_t in the rx buffer;
    return data->memory->RCFIFO;

}


/*
 * receive_all : this function is called during the interrupt routine;
 *
 *  It is called when the number of elements in the reception buffer is higher (or equal) than the watermark.
 *
 *  This function must transfer the maximum amount of data from the hardware buffer to the software buffer;
 *
 *  If it succeeds to fill the software buffer, it disables the transmission interrupt;
 */

void kinetis_UART_receive_all(kinetis_UART_data_t *data) {

    //To have a faster access, cache the register struct's pointer;
    kinetis_UART_memory_t *registers = data->memory;

    //Cache the number of uint16_t in the rx buffer;
    uint8_t reception_available = registers->RCFIFO;

    //If no data is available, complete;
    if (!reception_available)
        return;

    //Cache the number of elements that we can add to the reception buffer;
    uint8_t available_spaces = receptionBuffer->available_spaces();

    //If no space is available, complete;
    if (!available_spaces) {

        //De activate the reception interrupt;
        CLEAR(registers->C2,  UART_C2_RIE, 8);

        //Complete
        return;

    }

    //We will now read all possible elements from the rx buffer, except the last;
    for (; reception_available--;) {


        //If this cycle is the last one, read S1 to clear the interrupt;
        if (!reception_available) {

            //TODO VOLATILE CHECK
            //Read S1;
            registers->S1;


        }

        //First, start the return value;
        uint16_t ret_value = 0;

        //If the transmission mode is set to almost 9 bits and R8 is set :
        if ((registers->C1 & UART_C1_M) && (registers->C3 & UART_C3_R8)) {

            //Set the bit 8 of ret_value
            SET_BIT(ret_value, 8, 16);

        }

        //Add D's value to the return value;
        SET(ret_value, (uint16_t) (uint8_t) registers->D, 16);

        //Return the received value;
        receptionBuffer->insert_object(ret_value);

        //Decrease the number of spaces in the reception buffer;
        available_spaces--;

        //If the reception buffer is full :
        if (!available_spaces) {

            //De activate the reception interrupt;
            CLEAR(registers->C2,  UART_C2_RIE, 8);

            //Complete
            return;

        }

        //Now we are sure that the next cycle can happen;

    }

}


//-------------------------- Exceptions --------------------------

/*
 * interrupt : this function processes UART's interrupts : Rx, and Tx;
 */

void kinetis_UART_interrupt(kinetis_UART_data_t *data) {

    //First, cache the register pointer;
    kinetis_UART_memory_t *registers = data->memory;

    //The logical AND of C2 and S1;
    uint8_t masked_flags = registers->C2 & registers->S1;

    //TODO IDLE LINE;

    //TODO TRANSMISSION COMPLETE;

    //First, if the rx interrupt is enabled, and if its flag is asserted :
    if (masked_flags & UART_C2_RIE & UART_S1_RDRF) {

        //Receive as many data as possible;
        kinetis_UART_receive_all(data);

    }

    //Then, is the tx interrupt is enabled, and if its flag is asserted :
    if (masked_flags & UART_C2_TIE & UART_S1_TDRE) {

        //Transmit as many data as possible;
        transmit_all();

    }

}

/*
 * error : this function is called on an interrupt basis each time an error is detected by the hardware.
 *
 *  It supports different errors, that are described in the code below.
 */

void kinetis_UART_error(kinetis_UART_data_t *data) {

    /*
     * Errors supported are :
     *  - framing errors (locking) - read and forget last element;
     *  - receiver overrun (locking) - receiver overflow (same ones) -> receiver flush;
     *  - receiver underflow (misalignment) -> receiver flush;
     *  - transmitter overflow (misalignment) -> transmitter flush;
     *  - noise flag - parity error (informative) -> read and forget last element;
     */


    //First, cache the register pointer;
    kinetis_UART_memory_t *registers = data->memory;

    //First, start by unlocking reception by checking the framing error flag. At the same time, check noise or parity;
    if (registers->C2  & (UART_S1_FE | UART_S1_NF | UART_S1_PF)) {

        //In case of framing - parity - noise error, we read and discard;

        //TODO VOLATILE CHECK
        //Read D, to discard and clear flags;
        registers->D;

    }

    //Cache C2 (contains the state of tx and rx);
    uint8_t C2 = registers->C2;

    //If there was a receiver overrun or overflow :
    if ((registers->C2 & UART_S1_OR) | (registers->SFIFO & (UART_SFIFO_RXOF | UART_SFIFO_RXUF))) {

        //First, read D to clear the flag;

        //TODO VOLATILE CHECK
        //Read D, to discard and clear flags;
        registers->D;

        //Turn off rx;
        CLEAR(registers->C2, UART_C2_RE, 8);

        //Then, flush the rx fifo;
        registers->CFIFO |= UART_CFIFO_RXFLUSH;

        //Finally, clear RXOF and RXUF flags by writing a 1 to them
        registers->SFIFO = UART_SFIFO_RXOF | UART_SFIFO_RXUF;


    }

    //If there was a transmitter overflow :
    if (registers->SFIFO | UART_SFIFO_TXOF) {

        //Turn off tx;
        CLEAR(registers->C2, UART_C2_TE, 8);

        //Then, flush the tx fifo;
        registers->CFIFO |= UART_CFIFO_TXFLUSH;

        //Finally, clear the TXOF flag by writing a 1 to it;
        registers->SFIFO = UART_SFIFO_TXOF;

    }

    //Restore C2's previous state (will re-enable rx or tx if necessary;
    registers->C2 = C2;

}



//-------------------------- UARTS Definitions --------------------------
