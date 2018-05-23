//
// Created by root on 5/12/18.
//

#ifndef TRACER_COPY_STREAM_H
#define TRACER_COPY_STREAM_H

#include "Kernel/connection/connection.h"

/*
 * The copy stream is a stream composition, that copies data it transmits.
 *
 *  Its data cflux function determines the amount of data to transit, declares an array of the appropriate size, and
 *  passes it to the transmitter, that copies its data in it.
 *
 *  Then, it passes the array to the receiver that processes it;
 */

typedef struct {

    //--------- Composition ---------

    //The stream structure;
    flux_t flux;


    //--------- Flux functions ---------

    //The function for the transmitter to copy its data;
    data_processor tx_copier;

    //The function for the receiver to receive its data;
    data_processor rx_copier;


} data_flux_t;


//Create an array fux in the heap;
data_flux_t *data_flux_create(bool critical, size_t element_size,
                                                size_t (*data_amount)(void *),
                                                size_t (*spaces_amount)(void *),
                                                data_processor tx_copy,
                                                data_processor rx_receive);



#endif //TRACER_COPY_STREAM_H
