//
// Created by root on 5/12/18.
//

#include "data_flux.h"

#include <Kernel/kernel.h>


//Header for data flux function;
void data_flux(cflux_t *);

/*
 * array_flux_create : creates an array flux in the heap;
 */

data_flux_t *data_flux_create(bool critical, size_t element_size,
                                                size_t (*data_amount)(void *),
                                                size_t (*spaces_amount)(void *),
                                                data_processor tx_copier,
                                                data_processor rx_copier) {

    //Allocate some space for the flux;
    data_flux_t *flux = kernel_malloc(sizeof(data_flux_t));

    //Initialise the cflux base;
    cflux_initialise((cflux_t *) flux, critical, element_size, data_flux, data_amount, spaces_amount);

    //Initialise the array flux base;
    flux->tx_copier = tx_copier;
    flux->rx_copier = rx_copier;

    //Return the initialised flux;
    return flux;

}


void data_flux(cflux_t *flux_c) {

    //Cache the casted version of the flux;
    data_flux_t *flux = (data_flux_t *) flux_c;

    //Cache the previous node's instance;
    void *tx_instance = flux_get_previous_nodes_instance((cflux_t *) flux);

    //Cache the next node's instance;
    void *rx_instance = flux_get_next_nodes_instance((cflux_t *) flux);

    //Determine the transfer size;
    size_t transfer_size = data_flux_get_transfer_size((cflux_t *) flux, tx_instance, rx_instance);

    //Create an aligned memory region of the appropriate size. Can't be allocated on the stack, would be misaligned;
    void *memory = kernel_malloc(flux->flux.data_size);

    //For each piece of data to transfer :
    for (;transfer_size--;) {

        //Call the transmitter to fill the array;
        flux->tx_copier(tx_instance, memory);

        //Call the receiver to process the array;
        flux->rx_copier(rx_instance, memory);

    }

    //Release heap memory;
    kernel_free(memory);

}