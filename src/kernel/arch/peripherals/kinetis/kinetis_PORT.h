/*
  kinetis_PORT.h Part of TRACER

  Copyright (c) 2017 Raphaël Outhier

  TRACER is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  TRACER is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  aint32_t with TRACER.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef TRACER_KINETIS_PORT_H
#define TRACER_KINETIS_PORT_H

#include "stdint.h"

/*
 * Different types of hardware filtering;
 */

typedef enum {

    //No filtering enabled;
            PORT_NO_FILTERING,

    //Digital filtering enabled;
            PORT_DIGITAL_FILTERING,

    //Passive filtering enabled;
            PORT_PASSIVE_FILTERING,

} PORT_input_filter_t;


typedef struct {

    //The type of input filtering;
    PORT_input_filter_t input_filter;

    //The length of the eventual digital filtering;
    uint8_t filtering_length;

} PORT_input_filter_data_t;


/*
 * The PORT specific memory data;
 */

typedef struct __attribute__ ((packed)) {

    //32 Pin Configuration Registers;
    volatile uint32_t PCR[32];

    //Global Pin Control Low Regiser;
    volatile uint32_t GPCLR;

    //Global Pin Control High Regiser;
    volatile uint32_t GPCHR;

    //24 unused bytes;
    volatile uint32_t padding0[6];

    //Interrupt Status Flag Register;
    volatile uint32_t ISFR;

    //28 unused bytes;
    volatile uint32_t padding1[7];

    //Digital Filter Enable Register;
    volatile uint32_t DFER;

    //Digital Filter Clock Register;
    volatile uint32_t DFCR;

    //Digital Filter Width Register;
    volatile uint32_t DFWR;

} kinetis_PORT_memory_t;


/*
 * The GPIO specific memory data;
 */

typedef struct __attribute__ ((packed)) {

    //Port Data Output Register;
    volatile uint32_t PDOR;

    //Port Set Output Register;
    volatile uint32_t PSOR;

    //Port Clear Output Register;
    volatile uint32_t PCOR;

    //Port Toggle Output Register;
    volatile uint32_t PTOR;

    //Port Data Input Register;
    volatile uint32_t PDIR;

    //Port Data Direction Register;
    volatile uint32_t PDDR;

} kinetis_GPIO_memory_t;


/*
 * The port structure is composed of two pointer to hardware configuration zones, one for PORT, and the other for GPIO;
 */

typedef struct {

    //The PORT part
    volatile kinetis_PORT_memory_t *const PORT_data;

    //The GPIO part;
    volatile kinetis_GPIO_memory_t *const GPIO_data;

} PORT_data_t;


#define KINETIS_PORT_DECLARE(id) extern PORT_data_t PORT_##id

#define KINETIS_PORT_DEFINE(id, PORT_DATA, GPIO_DATA) \
PORT_data_t PORT_##id = {\
        .PORT_data = (volatile kinetis_PORT_memory_t *) (PORT_DATA),\
        .GPIO_data = (volatile kinetis_GPIO_memory_t *) (GPIO_DATA)\
};

//Define the GPIO mask type to be 4 bytes length;
typedef uint32_t GPIO_mask_t;


/*
 * ---------------------------------- Standard GPIO functions implementations ----------------------------------
 */


// ---------------------------------- GPIO Calculator ----------------------------------

/*
 * Determine the mask from a bit index;
 */

inline GPIO_mask_t GPIO_get_mask(uint8_t bit) { return ((uint32_t) 1 << bit); }


// ---------------------------------- GPIO functions ----------------------------------

/*
 * The software setter : no software process to do, all is supported by the hardware;
 */

inline void GPIO_data(volatile void *hw, GPIO_mask_t value) { *(volatile GPIO_mask_t *) hw = value; }


/*
 * The software bitwise setter : no software process to do, all is supported by the hardware;
 */

inline void GPIO_set_bits(volatile void *hw, GPIO_mask_t value) { *(volatile GPIO_mask_t *) hw = value; }


/*
 * The software bitwise clearer : no software process to do, all is supported by the hardware;
 */

inline void GPIO_clear_bits(volatile void *hw, GPIO_mask_t value) { *(volatile GPIO_mask_t *) hw = value; }


/*
 * The software bitwise toggler : no software process to do, all is supported by the hardware;
 */

inline void GPIO_toggle_bits(volatile void *hw, GPIO_mask_t value) { *(volatile GPIO_mask_t *) hw = value; }


#endif //TRACER_KINETIS_PORT_H
