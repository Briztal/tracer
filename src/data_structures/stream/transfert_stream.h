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
    void (*rx_process_data)(void *rx_struct, void *data_pointer, size_t size);

} transfer_stream_t;


#define EMPTY_TRANSFER_STREAM(size, tx_get, tx_discard, rx_process) {\
    EMPTY_STREAM(size, transfer_stream_data_flux),\
    .tx_get_ptr = (tx_get), .tx_discard_data = (tx_discard), .rx_process_data = (rx_process)\}


#define EMPTY_CRITICAL_TRANSFER_STREAM(size, tx_get, tx_discard, rx_process) {\
    EMPTY_STREAM(size, transfer_stream_data_flux_critical),\
    .tx_get_ptr = (tx_get), .tx_discard_data = (tx_discard), .rx_process_data = (rx_process)\}


//The data transfer function for transfer streams;
void transfer_stream_data_flux(linked_element_t *linked_element);

//The data transfer function for critical transfer streams;
void transfer_stream_data_flux_critical(linked_element_t *linked_element);

#endif //TRACER_TRANSFERT_STREAM_H
