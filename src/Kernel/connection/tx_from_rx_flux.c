//
// Created by root on 5/12/18.
//

#include "tx_from_rx_flux.h"

#include <Kernel/kernel.h>

void tx_from_rx_flux(connection_flux_t *flux_c) {

    //Cache the casted version of the flux;
    tx_from_rx_flux_t *flux = (tx_from_rx_flux_t *) flux_c;

    //Cache the previous node's instance;
    void *tx_instance = flux_get_previous_nodes_instance((connection_flux_t *) flux);

    //Cache the next node's instance;
    void *rx_instance = flux_get_next_nodes_instance((connection_flux_t *) flux);

    //Determine the transfer size;
    size_t transfer_size = data_flux_get_transfer_size((connection_flux_t *) flux, tx_instance, rx_instance);

    //Cache the output pointer;
    void *rx_ptr = flux->rx_get_ptr(tx_instance);

    //Transfer the pointer to the receiver;
    flux->copy_data_in(rx_instance, rx_ptr, transfer_size);

    //Notify the transmitter that [transfer_size] elements have been transferred;
    flux->rx_discard_data(tx_instance, transfer_size);

}
