//
// Created by root on 5/12/18.
//


#include "array_flux.h"
#include "Kernel/connection/connection.h"

#include <Kernel/kernel.h>

//Flux function header
void element_array_flux(flux_t *);


/*
 * array_flux_create : creates an array cflux in the heap;
 */

array_flux_t *array_flux_create(bool critical, size_t element_size,
                                size_t (*data_amount)(void *),
                                size_t (*spaces_amount)(void *),
                                array_processor tx_copier,
                                array_processor rx_copier) {

    //Allocate some space for the cflux;
    array_flux_t *flux = kernel_mallocc(sizeof(array_flux_t));

    //Initialise the cflux base;
    cflux_initialise((flux_t *) flux, critical, element_size, element_array_flux, data_amount, spaces_amount);

    //Initialise the array cflux base;
    flux->tx_copier = tx_copier;
    flux->rx_copier = rx_copier;

    //Return the initialised cflux;
    return flux;

}


/*
 * data_flux_acopy_process : declares an array in memory, calls tx copy in it, and calls rx process in it;
 */

void element_array_flux(flux_t *flux_c) {

    //Cache the casted version of the cflux;
    array_flux_t *flux = (array_flux_t *) flux_c;

    //Cache the previous node's instance;
    void *tx_instance = flux_get_previous_nodes_instance((flux_t *) flux);

    //Cache the next node's instance;
    void *rx_instance = flux_get_next_nodes_instance((flux_t *) flux);

    //Determine the transfer size;
    size_t transfer_size = data_flux_get_transfer_size((flux_t *) flux, tx_instance, rx_instance);

    //Create an array of the appropriate size;
    void *memory = kernel_mallocc(transfer_size * flux->flux.data_size);

    //Call the transmitter to fill the array;
    flux->tx_copier(tx_instance, memory, transfer_size);

    //Call the receiver to process the array;
    flux->rx_copier(rx_instance, memory, transfer_size);

    //Release heap memory;
    kernel_free(memory);

}

