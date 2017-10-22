//
// Created by root on 10/10/17.
//

#ifdef suus

#ifndef TRACER_EEPROMINTERFACE_H
#define TRACER_EEPROMINTERFACE_H


class EEPROMInterface {

    static float read(int8_t *data, uint8_t size);

    static float write(int8_t *data, uint8_t size);

    static void print_stored_data();

    static void send_structure();

    static void read_data(uint16_t *indice, uint16_t size, void *data);

    static void write_data(uint16_t *indice, uint16_t size, void *data);

};


#endif //TRACER_EEPROMINTERFACE_H

#endif