//
// Created by root on 5/12/18.
//


#include "data_flux_acopy.h"

#include <Kernel/kernel.h>


/*
 * data_flux_acopy_process : declares an array in memory, calls tx copy in it, and calls rx process in it;
 */

void data_flux_acopy_process(linked_element_t *linked_element) {

    //Cache the copy stream in the correct type;
    data_flux_acopy *stream = (data_flux_acopy*) linked_element;

    //Determine the transfer size;
    size_t transfer_size = stream_get_flux_size((data_flux_t *)stream);

    //Create an array of the appropriate size;
    void *memory = kernel_malloc(transfer_size * stream->stream.element_size);

    //Call the transmitter to fill the array;
    stream->tx_copy(stream->stream.tx_struct, memory, transfer_size);

    //Call the receiver to process the array;
    stream->rx_process(stream->stream.rx_struct, memory, transfer_size);

    //Release heap memory;
    kernel_free(memory);

}


/*
 * acopy_stream_data_flux : the function for a critical stream (callable via interrupt);
 */

void acopy_stream_data_flux(linked_element_t *linked_element) {

    //Enter a critical section;
    kernel_enter_critical_section();

    //Process data;
    data_flux_acopy_process(linked_element);

    //Leave the critical section;
    kernel_leave_critical_section();

}

/*
 * acopy_stream_data_flux_critical : the function for a critical stream (callable via interrupt);
 */

void acopy_stream_data_flux_critical(linked_element_t *linked_element) {

    //Enter a critical section;
    kernel_enter_critical_section();

    //Process data;
    data_flux_acopy_process(linked_element);

    //Leave the critical section;
    kernel_leave_critical_section();

}
