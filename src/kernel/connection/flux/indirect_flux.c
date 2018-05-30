//
// Created by root on 5/12/18.
//

#include "indirect_flux.h"
#include "flux.h"

#include <kernel/kernel.h>


//Header for tx passive cflux
void indirect_flux(flux_t *flux_c);

//Construct an indirect cflux where the receiver is passive;
indirect_flux_t *indirect_flux_create_tx_passive(bool critical, size_t element_size,
                                                 size_t (*data_amount)(void *),
                                                 size_t (*spaces_amount)(void *),
                                                 indirect_accessor_t tx_accessor,
                                                 array_processor rx_copier) {

    //Allocate some space for the cflux;
    indirect_flux_t *flux = kernel_mallocc(sizeof(indirect_flux_t));

    //Initialise the cflux base;
    cflux_initialise((flux_t *) flux, critical, element_size, indirect_flux, data_amount, spaces_amount);

    //The tx side is passive, and the rx size is active
    flux->tx_passive = true;
    flux->indirect_accessor = tx_accessor;
    flux->copier = rx_copier;

    //Return the scheduler_initialised cflux;
    return flux;

}

//Construct an indirect cflux where the receiver is passive;
indirect_flux_t *indirect_flux_create_rx_passive(bool critical, size_t element_size,
                                                 size_t (*data_amount)(void *),
                                                 size_t (*spaces_amount)(void *),
                                                 array_processor tx_copier,
                                                 indirect_accessor_t rx_accessor) {

    //Allocate some space for the cflux;
    indirect_flux_t *flux = kernel_mallocc(sizeof(indirect_flux_t));

    //Initialise the cflux base;
    cflux_initialise((flux_t *) flux, critical, element_size, indirect_flux, data_amount, spaces_amount);

    //The rx side is passive, and the tx size is active
    flux->tx_passive = false;
    flux->indirect_accessor = rx_accessor;
    flux->copier = tx_copier;

    //Return the scheduler_initialised cflux;
    return flux;
}


/*
 * indirect_tx_passive_flux : data cflux for indirect processor, where tx is passive.
 *
 *  As tx is passive, we must get its data pointer, let rx process its data, and notify tx
 *  that data has been processed;
 */

void indirect_flux(flux_t *flux_c) {

    //Cache the casted version of the cflux;
    indirect_flux_t *flux = (indirect_flux_t *) flux_c;

    //Cache the previous node's instance;
    void *tx_instance = flux_get_previous_nodes_instance((flux_t *) flux);

    //Cache the next node's instance;
    void *rx_instance = flux_get_next_nodes_instance((flux_t *) flux);

    //Determine the transfer size;
    size_t transfer_size = data_flux_get_transfer_size((flux_t *) flux, tx_instance, rx_instance);

    //Declare passive and active instances;
    void *passive_instance, *active_instance;

    //Initialise passive and active_instance;
    if (flux->tx_passive) {
        passive_instance = tx_instance, active_instance = rx_instance;
    } else {
        passive_instance = rx_instance, active_instance = tx_instance;
    }

    //Cache the passive side's pointer;
    void *passive_ptr = flux->indirect_accessor.get_ptr(passive_instance);

    //Let the active side process its data from the passive side's pointer;
    flux->copier(active_instance, passive_ptr, transfer_size);

    //Notify the passive side that some of its data has been processed;
    flux->indirect_accessor.data_processed(passive_instance, transfer_size);

}





