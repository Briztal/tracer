//
// Created by root on 5/12/18.
//

#include "transfert_stream.h"

#include <Kernel/kernel.h>


void transfer_stream_data_flux(linked_element_t *linked_element) {

    //Cache the copy stream in the correct type;
    transfer_stream_t *stream = (transfer_stream_t*) linked_element;

    //Determine the transfer size;
    size_t transfer_size = stream_get_flux_size((data_flux_t *)stream);

    //Cache the output pointer;
    void *tx_ptr = stream->tx_get_ptr(stream->stream.tx_struct);

    //Transfer the pointer to the receiver;
    stream->rx_process_data(stream->stream.rx_struct, tx_ptr, transfer_size);

    //Notify the transmitter that [transfer_size] elements have been transferred;
    stream->tx_discard_data(stream->stream.tx_struct, transfer_size);

}


void transfer_stream_data_flux_critical(linked_element_t *linked_element) {

    //Enter a critical section;
    kernel_enter_critical_section();

    //Process data;
    transfer_stream_data_flux(linked_element);

    //End the critical section;
    kernel_leave_critical_section();

}
