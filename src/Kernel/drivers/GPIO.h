//
// Created by root on 5/20/18.
//

/*
 * The GPIO Driver comprises several inline functions, that are declared in this file, and defined in a .c file
 *  depending on the architecture you build on;
 *
 *  This file exists only as a declaration header, but real inline functions are defined in the driver implementation
 *  header file, included in arch.h just after;
 */




//Define the GPIO mask type to be of your register length;
typedef x GPIO_mask_t;

//Define GPIO hardware register;
typedef volatile GPIO_mask_t *GPIO_hw_register;


/*
 * Determine the mask from a bit index;
 */

static inline GPIO_mask_t GPIO_get_mask(uint8_t bit);


// ---------------------------------- GPIO functions ----------------------------------

/*
 * The software setter : no software process to do, all is supported by the hardware;
 */

static inline void GPIO_set(GPIO_hw_register hw, GPIO_mask_t value);


/*
 * The software bitwise setter : no software process to do, all is supported by the hardware;
 */

static inline void GPIO_set_bits(GPIO_hw_register hw, GPIO_mask_t value);


/*
 * The software bitwise clearer : no software process to do, all is supported by the hardware;
 */

static inline void GPIO_clear_bits(GPIO_hw_register hw, GPIO_mask_t value);


/*
 * The software bitwise toggler : no software process to do, all is supported by the hardware;
 */

static inline void GPIO_toggle_bits(GPIO_hw_register hw, GPIO_mask_t value);

