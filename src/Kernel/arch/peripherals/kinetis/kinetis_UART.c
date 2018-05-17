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


//-------------------------------- Interrupts enable functions -----------------------------------

//Enable the reception interrupt
void enable_rx_int(kinetis_UART_data_t *);

//Enable the transmission interrupt
void enable_tx_int(kinetis_UART_data_t *);


//-------------------------- Error and interrupt --------------------------

//The interrupt function;
void interrupt(kinetis_UART_data_t *data);

//The error function;
void error(kinetis_UART_data_t *data);


//-------------------------- Reception methods --------------------------

//Transmit a uint16_t to the UART;
void read_rx(kinetis_UART_data_t *);


//Initialise the hardware;
void initialise_hardware(kinetis_UART_data_t *);

//Configure the packet format;
void configure_packet_format(kinetis_UART_data_t *data, UART_config_t *);

//Configure the state;
void configure_state(kinetis_UART_data_t *data, UART_config_t *);

//Configure the state;
void configure_modem(kinetis_UART_data_t *data, UART_config_t *);

//Configure the transmission layer;
void configure_transmission_layer(kinetis_UART_data_t *data, UART_config_t *);


//-------------------------- Configuration methods --------------------------

/*
 * configure_packet_format : this function configures the UART. Config is divided into several private functions
 *  that are all called;
 */

void kinetis_UART_init(kinetis_UART_data_t *data, UART_config_t *config) {

    //Initialise the hardware;//TODO IS IT THE CORRECT ORDER ?
    initialise_hardware(data);

    configure_packet_format(data, config);
    configure_state(data, config);
    configure_modem(data, config);
    configure_transmission_layer(data, config);

}


//TODO CORRECT FLUX, TXRXSIZES AND DATA INITIALISATION
//TODO CORRECT FLUX, TXRXSIZES AND DATA INITIALISATION
//TODO CORRECT FLUX, TXRXSIZES AND DATA INITIALISATION
//TODO CORRECT FLUX, TXRXSIZES AND DATA INITIALISATION
//TODO CORRECT FLUX, TXRXSIZES AND DATA INITIALISATION
//TODO CORRECT FLUX, TXRXSIZES AND DATA INITIALISATION
//TODO CORRECT FLUX, TXRXSIZES AND DATA INITIALISATION
//TODO CORRECT FLUX, TXRXSIZES AND DATA INITIALISATION
//TODO CORRECT FLUX, TXRXSIZES AND DATA INITIALISATION
//TODO CORRECT FLUX, TXRXSIZES AND DATA INITIALISATION
//TODO CORRECT FLUX, TXRXSIZES AND DATA INITIALISATION
//TODO CORRECT FLUX, TXRXSIZES AND DATA INITIALISATION
//TODO CORRECT FLUX, TXRXSIZES AND DATA INITIALISATION
//TODO CORRECT FLUX, TXRXSIZES AND DATA INITIALISATION
//TODO CORRECT FLUX, TXRXSIZES AND DATA INITIALISATION
//TODO CORRECT FLUX, TXRXSIZES AND DATA INITIALISATION
//TODO CORRECT FLUX, TXRXSIZES AND DATA INITIALISATION
//TODO CORRECT FLUX, TXRXSIZES AND DATA INITIALISATION
//TODO CORRECT FLUX, TXRXSIZES AND DATA INITIALISATION
//TODO CORRECT FLUX, TXRXSIZES AND DATA INITIALISATION
//TODO CORRECT FLUX, TXRXSIZES AND DATA INITIALISATION
//TODO CORRECT FLUX, TXRXSIZES AND DATA INITIALISATION
//TODO CORRECT FLUX, TXRXSIZES AND DATA INITIALISATION
//TODO CORRECT FLUX, TXRXSIZES AND DATA INITIALISATION
//TODO CORRECT FLUX, TXRXSIZES AND DATA INITIALISATION
//TODO CORRECT FLUX, TXRXSIZES AND DATA INITIALISATION
//TODO CORRECT FLUX, TXRXSIZES AND DATA INITIALISATION


/*
 * initialise_hardware : initialises the data pointer and the frequency;w
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


/*
 * Packet format configuration : sets :
 *  - The number of data bits;
 *  - The parity bit (enables of disables);
 *  - The parity type (Even or Odd) if it is enabled;
 */

void configure_packet_format(kinetis_UART_data_t *data, UART_config_t *config) {

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

void configure_modem(kinetis_UART_data_t *data, UART_config_t *config) {

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

void configure_transmission_layer(kinetis_UART_data_t *data, UART_config_t *config) {

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

void configure_state(kinetis_UART_data_t *data, UART_config_t *config) {

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
 * enable_rx_int : enables the reception interrupts;
 */

void enable_rx_int(kinetis_UART_data_t *data) {

    //Set the RIE bit in C2 to enable the transmission interrupt;
    SET(data->memory->C2, UART_C2_RIE, 8);

}


/*
 * enable_tx_int : enables the transmission interrupts;
 */

void enable_tx_int(kinetis_UART_data_t *data) {

    //Set the TIE bit in C2 to enable the transmission interrupt;
    SET(data->memory->C2, UART_C2_TIE, 8);

}


//-------------------------- Transmission methods --------------------------

/*
 * tx_available : this function will return true if a uint16_t can be transmitted to the UART;
 */

uint8_t kinetis_UART_tx_available(const void *data_c) {

    //Cache data in the correct type;
    kinetis_UART_data_t *data = (kinetis_UART_data_t *) data_c;

    //Return the maximum number of element minus the current number of elements present in the tx buffer;
    return ((uint8_t) data->tx_max - (uint8_t) data->memory->TCFIFO);

}


/*
 * update_tx_water :
 *
 *  - Enables the reception interrupt. If called by a connection, will allow automatic processing. If called by an
 *      interrupt, won't change anything;
 *  - Determines the safest number of transmissions to execute;
 *  - Determines the final number of elements in the transmission buffer;
 *  - Determines the final watermark;
 */

uint8_t update_tx_watermark(const kinetis_UART_data_t *const data, uint8_t nb_transmissions) {

    //To have a faster access, cache the register struct's pointer;
    kinetis_UART_memory_t *const registers = data->memory;

    //Activate the reception interrupt;
    SET(registers->C2, UART_C2_TIE, 8);

    //Cache the number of elements in the hardware buffer;
    const uint8_t hw_tx_nb_elements = registers->TCFIFO;

    //Cache the number of spaces in the hardware buffer;
    const uint8_t hw_tx_nb_spaces = data->tx_max - hw_tx_nb_elements;

    //If we must transmit more characters that we can, major the size to transfer;
    if (nb_transmissions > hw_tx_nb_spaces) nb_transmissions = hw_tx_nb_spaces;

    //Determine the final number of uint16_t in the hw buffer;
    const uint8_t final_nb = hw_tx_nb_elements + nb_transmissions;

    //Call the watermark specific function;
    registers->TWFIFO = data->tx_water_function(final_nb);

    //Return the correct number of transmissions to execute;
    return nb_transmissions;

}


/*
 * kinetis_UART_copy_to_tx : this function copies the required amount of data from the src pointer to
 *  the hardware tx buffer;
 *
 *  It calls for watermark update, updates its transfer size, and copies the required amount of data;
 *
 *  If the uint16_t it copies is the last, it reads S1 to clear the interrupt flag;
 */

void kinetis_UART_copy_to_tx(const void *const data_c, const void *src_c, const size_t size) {

    //Cache data in the correct type;
    const kinetis_UART_data_t *const instance = data_c;

    //Cache src in the correct type;
    const uint16_t *src = src_c;

    //As checking the packet mode takes more processing time than just send the 9-th bit, we won't check it.
    kinetis_UART_memory_t *const registers = instance->memory;

    //Update the watermark and determine the number of transmissions to execute;
    uint8_t nb_transmissions = update_tx_watermark(instance, (uint8_t) size);

    //For each element to transfer to the hardware buffer :
    while (nb_transmissions--) {

        //If the current element is the last one that can be transferred, read S1 to clear the interrupt;
        if (!nb_transmissions) {

            //TODO VOLATILE CHECK
            //Read S1;
            registers->S1;

        }

        //Get an element from the buffer;
        uint16_t element = *(src++);

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

    }

}


//-------------------------- Reception methods --------------------------

/*
 * kinetis_UART_rx_available : determine how many chars are available in the rx hardware buffer;
 */

size_t kinetis_UART_rx_available(const void *const data_c) {

    //Cache data in the correct type;
    kinetis_UART_data_t *data = (kinetis_UART_data_t *) data_c;

    //Cache the number of uint16_t in the rx buffer;
    return data->memory->RCFIFO;

}


/*
 * update_tx_water :
 *  - Enabled the reception interrupt;
 *  - Determines the safest maximum number of readings to perform;
 *  - Determine the final number of elements in the hardware rx buffer;
 *  - Ca
 *
 * his function updates the rx watermark in prevision of a determined number of readings.
 *  It also corrects and returns the number of readings so that there are no underflows;
 */

uint8_t update_rx_watermark(const kinetis_UART_data_t *const data, uint8_t nb_readings) {

    //To have a faster access, cache the register struct's pointer;
    kinetis_UART_memory_t *const registers = data->memory;

    //Activate the reception interrupt;
    SET(registers->C2, UART_C2_RIE, 8);

    //Cache the number of uint16_t in the rx hardware buffer;
    const uint8_t hw_rx_nb = registers->RCFIFO;

    //Major the transfer size by the number of uint16_t in the hardware buffer;
    if (hw_rx_nb < nb_readings) nb_readings = hw_rx_nb;

    //Determine the final number of uint16_t in the hw buffer;
    const uint8_t final_nb = hw_rx_nb - nb_readings;

    //Call the watermark specific function;
    registers->RWFIFO = data->rx_water_function(final_nb, data->rx_max);

    //Return the safest maxmimum number of readings to perform;
    return nb_readings;

}


/*
 * rx_watermark_strict : the strictest watermark policy, always increments of 1, interrupt triggered as soon as a
 *  uint16_t is received;
 */

uint8_t rx_watermark_strict(const uint8_t nb_elements, const uint8_t unused) {

    //Return the number of elements incremented of one;
    return nb_elements + (uint8_t) 1;

}

/*
 * rx_watermark_half : a less strict watermark policy, al latency is left to the buffer.
 *  This latency corresponds to the upper half of the number of spaces in the buffer;
 */

uint8_t rx_watermark_half(const uint8_t nb_elements, const uint8_t max_size) {

    //Return the number of elements incremented of the upper half remaining number of elements;
    return nb_elements + ((max_size + (uint8_t) 1 - nb_elements) >> 1);

}


/*
 * kinetis_UART_copy_from_rx : this function copies the required amount of uint16_t from the hardware buffer to the
 *  provided destination;
 *
 *  It calls for watermark update, updates its transfer size, and copies the required amount of data;
 *
 *  Before transferring the last uint16_t, it reads S1 to clear the interrupt flag;
 */

void kinetis_UART_copy_from_rx(const void *const data_c, void *dest_c, size_t size) {

    //Cache data in the correct type;
    const kinetis_UART_data_t *const instance = data_c;

    //Cache src in the correct type;
    uint16_t *dest = dest_c;

    //To have a faster access, cache the register struct's pointer;
    kinetis_UART_memory_t *const registers = instance->memory;

    //Update the rw watermark according to the transfer size, and get the safe number of readings to do;
    uint8_t nb_readings = update_rx_watermark(instance, (uint8_t) size);

    //For each element to transfer from the hw buffer :
    while (nb_readings--) {

        //If this cycle is the last one, read S1 to clear the interrupt;
        if (!nb_readings) {

            //TODO VOLATILE CHECK
            //Read S1;
            registers->S1;

        }

        //First, cache the return value;
        uint16_t ret_value = 0;

        //If the transmission mode is set to almost 9 bits and R8 is set :
        if ((registers->C1 & UART_C1_M) && (registers->C3 & UART_C3_R8)) {

            //Set the bit 8 of ret_value
            SET_BIT(ret_value, 8, 16);

        }

        //Add D's value to the return value;
        SET(ret_value, (uint16_t) (uint8_t) registers->D, 16);

        //Return the received value;
        *(dest++) = ret_value;

    }

}


//-------------------------- Exceptions --------------------------

/*
 * interrupt : this function processes UART's interrupts : Rx, and Tx;
 */

void kinetis_UART_interrupt(const kinetis_UART_data_t *const data) {

    //First, cache the register pointer;
    kinetis_UART_memory_t *registers = data->memory;

    //The logical AND of C2 and S1;
    uint8_t masked_flags = registers->C2 & registers->S1;

    //TODO IDLE LINE;

    //TODO TRANSMISSION COMPLETE;

    //First, if the rx interrupt is enabled, and if its flag is asserted :
    if (masked_flags & UART_C2_RIE & UART_S1_RDRF) {

        //Execute the reception flux;
        flux_process(data->rx_flux);

        //If the interrupt is still active, the flux didn't succeed in transferring data;
        if (registers->S1 & UART_S1_RDRF) {

            /*
             * The flux had no data to proceed in spite of spaces in the transmission buffer;
             * There is no data available in the other side of the flux, and only an external call will provide some;
             * There is no reason to keep the interrupt active. We will disable the interrupt;
             *
             * We will keep the watermark like it is currently, if the interrupt is reactivated by mistake (lol)
             *  it could provide some data;
             */

            //Clear RIE bit of uint8_t C2 to disable the reading interrupt;
            CLEAR_BIT(registers->C2, UART_C2_RIE, 8);

        }

    }

    //Then, is the tx interrupt is enabled, and if its flag is asserted :
    if (masked_flags & UART_C2_TIE & UART_S1_TDRE) {

        //Execute the transmission flux;
        flux_process(data->tx_flux);

        //If the interrupt is still active, the stream didn't succeed in transferring data;
        if (registers->S1 & UART_S1_TDRE) {
/*
             * The flux had no data to proceed in spite of elements in the reception buffer;
             * There is no data available in the other side of the flux, and only an external call will provide some;
             * There is no reason to keep the interrupt active. We will disable the interrupt;
             *
             * We will keep the watermark like it is currently, if the interrupt is reactivated by mistake (lol)
             *  it could provide some data;
             */

            //Clear RIE bit of uint8_t C2 to disable the reading interrupt;
            CLEAR_BIT(registers->C2, UART_C2_TIE, 8);

        }

    }

}


/*
 * error : this function is called on an interrupt basis each time an error is detected by the hardware.
 *
 *  It supports different errors, that are described in the code below.
 */

void kinetis_UART_error(const kinetis_UART_data_t *const data) {

    /*
     * Errors supported are :
     *  - framing errors (locking) - read and forget last element;
     *  - receiver overrun (locking) - receiver overflow (same ones) -> receiver flush;
     *  - receiver underflow (misalignment) -> receiver flush;
     *  - transmitter overflow (misalignment) -> transmitter flush;
     *  - noise flag - parity error (informative) -> read and forget last element;
     */

    //TODO MANAGE FLUX CORRECTLY HERE
    //TODO MANAGE FLUX CORRECTLY HERE
    //TODO MANAGE FLUX CORRECTLY HERE
    //TODO MANAGE FLUX CORRECTLY HERE
    //TODO MANAGE FLUX CORRECTLY HERE
    //TODO MANAGE FLUX CORRECTLY HERE
    //TODO MANAGE FLUX CORRECTLY HERE
    //TODO MANAGE FLUX CORRECTLY HERE
    //TODO MANAGE FLUX CORRECTLY HERE
    //TODO MANAGE FLUX CORRECTLY HERE
    //TODO MANAGE FLUX CORRECTLY HERE
    //TODO MANAGE FLUX CORRECTLY HERE
    //TODO MANAGE FLUX CORRECTLY HERE
    //TODO MANAGE FLUX CORRECTLY HERE
    //TODO MANAGE FLUX CORRECTLY HERE
    //TODO MANAGE FLUX CORRECTLY HERE

    //First, cache the register pointer;
    kinetis_UART_memory_t *registers = data->memory;

    //First, start by unlocking reception by checking the framing error flag. At the same time, check noise or parity;
    if (registers->C2 & (UART_S1_FE | UART_S1_NF | UART_S1_PF)) {

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
