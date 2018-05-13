//
// Created by root on 5/12/18.
//

#ifndef TRACER_COPY_STREAM_H
#define TRACER_COPY_STREAM_H

#include "stream.h"

/*
 * The copy stream is a stream composition, that copies data it transmits.
 *
 *  Its data flux function determines the amount of data to transit, declares an array of the appropriate size, and
 *  passes it to the transmitter, that copies its data in it.
 *
 *  Then, it passes the array to the receiver that processes it;
 */

typedef struct {

    //--------- Composition ---------

    //The stream structure;
    data_flux_t stream;


    //--------- Transmission ---------

    //The function for the transmitter to copy its data;
    void (*tx_push)(void *tx_struct, void *data_array);

    //The function for the transmitter to receive its data;
    void (*rx_pull)(void *rx_struct, void *data_array);

} copy_stream_t;


#define EMPTY_COPY_STREAM(size, tx_push_f, rx_pull_f) {\
    EMPTY_STREAM(size, copy_stream_data_flux), .tx_push = (tx_push_f), .rx_pull = (rx_pull_f)}

#define EMPTY_CRITICAL_COPY_STREAM(size, tx_push_f, rx_pull_f) {\
    EMPTY_STREAM(size, copy_stream_data_flux_critical), .tx_push = (tx_push_f), .rx_pull = (rx_pull_f)}


//The data flux function for a copy stream;
void copy_stream_data_flux(linked_element_t *linked_element);

//The data flux function for a copy stream;
void copy_stream_data_flux_critical(linked_element_t *linked_element);


#endif //TRACER_COPY_STREAM_H
