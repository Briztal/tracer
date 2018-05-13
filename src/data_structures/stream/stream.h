//
// Created by root on 5/11/18.
//

#ifndef TRACER_STREAM_H
#define TRACER_STREAM_H

#include <stddef.h>

#include "DataStructures/containers/llist.h"

struct data_flux_t;


typedef struct {

    //The instance pointer;
    void *instance;

    //The instance's destructor;
    void (*destructor)(void *);

    //The eventual input data flux;
    struct data_flux_t *input_flux;

    //The eventual output flux;
    struct data_flux *output_flux;

} data_processor_t;



typedef struct {

    //--------- Size determination part ---------;

    //The transmitter size query function;
    size_t (*tx_size)(void *tx_struct);

    //The receiver size query function;
    size_t (*rx_size)(void *rx_struct);


    //--------- Data flux function ---------;

    //The size of an element that transmits;
    size_t element_size;

    //Move data; Will take a struct in entry, must be casted;
    void (*data_flux)(linked_element_t *);


    //--------- Links ---------;

    //The previous data processor;
    data_processor_t *tx_processor;

    //The next data processor;
    data_processor_t *rx_processor;


} data_flux_t;


typedef struct {

    //The stream's first data processor;
    data_processor_t *first_processor;

    //The stream's last data processor;
    data_processor_t *last_processor;

    //The number of data flux in the stream;
    size_t nb_data_flux;

} stream_t;


#define EMPTY_STREAM(size, data_flux_function) {\
    .rx_struct = 0, .rx_size = stream_null_size, .rx_pull = stream_empty_pull,\
    .tx_struct = 0, .tx_size = stream_null_size, .tx_push = stream_empty_push,\
    .element_size = (size), .data_flux = (data_flux_function)};



//Size determination;
size_t stream_get_flux_size(data_flux_t *, void *tx_instance, void *rx_instance);

//Create a data processor from its base parameters;
data_processor_t *stream_create_data_processor(void *instance, void (*destructor)(void *));

//Create a stream from a heap allocated data processor;
stream_t *stream_create(data_processor_t *first_processor);

//Add a data processor to an existing stream;
void stream_add_data_processor(stream_t *stream, data_flux_t *data_flux, data_processor_t *data_processor);


#endif //TRACER_STREAM_H
