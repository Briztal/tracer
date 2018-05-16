//
// Created by root on 5/12/18.
//

#ifndef TRACER_TRANSFERT_STREAM_H
#define TRACER_TRANSFERT_STREAM_H

#include "connection.h"

#include <stddef.h>

typedef struct {

    //--------- Composition ---------

    //The stream structure;
    connection_flux_t stream;

    //--------- Transmission ---------

    //The transmitter copies its data in the array;
    void (*copy_data_in)(void *tx_instance, void *data_pointer, size_t size);


    //--------- Reception ---------

    //Get a pointer to rx's data;
    void *(*rx_get_ptr)(void *tx_struct);

    //Discard a certain number of transmitted elements;
    void (*rx_discard_data)(void *tx_struct, size_t transfer_size);


} tx_from_rx_flux_t;


//The data transfer function for transfer streams;
void tx_from_rx_flux(connection_flux_t *flux);


#endif //TRACER_TRANSFERT_STREAM_H
