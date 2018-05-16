//
// Created by root on 5/12/18.
//

#include "rx_from_tx_flux.h"

#include <Kernel/kernel.h>


void rx_from_tx_flux(connection_flux_t *flux_c) {

    //Cache the casted version of the flux;
    rx_from_tx_flux_t *flux = (rx_from_tx_flux_t *) flux_c;

    //Cache the previous node's instance;
    void *tx_instance = flux_get_previous_nodes_instance((connection_flux_t *) flux);

    //Cache the next node's instance;
    void *rx_instance = flux_get_next_nodes_instance((connection_flux_t *) flux);

    //Determine the transfer size;
    size_t transfer_size = data_flux_get_transfer_size((connection_flux_t *) flux, tx_instance, rx_instance);

    //Cache the output pointer;
    void *tx_ptr = flux->tx_get_ptr(tx_instance);

    //Transfer the pointer to the receiver;
    flux->copy_data_from(rx_instance, tx_ptr, transfer_size);

    //Notify the transmitter that [transfer_size] elements have been transferred;
    flux->tx_discard_data(tx_instance, transfer_size);

}
