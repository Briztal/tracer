//
// Created by root on 5/12/18.
//


#include "element_array_flux.h"
#include "connection.h"

#include <Kernel/kernel.h>


/*
 * data_flux_acopy_process : declares an array in memory, calls tx copy in it, and calls rx process in it;
 */

void elemeny_array_flux(connection_flux_t *flux_c) {

    //Cache the casted version of the flux;
    element_array_flux_t *flux = (element_array_flux_t *) flux_c;

    //Cache the previous node's instance;
    void *tx_instance = flux_get_previous_nodes_instance((connection_flux_t *) flux);

    //Cache the next node's instance;
    void *rx_instance = flux_get_next_nodes_instance((connection_flux_t *) flux);

    //Determine the transfer size;
    size_t transfer_size = data_flux_get_transfer_size((connection_flux_t *) flux, tx_instance, rx_instance);

    //Create an array of the appropriate size;
    void *memory = kernel_malloc(transfer_size * flux->flux.data_size);

    //Call the transmitter to fill the array;
    flux->copy_data_in(tx_instance, memory, transfer_size);

    //Call the receiver to process the array;
    flux->copy_data_from(rx_instance, memory, transfer_size);

    //Release heap memory;
    kernel_free(memory);

}

