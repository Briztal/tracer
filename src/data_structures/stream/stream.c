//
// Created by root on 5/11/18.
//

#include "stream.h"

/*
 * stream_get_flux_size : interrogates both receiver and transmitter, and returns the minimum of their max size;
 */

size_t stream_get_flux_size(data_flux_t *stream, void *tx_instance, void *rx_instance) {

    //Get the tx rx_size;
    size_t tx_size = stream->tx_size(tx_instance);

    //Get the rx rx_size;
    size_t rx_size = stream->rx_size(rx_instance);

    //Return the minimal size;
    return (tx_size < rx_size) ? tx_size : rx_size;

}

