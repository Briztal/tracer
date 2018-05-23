

#include "flux.h"

#include "kernel/kernel.h"


/*
 * flux_process : executes the cflux function;
 */

void flux_process(flux_t *flux) {

    //Cache the critical state of the cflux;
    bool critical = flux->critical;

    //If the cflux is critical, enter a critical section;
    if (critical) {
        kernel_enter_critical_section();
    }

    //If the data cflux has to be deleted :
    if (flux->state == (flux_state_t)CLOSED_FLUX) {
        goto end;
    }

    //Execute the data cflux function, passing its arguments;
    flux->flux_function(flux);

    end :

    //If the cflux was critical, leave the critical section;
    if (critical) {
        kernel_enter_critical_section();
    }

}


/*
 * data_flux_get_transfer_size : interrogates both receiver and transmitter, and returns
 *  the minimum of their max transfer size;
 */

size_t data_flux_get_transfer_size(flux_t *flux, void *tx_instance, void *rx_instance) {

    //Get the tx spaces_amount;
    size_t tx_size = flux->data_amount(tx_instance);

    //Get the rx spaces_amount;
    size_t rx_size = flux->spaces_amount(rx_instance);

    //Return the minimal size;
    return (tx_size < rx_size) ? tx_size : rx_size;

}
