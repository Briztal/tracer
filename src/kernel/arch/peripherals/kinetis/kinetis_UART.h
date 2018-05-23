//
// Created by root on 4/19/18.

//TODO COPYRIGHT PJRC.COM LLC EN PLUS
//

#ifndef TRACER_KINETIS_UART_H
#define TRACER_KINETIS_UART_H

#include <kernel/drivers/UART.h>

#include <kernel/connection/connection.h>

/*
 * First, we define the UART memory map. As all data related to a specific UART are in a continuous memory zone,
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

} kinetis_UART_memory_t;


/*
 * Then, we define the UART data structure, that will contain a memory map, and a set of hardware constants;
 */

typedef struct {

    //The memory struct;
    kinetis_UART_memory_t *const memory;

    //The clock frequency;
    const uint32_t clockFrequency;

    //The status interrupt index;
    const uint8_t status_interrupt_id;

    //The error interrupt index;
    const uint8_t error_interrupt_id;

    //The status interrupt function;
    void (*const status_interrupt_function)(void);

    //The error interrupt function;
    void (*const error_interrupt_function)(void);

    //The maximum number of uint16_t in the rx hardware buffer;
    const uint8_t rx_size;

    //The maximum number of uint16_t in the tx hardware buffer;
    const uint8_t tx_size;

    //The rx_watermark update function;
    uint8_t (*rx_water_function)(const uint8_t nb_elements, const uint8_t rx_max);

    //The rx_watermark update function;
    uint8_t (*tx_water_function)(const uint8_t nb_elements);

    //The data cflux to receive from the UART; Will interact with rx;
    flux_t *rx_flux;

    //The data cflux to transmit to the UART; Will interact with tx;
    flux_t *tx_flux;

} kinetis_UART_data_t;


/*
 * The kinetis UART integrates a hardware buffer for both rx and tx canals, whose sizes depend on the module;
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

//-------------------------- Configuration methods --------------------------


/*
 * Init, start and exit functions : public to allow the UART driver creation;
 */

//Initialise the UART;
void kinetis_UART_init(void *data, const driver_config_t *);

//Start the UART;
void kinetis_UART_start(const void *data);

//De-initialise the UART;
void kinetis_UART_exit(const void *data);



/*
 * size functions : public to allow the UART driver creation;
 */

//Get the number of spaces in the tx hw buffer;
size_t kinetis_UART_tx_available(const void *data);

//Get the number of chars in the rx hw buffer;
size_t kinetis_UART_rx_available(const void * data);



/*
 * cflux functions : public to allow the UART driver creation;
 */

//Pass a certain amount of to tx;
void kinetis_UART_copy_to_tx(const void *data, void *src, size_t size);

//Get a certain amount of data from rx;
void kinetis_UART_copy_from_rx(const void *data, void *dest, size_t size);

/*
 * Exception functions : public to allow the creation of the interrupt link;
 */

//The interrupt function;
void kinetis_UART_status_interrupt(const kinetis_UART_data_t *data);

//The error function;
void kinetis_UART_error_interrupt(const kinetis_UART_data_t *data);


/*
 * -------------------------------- Driver construction --------------------------------
 */


typedef enum {
    size_1 = 1,
    size_4 = 4,
    size_8 = 8,
    size_16 = 16,
    size_32 = 32,
    size_64 = 64,
    size_128 = 128,
} kinetis_UART_fifo_size_t;


/*
 * Then, we define the UART data structure, that will contain a memory map, and a set of hardware constants;
 */

#define KINETIS_UART_DATA(mem_p, clock_f, rsize, tsize, status_int_id, error_int_id, status_int_f, error_int_f) {\
    .memory = (kinetis_UART_memory_t *const )(mem_p),\
    .clockFrequency = (clock_f),\
    .rx_size = (rsize),\
    .tx_size = (tsize), \
    .status_interrupt_id = (status_int_id),\
    .error_interrupt_id = (error_int_id),\
    .status_interrupt_function = (status_int_f),\
    .error_interrupt_function = (error_int_f),\
    .rx_water_function = 0,\
    .tx_water_function = 0,\
    .tx_flux = 0,\
    .rx_flux = 0,\
    }


/*
 * KINETIS_UART_FUNCTIONS : declares and defines the couple of interrupt functions related to the required UART;
 */

#define KINETIS_UART_FUNCTIONS(id)\
    void UART_##id##_data##_status_int() {kinetis_UART_status_interrupt(&UART_##id##_data);}\
    void UART_##id##_data##_error_int() {kinetis_UART_error_interrupt(&UART_##id##_data);}


/*
 * KINETIS_UART_DEFINE :
 *  - Declares the UART data structure;
 *  - Declares and defines interrupt functions;
 *  - Defines the UART data structure;
 *  - Defines the UART driver struct;
 */

#define KINETIS_UART_DEFINE(id, mem_p, clock_f, rsize, tsize, status_int_id, error_int_id)\
    extern kinetis_UART_data_t UART_##id##_data;\
    KINETIS_UART_FUNCTIONS(id);\
    kinetis_UART_data_t UART_##id##_data = KINETIS_UART_DATA(mem_p, clock_f, rsize, tsize, status_int_id, error_int_id, UART_##id##_data##_status_int, UART_##id##_data##_error_int);\
    UART_driver_t UART_##id = UART_DRIVER((void *)&UART_##id##_data, kinetis_UART_init, kinetis_UART_start, kinetis_UART_exit, kinetis_UART_tx_available, kinetis_UART_rx_available, &kinetis_UART_copy_to_tx, &kinetis_UART_copy_from_rx);


#endif //TRACER_TEENSY35_UART_H
