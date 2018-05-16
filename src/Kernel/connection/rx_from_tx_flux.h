//
// Created by root on 5/12/18.
//

#ifndef TRACER_TRANSFERT_STREAM_H
#define TRACER_TRANSFERT_STREAM_H

#include "connection.h"

typedef struct {

    //--------- Composition ---------

    //The stream structure;
    connection_flux_t stream;


    //--------- Transmission ---------

    //Get a pointer to rx's data;
    void *(*tx_get_ptr)(void *tx_struct);

    //Discard a certain number of transmitted elements;
    void (*tx_discard_data)(void *tx_struct, size_t transfer_size);

    //Pass the data pointer to rx;
    void (*copy_data_from)(void *rx_struct, void *data_pointer, size_t size);

} rx_from_tx_flux_t;


//The data transfer function for transfer streams;
void rx_from_tx_flux(connection_flux_t *flux);


#endif //TRACER_TRANSFERT_STREAM_H
