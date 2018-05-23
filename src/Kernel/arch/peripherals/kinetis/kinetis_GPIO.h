//
// Created by root on 5/20/18.
//

#ifndef TRACER_KINETIS_GPIO_H
#define TRACER_KINETIS_GPIO_H

#include "stdint.h"

//Define the GPIO mask type to be 4 bytes length;
typedef uint32_t GPIO_mask_t;

//Define GPIO hardware processors;
typedef volatile GPIO_mask_t *GPIO_hw_register;

#include "Arduino.h"


/*
 * Standard GPIO functions implementations;
 */

// ---------------------------------- GPIO Calculators ----------------------------------


/*
 * Determine the mask from a bit index;
 */

inline GPIO_mask_t GPIO_get_mask(uint8_t bit) { return ((uint32_t)1<<bit);}


// ---------------------------------- GPIO functions ----------------------------------

/*
 * The software setter : no software process to do, all is supported by the hardware;
 */

inline void GPIO_set(GPIO_hw_register hw, GPIO_mask_t value) {*hw = value; }


/*
 * The software bitwise setter : no software process to do, all is supported by the hardware;
 */

inline void GPIO_set_bits(GPIO_hw_register hw, GPIO_mask_t value) { *hw = value; }


/*
 * The software bitwise clearer : no software process to do, all is supported by the hardware;
 */

inline void GPIO_clear_bits(GPIO_hw_register hw, GPIO_mask_t value) { *hw = value; }


/*
 * The software bitwise toggler : no software process to do, all is supported by the hardware;
 */

inline void GPIO_toggle_bits(GPIO_hw_register hw, GPIO_mask_t value) { *hw = value; }



/*
 * Registers declarations;
 *
 *
 */

#endif //TRACER_KINETIS_GPIO_H
