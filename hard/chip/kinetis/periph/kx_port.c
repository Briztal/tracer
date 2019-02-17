/*
  kx_port.c Part of TRACER

  Copyright (c) 2018 Raphaël Outhier, Paul Stoffergen, for its port macros;

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



//--------------------------------------------------- Make parameters --------------------------------------------------

/*
 * mk64fx512.mk must provide :
 * 	- NB_PORTS : 		number of ports;
 * 	- PORT_REG : 		start of the port register area;
 * 	- PORT_SPACING : 	spacing between two port register areas
 * 	- GPIO_REG : 		start of the port register area;
 * 	- GPIO_SPACING : 	spacing between two port register areas
 */


//If one of the macro was not provided :
#if !defined(PORT_REG) || !defined(PORT_SPACING) || !defined(GPIO_REG) || !defined(GPIO_SPACING)

//Log
#error "Error, at least one macro argument hasn't been provided. Check the makefile;"

//Define macros. Allows debugging on IDE environment;
#define    PORT_REG    4
#define    PORT_SPACING 4
#define    GPIO_REG    4
#define    GPIO_SPACING 4

#endif


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


//--------------------------------------------------- Build constants --------------------------------------------------

//There are 5 ports on the kx port module;
#define NB_PORTS 5


//------------------------------------------------------ Includes ------------------------------------------------------


#include <if/port.h>

#include <if/gpio.h>

#include <fs/iinode.h>

#include <kernel/exec/mod_hook>


#include <macro/incr_call.h>

#include <stdmem.h>

#include <debug/printk.h>

#include "kx_sim.h"



//--------------------------------------------------- Memory structs ---------------------------------------------------

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
        .gpio = (struct gpio_memory *)(GPIO_REG + (i) * GPIO_SPACING)\
    },

//Initialize the areas array;
static const struct port_ptrs port_areas[NB_PORTS] = {INCR_CALL(NB_PORTS, INIT_AR)};

//Macro not used anymore;
#undef INIT_AR

//--------------------------------------------------- PORT operations --------------------------------------------------

//GPIO_DEFINE will define all io functions for a GPIO
#define GPIO_DEFINE(i) \
    static void set_##i(size_t mask) {((struct gpio_memory *) (GPIO_REG + (i) * GPIO_SPACING))->PSOR = mask;}\
    static void clear_##i(size_t mask) {((struct gpio_memory *) (GPIO_REG + (i) * GPIO_SPACING))->PCOR = mask;}\
    static void toggle_##i(size_t mask) {((struct gpio_memory *) (GPIO_REG + (i) * GPIO_SPACING))->PTOR = mask;}\
    static void write_##i(size_t mask) {((struct gpio_memory *) (GPIO_REG + (i) * GPIO_SPACING))->PDOR = mask;}\
    static size_t read_##i() {return (size_t)(((struct gpio_memory *) (GPIO_REG + (i) * GPIO_SPACING))->PDIR);}\

//Declare each channel;
INCR_CALL(NB_PORTS, GPIO_DEFINE)

//Macro not used anymore;
#undef GPIO_DEFINE


//----------------------------------------------- Port operations structs ----------------------------------------------

//INIT_ARRAY will print the reference of the i-th timer channel;
#define INIT_AR(i) {.set = &set_##i, .clear = &clear_##i, .toggle = &toggle_##i, .write = &write_##i, .read = &read_##i},

//Initialize the operations array;
static const struct gpio_port_operations operations[NB_PORTS] = {INCR_CALL(NB_PORTS, INIT_AR)};

//Macro not used anymore;
#undef INIT_AR


//---------------------------------------------------- Pins structs ----------------------------------------------------

/*
 * A pin data contains a port identifier and a bit identifier;
 */

struct pin_data {

	//The id of the port;
	const uint8_t port_id;

	//The id of the bit;
	const uint8_t bit_id;

	//The interfaced struct;
	struct gpio_if *interface;

};


/*
 * There is a fixed number of declared pins, that is decided at compile time;
 * 	The following struct keeps track of their data;
 */

//PIN will initialise a pin data struct;
#define PIN(name, port, bit) {.port_id = (port), .bit_id = (bit), .interface = 0,},

static struct pin_data pins[] = {

//Initialise all pins;
#include "hard/chip/kinetis/chips/mk64fx512/kx_port_pins.h"

};

//Macro not used anymore;
#undef PIN



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
/*

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
*/


static void pin_configuration(const struct pin_data *const pin, const struct port_pin_config *const config) {

	//Cache port and bit index;
	const uint8_t port_id = pin->port_id;
	const uint8_t bit = pin->bit_id;

	//Get port and gpio memory areas;
	struct port_memory *const port = port_areas[port_id].port;
	struct gpio_memory *const gpio = port_areas[port_id].gpio;

	//TODO ERRORS IN CASE OF BAD CONFIGURATION;

	//Declare the configuration register to write; Set the flag bit to clear it by default;
	uint32_t config_register = (PORT_PCR_ISF);

	//Get IRQ bits;
	uint8_t IRQ_bits = type_to_IRQ_bits(config->interrupt_type);

	//Set IRQ bits in the register;
	config_register |= PORT_PCR_TO_IRQC(IRQ_bits);

	//Update the interrupt function if the interrupt is enabled;
	if (IRQ_bits) {

		//TODO INTERRUPT FUNCTION;

	}

	//printkf("port : %d - %h, pin %d, alt %d", port_id, port, bit, config->mux_channel);

	//Set the multiplexer channel;
	config_register |= (PORT_PCR_TO_MUX(config->mux_channel));
	
	//If the data is received :
	if (config->direction == PIN_INPUT) {

		/*
		 * Input mode :
		 */

		//Evaluate the input mode;
		switch (config->input_mode) {

			//Only the pullup requires the PS bit set;
			case PORT_PULL_UP:
				config_register |= (PORT_PCR_PS);

				//Both pull-modes require the PE bit set;
			case PORT_PULL_DOWN:
				config_register |= (PORT_PCR_PE);

				//Hysteresis and repeater are not supported, High Impedance is the default mode;
			default://TODO ERROR
				break;

		}

		//Clear the appropriate bit in the GPIO direction register;
		gpio->PDDR &= ~((uint32_t) 1 << bit);


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
				config_register |= PORT_PCR_DSE;
				break;
			case PORT_OPEN_DRAIN:
			default:
				//Open drain in default case
				config_register |= PORT_PCR_ODE;
				break;
		}

		//If the slew rate is slow, set the appropriate bit;
		if (config->slew_rate == PORT_HIGH_RATE) {
			config_register |= PORT_PCR_SRE;
		}

		//Set the appropriate bit in the GPIO direction register;
		gpio->PDDR |= ((uint32_t) 1 << bit);

	}

	//Write the configuration register;
	port->PCR[bit] = config_register;
	
	
}


//--------------------------------------------------- File operations --------------------------------------------------

/*
 * A pin iinode contains only the index of the pin it references;
 */

struct pin_inode {

	//The iinode base;
	struct iinode node;

	//The pin index;
	const size_t pin_index;

};


/*
 * There is a fixed number of declared pins, that is decided at compile time;
 * 	The following struct keeps track of their data;
 */

//PIN will add an iinode initializer;
#define PIN(name, port, bit) {{0}},

//Declare the inodes array and let initializers determine the size;
static struct pin_inode inodes[] = {

//Add all initializers;
#include "hard/chip/kinetis/chips/mk64fx512/kx_port_pins.h"

};

//Macro not used anymore;
#undef PIN


/*
 * A port pin supports configuration, if and close;
 */

static bool fs_pin_init(const struct pin_inode *const node, const void *const config, const size_t config_size) {

	//If the configuration struct is not valid
	if (config_size != sizeof(struct port_pin_config)) {

		//Abort;
		return false;

	}
	
	//Configure the pin, providing the proper pin data struct reference;
	pin_configuration(&pins[node->pin_index], config);

	//Complete;
	return true;

}


//TODO
static bool fs_pin_interface(const struct pin_inode *const node, void *const gpio_iface, const size_t iface_size) {

	//If the configuration struct is not valid
	if (iface_size != sizeof(struct gpio_if)) {

		//Abort;
		return false;

	}

	//Cache the pin index;
	size_t pin_index = node->pin_index;

	//If another struct is interfaced :
	if (pins[pin_index].interface) {

		//Abort;
		return false;

	}

	//Cache port and bit identifier;
	uint8_t port_id = pins[pin_index].port_id;
	uint8_t bit_id = pins[pin_index].bit_id;

	//Cache the port register;
	struct port_memory *port_area = port_areas[port_id].port;

	//Create the gpio if initializer;
	struct gpio_if init = {

		//Provide the address of the port area; It is unique;
		.port_identifier = (size_t) port_area,

		//Provide access to the operations struct;
		.operations = operations + port_id,

		//Determine and save the pin's bit mask;
		.pin_mask = (size_t) (((size_t) 1) << bit_id),

	};

	//Keep track of the if struct for eventual neutralisation;
	pins[pin_index].interface = gpio_iface;

	//Interface the struct;
	memcpy(gpio_iface, &init, sizeof(struct gpio_if));

	//Complete;
	return true;

}


//TODO
static void fs_pin_close(const struct pin_inode *const node) {

	//Cache the pin index;
	size_t pin_index = node->pin_index;

	//Cache the location of the reference of the eventual if struct;
	struct gpio_if *iface = pins[pin_index].interface;

	//If the pin is interfaced :
	if (iface) {

		//Neutralise the struct;
		memcpy(iface, &neutral_gpio_interface, sizeof(struct gpio_if));

		//Reset the if pointer;
		pins[pin_index].interface = 0;

	}

}

/*
 * The port pin file operation structure will be the same for all pins;
 * 	A safe up-case is made to cast pin_inode * into iinode *;
 */
static const struct inode_ops port_pin_file_ops = {
	.init = (bool (*)(struct iinode *, const void *, size_t)) &fs_pin_init,
	.interface = (bool (*)(struct iinode *, void *, size_t)) &fs_pin_interface,
	.close = (void (*)(struct iinode *)) &fs_pin_close,
	//TODO IMPLEMENT A RESET OPERATION
	//TODO IMPLEMENT A RESET OPERATION
	//TODO IMPLEMENT A RESET OPERATION
	//TODO IMPLEMENT A RESET OPERATION
	//TODO IMPLEMENT A RESET OPERATION
	//TODO IMPLEMENT A RESET OPERATION
	//TODO IMPLEMENT A RESET OPERATION
	//TODO IMPLEMENT A RESET OPERATION
	//TODO IMPLEMENT A RESET OPERATION
	//TODO IMPLEMENT A RESET OPERATION
	//TODO IMPLEMENT A RESET OPERATION
	//TODO IMPLEMENT A RESET OPERATION
	//TODO IMPLEMENT A RESET OPERATION
	//TODO IMPLEMENT A RESET OPERATION
	//TODO IMPLEMENT A RESET OPERATION
	//TODO IMPLEMENT A RESET OPERATION
	//TODO IMPLEMENT A RESET OPERATION
	//TODO IMPLEMENT A RESET OPERATION
	//TODO IMPLEMENT A RESET OPERATION
	//TODO IMPLEMENT A RESET OPERATION
	//TODO IMPLEMENT A RESET OPERATION
	//TODO IMPLEMENT A RESET OPERATION
	//TODO IMPLEMENT A RESET OPERATION
	//TODO IMPLEMENT A RESET OPERATION
};


//-------------------------------------------------- Pins registration -------------------------------------------------

static void register_pin(const char *name, size_t pin_index) {

	//Cache the pin iinode;
	struct pin_inode *inode = &inodes[pin_index];

	//Create the iinode initializer;
	struct pin_inode init = {
		.node = INODE(&port_pin_file_ops),
		.pin_index = pin_index,
	};

	//Initialise the iinode;
	memcpy(inode, &init, sizeof(struct pin_inode));

	//Register the iinode;
	fs_create(name, (struct iinode *) inode);

}


//------------------------------------------------- Init - Exit ------------------------------------------------


static bool kx_port_init() {

	//Enable gating for all ports;
	sim_enable_PORTA_clock_gating();
	sim_enable_PORTB_clock_gating();
	sim_enable_PORTC_clock_gating();
	sim_enable_PORTD_clock_gating();
	sim_enable_PORTE_clock_gating();

	/*
	 * Pin registration;
	 */

	size_t pin_id = 0;

	//To register a pin, we simply call register_pin, providing its name and its address.
	// The index is then incremented;
	#define PIN(name, port, bit) register_pin(#name, pin_id++);

	//Register all pins
	#include "hard/chip/kinetis/chips/mk64fx512/kx_port_pins.h"

	//Macro not used anymore;
	#undef PIN


	//Complete;
	return true;

}


//Embed the module in the executable;
KERNEL_HOOK_MODULE(PERIPHERAL_MODULE, port, &kx_port_init)