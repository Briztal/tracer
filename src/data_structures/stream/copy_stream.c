//
// Created by root on 5/12/18.
//

#include "copy_stream.h"

#include <Kernel/kernel.h>


void copy_stream_data_flux(linked_element_t *linked_element) {

    //Cache the copy stream in the correct type;
    copy_stream_t *stream = (copy_stream_t*) linked_element;

    //Determine the transfer size;
    size_t transfer_size = data_flux_get_transfer_size((connection_flux_t *) stream);

    //Create an aligned memory region of the appropriate size. Can't be allocated on the stack, would be misaligned;
    void *memory = kernel_malloc(stream->stream.data_size);

    //For each piece of data to transfer :
    for (;transfer_size--;) {

        //Call the transmitter to fill the array;
        stream->tx_push(stream->stream.tx_struct, memory);

        //Call the receiver to process the array;
        stream->rx_pull(stream->stream.rx_struct, memory);

    }

    //Release heap memory;
    kernel_free(memory);

}


void critical_element_flux(linked_element_t *linked_element) {

    //Enter a critical section;
    kernel_enter_critical_section();

    //Process data;
    copy_stream_data_flux(linked_element);

    //End the critical section;
    kernel_leave_critical_section();

}