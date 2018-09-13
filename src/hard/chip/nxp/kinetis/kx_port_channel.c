/*
  kx_port.c Part of TRACER

  Copyright (c) 2018 Raphaël Outhier, Paul Stoffergen, for its K64 macros;

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


#include <stdint.h>
#include <kernel/interface/port.h>
#include <util/macro/incr_call.h>
#include <kernel/interface/gpio.h>
#include <stdbool.h>


// --------------------------------------------- Hardware constants macros ---------------------------------------------

//TODO PJRC TRIBUTE

#define PORT_PCR_ISF                ((uint32_t)0x01000000)        // Interrupt Status Flag
#define PORT_PCR_TO_IRQC(n)         ((uint32_t)(((n) & 15) << 16))    // Interrupt Configuration
#define PORT_PCR_FROM_IRQC(n)       ((uint32_t)(((n) >> 16) & 15))    // Interrupt Configuration
#define PORT_PCR_LK                 ((uint32_t)0x00008000)        // Lock Register
#define PORT_PCR_TO_MUX(n)          ((uint32_t)(((n) & 7) << 8))    // Pin Mux Control
#define PORT_PCR_FROM_MUX(n)        ((uint32_t)(((n) >> 8 ) & 7))    // Pin Mux Control
#define PORT_PCR_DSE                ((uint32_t)0x00000040)        // Drive Strength Enable
#define PORT_PCR_ODE                ((uint32_t)0x00000020)        // Open Drain Enable
#define PORT_PCR_PFE                ((uint32_t)0x00000010)        // Passive Filter Enable
#define PORT_PCR_SRE                ((uint32_t)0x00000004)        // Slew Rate Enable
#define PORT_PCR_PE                 ((uint32_t)0x00000002)        // Pull Enable
#define PORT_PCR_PS                 ((uint32_t)0x00000001)        // Pull Select


/*
 * SET : will set [data]'s bits that are set to 1 in [mask]. [data] is of size [size];
 */

#define SET(data, mask, size) (data) |= (uint##size##_t)(mask)


/*
 * CLEAR : will clear [data]'s bits that are set to 1 in [mask]. [data] is of size [size];
 */

#define CLEAR(data, mask, size) (data) &= ~(uint##size##_t)(mask)


//TODO BITWISE.h


//-------------------------------------------------- Memory structures --------------------------------------------------

/*
 * The kx port memory map;
 */

struct __attribute__ ((packed)) port_memory {

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

};


/*
 * The kx gpio memory map;
 */

struct __attribute__ ((packed)) gpio_memory {

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

};


//--------------------------------------------------- Make parameters --------------------------------------------------

/*
 * Makefile must provide :
 * 	- NB_PORTS : 		number of ports;
 * 	- PORT_REG : 		start of the port register area;
 * 	- PORT_SPACING : 	spacing between two port register areas
 * 	- GPIO_REG : 		start of the port register area;
 * 	- GPIO_SPACING : 	spacing between two port register areas
 */


//If one of the macro was not provided :
#if !defined(NB_PORTS) || !defined(PORT_REG) || !defined(PORT_SPACING) || !defined(GPIO_REG) || !defined(GPIO_SPACING)

//Log
#error "Error, at least one macro argument hasn't been provided. Check the makefile;"

//Define macros. Allows debugging on IDE environment;
#define    NB_PORTS     4
#define    PORT_REG  	4
#define    PORT_SPACING 4
#define    GPIO_REG  	4
#define    GPIO_SPACING 4

#endif


//----------------------------------------------- PORT pointers structs ----------------------------------------------

struct port_ptrs {

	//The port register area;
	struct port_memory *port;

	//The gpio register area;
	struct gpio_memory *gpio;

};

//INIT_ARRAY will print the reference of the i-th timer channel;
#define INIT_AR(i)\
	{\
		.port =  (struct port_memory *) (PORT_REG + (i) * PORT_SPACING),\
		.gpio = (struct gpio_memory *)(GPIO_REG + (i) * PORT_SPACING)\
	},

//Initialize the areas array;
static struct port_ptrs port_areas[NB_PORTS] = {INCR_CALL(NB_PORTS, INIT_AR)};

//Macro not used anymore;
#undef INIT_AR

//--------------------------------------------------- PORT operations --------------------------------------------------

//GPIO_DEFINE will define all io functions for a GPIO
#define GPIO_DEFINE(i) \
	static void set_##i(size_t mask) {((struct gpio_memory *) (GPIO_REG + (i) * PORT_SPACING))->PSOR = mask;}\
	static void clear_##i(size_t mask) {((struct gpio_memory *) (GPIO_REG + (i) * PORT_SPACING))->PCOR = mask;}\
	static void toggle_##i(size_t mask) {((struct gpio_memory *) (GPIO_REG + (i) * PORT_SPACING))->PTOR = mask;}\
	static void write_##i(size_t mask) {((struct gpio_memory *) (GPIO_REG + (i) * PORT_SPACING))->PDOR = mask;}\
	static size_t read_##i() {return (size_t)(((struct gpio_memory *) (GPIO_REG + (i) * PORT_SPACING))->PDIR);}\

//Declare each channel;
INCR_CALL(NB_PORTS, GPIO_DEFINE);

//Macro not used anymore;
#undef GPIO_DEFINE


//----------------------------------------------- PORT operations structs ----------------------------------------------

//INIT_ARRAY will print the reference of the i-th timer channel;
#define INIT_AR(i) {.set = &set_##i, .clear = &clear_##i, .toggle = &toggle_##i, .write = &write_##i, .read = &read_##i},

//Initialize the operations array;
static struct gpio_port_operations operations[NB_PORTS] = {INCR_CALL(NB_PORTS, INIT_AR)};

//Macro not used anymore;
#undef INIT_AR



/*
 * TODO :
 *
 * 	Required data :
 * 		- Array of port registers area and gpio registers area for each port;
 * 		- Array of gpio operations for each port;
 *
 *	Functions :
 *		- Port configure (port) -> port data stored in file
 *		- Pin configure (port + bit) -> port data + bit stored in file;
 *		- Gpio operations ()
 *
 *
 * 	Pin registration is centralised by the
 *
 *
 *
 * 	Pin / Port configuration functions are not hot code, and are generic. Same function is used for all pins;
 *
 * 	GPIO operations are hot, and are implemented separately for each channel;
 *
 */


//------------------------------------------------- Init - Exit ------------------------------------------------


void kx_port_init() {

	//TODO REGISTER ALL PORTS;

	//TODO REGISTER ALL AVAILABLE PINS;

	//TODO

}



bool kx_port_exit() {

}


//-------------------------------------------------- Pin configuration -------------------------------------------------

/*
 * type_to_IRQ_bits : determines the value to set in the IRQ bits of the PCR, depending on the interrupt type;
 */

static uint8_t type_to_IRQ_bits(const enum port_interrupt_t type) {

	//Cf kinetis datasheet;

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
 * type_to_IRQ_bits : determines the value to set in the IRQ bits of the PCR, depending on the interrupt type;
 */

static enum port_interrupt_t IRQ_bits_to_type(const uint8_t bits) {

	//Cf kinetis datasheet;

	switch (bits) {
		case 1 :
			return PORT_DMA_RISING_EDGE;
		case 2 :
			return PORT_DMA_FALLING_EDGE;
		case 3 :
			return PORT_DMA_EDGE;
		case 8 :
			return PORT_INTERRUPT_0;
		case 9 :
			return PORT_INTERRUPT_RISING_EDGE;
		case 10 :
			return PORT_INTERRUPT_FALLING_EDGE;
		case 11 :
			return PORT_INTERRUPT_EDGE;
		case 12 :
			return PORT_INTERRUPT_1;
		case 0 :
		default:
			return PORT_NO_INTERRUPT;
	}

}


static void configure_pin(struct gpio_memory *gpio, struct port_memory *port,
						  struct port_pin_config *const config, uint8_t bit) {

	//TODO ERRORS IN CASE OF BAD CONFIGURATION;

	//Declare the configuration register to write; Set the flag bit to clear it by default;
	uint32_t config_register = PORT_PCR_ISF;

	//Get IRQ bits;
	uint8_t IRQ_bits = type_to_IRQ_bits(config->interrupt_type);

	//Set IRQ bits in the register;
	SET(config_register, PORT_PCR_TO_IRQC(IRQ_bits), 32);

	//Update the interrupt function if the interrupt is enabled;
	if (IRQ_bits) {

		//TODO INTERRUPT FUNCTION;

	}

	//Set the multiplexer channel;
	SET(config_register, PORT_PCR_TO_MUX(config->mux_channel), 32);

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

				//Hysteresis and repeater are not supported, High Impedance is the default mode;
			default://TODO ERROR
				break;

		}

		//Clear the appropriate bit in the GPIO direction register;
		CLEAR(gpio->PDDR, 1 << bit, 32);


		/*
		 * Filtering : Not handled for instance;
		 */

		/*

		switch (config->input_filter) {

			//If the passive filtering must be enabled :
			case PORT_PASSIVE_FILTERING :

				//Set the passive filtering bit in the config register;
				SET(config_register, PORT_PCR_PFE, 32);
				break;

				//If the digital filtering must be enabled :
			case PORT_DIGITAL_FILTERING :

				//Set the appropriate bit in the Filtering Enable Register;
				SET(port->DFER, 1 << bit, 32);

				//Cache the width of the filter;
				uint8_t filter_width = config->filtering_length;

				//Major the width of the filter;
				if (filter_width > 15) filter_width = 15;

				//Set the filter width bits;
				SET(port->DFWR, filter_width, 32);

				//TODO FILTERING IS CONFIGURED PER PORT. MARK IT IN DATA;

				//No filtering is default mode;
			default:
				break;

		}
		 */

	} else {

		/*
		 * Output mode :
		 *
		 * Set the output mode. The section is left verbose to clearly sequences_initialised that all cases are handled;
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
		SET(gpio->PDDR, 1 << bit, 32);

	}

	//Write the configuration register;
	port->PCR[bit] = config_register;

}


