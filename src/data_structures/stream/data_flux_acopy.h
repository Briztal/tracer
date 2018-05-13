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
    data_flux_t data_flux;

    //--------- Transmission ---------

    //The function for the transmitter to copy its data;
    void (*tx_copy)(void *tx_struct, void *data_array, size_t size);

    //The function for the transmitter to receive its data;
    void (*rx_process)(void *rx_struct, void *data_array, size_t size);

} data_flux_acopy;


//The data flux function for a copy stream;
void data_flux_acopy_process(linked_element_t *linked_element);

//The data flux function for a copy stream;
void data_flux_acopy_process_critical(linked_element_t *linked_element);


#endif //TRACER_COPY_STREAM_H
