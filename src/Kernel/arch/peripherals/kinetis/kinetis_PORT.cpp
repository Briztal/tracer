//
// Created by root on 5/22/18.
//

#include <Kernel/drivers/PORT.h>
#include "kinetis.h"

#include "kinetis_PORT.h"

#include "Kernel/drivers/PORT.h"


//TODO BITWISE.h

/*
 * SET : will set [data]'s bits that are set to 1 in [mask]. [data] is of size [size];
 */

#define SET(data, mask, size) (data) |= (uint##size##_t)(mask)


/*
 * CLEAR : will clear [data]'s bits that are set to 1 in [mask]. [data] is of size [size];
 */

#define CLEAR(data, mask, size) (data) &= ~(uint##size##_t)(mask)



/*
 * ------------------------------- Hardware constants macros -------------------------------
 */
//TODO PJRC TRIBUTE

#define PORT_PCR_ISF            ((uint32_t)0x01000000)        // Interrupt Status Flag
#define PORT_PCR_IRQC(n)        ((uint32_t)(((n) & 15) << 16))    // Interrupt Configuration
#define PORT_PCR_LK            ((uint32_t)0x00008000)        // Lock Register
#define PORT_PCR_MUX(n)            ((uint32_t)(((n) & 7) << 8))    // Pin Mux Control
#define PORT_PCR_DSE            ((uint32_t)0x00000040)        // Drive Strength Enable
#define PORT_PCR_ODE            ((uint32_t)0x00000020)        // Open Drain Enable
#define PORT_PCR_PFE            ((uint32_t)0x00000010)        // Passive Filter Enable
#define PORT_PCR_SRE            ((uint32_t)0x00000004)        // Slew Rate Enable
#define PORT_PCR_PE            ((uint32_t)0x00000002)        // Pull Enable
#define PORT_PCR_PS            ((uint32_t)0x00000001)        // Pull Select


/*
 * ------------------------------- Private headers -------------------------------
 */

//Determine IRQ bits from interrupt type;
uint8_t type_to_IRQ_bits(PORT_interrupt_t type);

//Determine interrupt type from IRQ bits;
PORT_interrupt_t IRQ_bits_to_type(uint8_t IRQ_bits);


/*
 * ------------------------------- Definitions -------------------------------
 */

/*
 * PORT_get_pin_config : retreives a pin's current configuration (avoid defaults mistakes);
 */

void PORT_get_pin_config(volatile PORT_data_t port, uint8_t bit, PORT_pin_config_t *config) {


}

/*
 * type_to_IRQ_bits : determines the value to set in the IRQ bits of the PCR, depending on the interrupt type;
 */

uint8_t type_to_IRQ_bits(PORT_interrupt_t type) {

    /*
     * Direct from the kinetis datasheet;
     */

    switch (type) {
        case PORT_NO_INTERRUPT :
            return 0;
        case PORT_DMA_RISING_EDGE :
            return 1;
        case PORT_DMA_FALLING_EDGE :
            return 2;
        case PORT_DMA_EDGE :
            return 3;
        case PORT_INTERRUPT_0 :
            return 8;
        case PORT_INTERRUPT_RISING_EDGE :
            return 9;
        case PORT_INTERRUPT_FALLING_EDGE :
            return 10;
        case PORT_INTERRUPT_EDGE :
            return 11;
        case PORT_INTERRUPT_1 :
            return 12;
        default:
            return 0;
    }

}

/*
 * PORT_set_pin_configuration : sets all registers to fit the required configuration;
 *
 *  An invalid configuration generates an error;
 */

void PORT_set_pin_configuration(PORT_data_t *volatile port, uint8_t bit, PORT_pin_config_t *config) {

    //TODO ERRORS IN CASE OF BAD CONFIGURATION;

    //Declare the configuration register to write; Set the flag bit to clear it by default;
    uint32_t config_register = 1 << PORT_PCR_ISF;

    //Get IRQ bits;
    uint8_t IRQ_bits = type_to_IRQ_bits(config->interrupt_type);

    //Set IRQ bits in the register;
    SET(config_register, PORT_PCR_IRQC(IRQ_bits), 32);

    //Update the interrupt function if the interrupt is enabled;
    if (IRQ_bits) {

        //TODO INTERRUPT FUNCTION;

    }

    //Set the multiplexer channel;
    SET(config_register, PORT_PCR_MUX(config->mux_channel), 32);

    //If the data is received :
    if (config->direction == PORT_INPUT) {

        /*
         * Input mode :
         */

        //Evaluate the input mode;
        switch (config->input_mode) {

            //Only the pullup requires the PS bit set;
            case PORT_PULL_UP:
                SET(config_register, PORT_PCR_PS, 32);

                //Both pull-modes require the PE bit set;
            case PORT_PULL_DOWN:
                SET(config_register, PORT_PCR_PE, 32);

                //Hysteresis is not supported, High Impedance is the default mode;
            default:
                break;

        }


        /*
         * Filtering :
         */

        switch (config->input_filter.input_filter) {

            //If the passive filtering must be enabled :
            case PORT_PASSIVE_FILTERING :

                //Set the passive filtering bit in the config register;
                SET(config_register, PORT_PCR_PFE, 32);
                break;

                //If the digital filtering must be enabled :
            case PORT_DIGITAL_FILTERING :

                //Set the appropriate bit in the Filtering Enable Register;
                SET(port->PORT_data->DFER, 1 << bit, 32);

                //Cache the width of the filter;
                uint8_t filter_width = config->input_filter.filtering_length;

                //Major the width of the filter;
                if (filter_width > 15) filter_width = 15;

                //Set the filter width bits;
                SET(config_register, filter_width, 32);

                //No filtering is default mode;
            default:
                break;

        }

    } else {

        /*
         * Output mode :
         *
         * Set the output mode. The section is left verbose to clearly state that all cases are handled;
         *  Compiler optimised;
         */

        switch (config->output_mode) {
            case PORT_PUSH_PULL:
                break;//Nothing to do;
            case PORT_HIGH_DRIVE:
                SET(config_register, PORT_PCR_DSE, 32);
                break;
            case PORT_OPEN_DRAIN:
            default:
                //Open drain in default case
                SET(config_register, PORT_PCR_ODE, 32);
                break;
        }

        //If the slew rate is slow, set the appropriate bit;
        if (config->slew_rate == PORT_HIGH_RATE) {
            SET(config_register, PORT_PCR_SRE, 32);
        }

        //Set the appropriate bit in the GPIO direction register;
        CLEAR(port->GPIO_data->PDDR, 1 << bit, 32);

    }

    //Write the configuration register;
    port->PORT_data->PCR[bit] = config_register;

}


/*
 * Declaration
 */


//TODO PJRC TRIBUTE

#define PORT_A_DATA		(volatile uint32_t *)0x40049000
#define PORT_B_DATA		(volatile uint32_t *)0x4004A000
#define PORT_C_DATA		(volatile uint32_t *)0x4004B000
#define PORT_D_DATA		(volatile uint32_t *)0x4004C000
#define PORT_E_DATA		(volatile uint32_t *)0x4004D000

#define GPIO_A_DATA     (volatile uint32_t *)0x400FF000
#define GPIO_B_DATA     (volatile uint32_t *)0x400FF040
#define GPIO_C_DATA     (volatile uint32_t *)0x400FF080
#define GPIO_D_DATA     (volatile uint32_t *)0x400FF0C0
#define GPIO_E_DATA     (volatile uint32_t *)0x400FF100

/*
 * The kinetis PORT peripheral always presents 5 distinct ports;
 */

PORT_data_t PORT_A {
        .PORT_data = (kinetis_PORT_memory_t *)PORT_A_DATA,
        .GPIO_data = (kinetis_GPIO_memory_t *)GPIO_A_DATA
};

PORT_data_t PORT_B {
        .PORT_data = (kinetis_PORT_memory_t *)PORT_B_DATA,
        .GPIO_data = (kinetis_GPIO_memory_t *)GPIO_B_DATA
};

PORT_data_t PORT_C {
        .PORT_data = (kinetis_PORT_memory_t *)PORT_C_DATA,
        .GPIO_data = (kinetis_GPIO_memory_t *)GPIO_C_DATA
};

PORT_data_t PORT_D {
        .PORT_data = (kinetis_PORT_memory_t *)PORT_D_DATA,
        .GPIO_data = (kinetis_GPIO_memory_t *)GPIO_D_DATA
};
PORT_data_t PORT_E {
        .PORT_data = (kinetis_PORT_memory_t *)PORT_E_DATA,
        .GPIO_data = (kinetis_GPIO_memory_t *)GPIO_E_DATA
};


