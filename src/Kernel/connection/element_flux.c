//
// Created by root on 5/12/18.
//

#include "element_flux.h"

#include <Kernel/kernel.h>


void element_flux(connection_flux_t *flux_c) {

    //Cache the casted version of the flux;
    element_flux_t *flux = (element_flux_t *) flux_c;

    //Cache the previous node's instance;
    void *tx_instance = flux_get_previous_nodes_instance((connection_flux_t *) flux);

    //Cache the next node's instance;
    void *rx_instance = flux_get_next_nodes_instance((connection_flux_t *) flux);

    //Determine the transfer size;
    size_t transfer_size = data_flux_get_transfer_size((connection_flux_t *) flux, tx_instance, rx_instance);

    //Create an aligned memory region of the appropriate size. Can't be allocated on the stack, would be misaligned;
    void *memory = kernel_malloc(flux->flux.data_size);

    //For each piece of data to transfer :
    for (;transfer_size--;) {

        //Call the transmitter to fill the array;
        flux->tx_push(tx_instance, memory);

        //Call the receiver to process the array;
        flux->rx_pull(rx_instance, memory);

    }

    //Release heap memory;
    kernel_free(memory);

}




/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!
/////TODO : PROCESS FUNCTIONS QUERY FROM DRIVERS !!!!!!!!!!!!