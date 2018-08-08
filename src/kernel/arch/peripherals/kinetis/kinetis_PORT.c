/*
  kinetis_PORT.c Part of TRACER

  Copyright (c) 2017 Raphaël Outhier, Paul Stoffergen, for its kinetis macros;

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

#include "kinetis_PORT.h"

#include <kernel/kernel.h>


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
 * The kinetis PORT specific memory data;
 */

struct __attribute__ ((packed)) kinetis_PORT_registers {

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
 * The GPIO specific memory data;
 */

struct __attribute__ ((packed)) kinetis_GPIO_memory_t {

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


//--------------------------------------------------- Private Headers --------------------------------------------------

//Get the descriptor for a single IO pin : the register and the appropriate mask for the pin
bool kinetis_PORT_get_gpio_descriptor(const struct kinetis_PORT_driver_t *, enum gpio_register_type_e type,
									  const struct io_desc_t *,
									  volatile void **gpio_register, void *mask_p);

//Delete a kinetis PORT driver;
void kinetis_PORT_destructor(struct kinetis_PORT_driver_t *driver_data);


//Get a pin's current configuration;
void kinetis_PORT_get_pin_config(const struct kinetis_PORT_driver_t *driver, const struct io_desc_t *pin,
								 struct kinetis_pin_configuration_t *config);

//Set a pin's current configuration;
void kinetis_PORT_configure_pin(const struct kinetis_PORT_driver_t *driver, const struct io_desc_t *pin,
								struct kinetis_pin_configuration_t *config);


//Determine IRQ bits from interrupt type;
uint8_t type_to_IRQ_bits(enum PORT_interrupt_t type);

//Determine interrupt type from IRQ bits;
enum PORT_interrupt_t IRQ_bits_to_type(uint8_t bits);



//------------------------------------------------- Creation - Deletion ------------------------------------------------

/*
 * kinetis_GPIO_create : creates a kinetis port driver. Initialises function pointers and gpio registers sizes,
 * 	and initialises memory zones;
 */

struct kinetis_PORT_driver_t *kinetis_PORT_create(volatile void *const first_port_peripheral,
												  volatile void *const first_gpio_peripheral,
												  const uint8_t nb_peripherals,
												  const size_t port_memory_spacing, const size_t gpio_memory_spacing) {

	//Create the initializer;
	const struct kinetis_PORT_driver_t init = {

		.port_driver = {

			//Function pointers will only cast port driver pointer in the kinetis port driver pointer in function args,
			// legit because kinetis_PORT_driver is composed by port_driver;

			//Provide access to the pin config getter;
			.get_pin_config =
			(void (*const)(const struct port_driver_t *, const struct io_desc_t *, void *))
				&kinetis_PORT_get_pin_config,

			//Provide access to the pin configuration setter;
			.configure_pin =
			(void (*const)(const struct port_driver_t *, const struct io_desc_t *, void *))
				&kinetis_PORT_configure_pin,


			//gpio registers are 32 bits long;
			.gpio_register_size = 4,


			//Provide access to the descriptor provider;
			.get_gpio_descriptor =
			(bool (*const)(const struct port_driver_t *, enum gpio_register_type_e, const struct io_desc_t *,
						   volatile void **, void *))
				&kinetis_PORT_get_gpio_descriptor,

			//Provide access to the destructor;
			.destructor =  (void (*const)(struct port_driver_t *)) &kinetis_PORT_destructor,

		},

		//Initialise the port memory zone;
		.ports = {

			.memory_map = {
				.start_address = first_port_peripheral,
				.block_spacing = port_memory_spacing,
			},

			.bloc_desc = {
				.nb_blocks = nb_peripherals,
				.block_size = sizeof(struct kinetis_PORT_registers),
			}

		},


		//Initialise the gpios memory zone;
		.gpios = {

			.memory_map = {
				.start_address = first_gpio_peripheral,
				.block_spacing = gpio_memory_spacing,
			},

			.bloc_desc = {
				.nb_blocks = nb_peripherals,
				.block_size = sizeof(struct kinetis_GPIO_memory_t)
			}

		},


	};

	//Allocate, initialise, and return the gpio driver struct;
	return kernel_malloc_copy(sizeof(struct kinetis_PORT_driver_t), &init);

}


/*
 * kinetis_GPIO_destructor : deletes the kinetis GPIO driver struct;
 */

void kinetis_PORT_destructor(struct kinetis_PORT_driver_t *const driver_data) {

	//The kinetis GPIO driver doesn't own any dynamic data, we can just free the structure;
	return kernel_free(driver_data);

}



//------------------------------------------------- Descriptor access ------------------------------------------------


/*
 * kinetis_PORT_get_gpio_descriptor : Get the descriptor for a single IO pin
 * 	(updates the register and the appropriate mask for the pin);
 */

bool kinetis_PORT_get_gpio_descriptor(const struct kinetis_PORT_driver_t *driver, const enum gpio_register_type_e type,
									  const struct io_desc_t *io_desc,
									  volatile void **const gpio_register, void *const mask_p) {


	//Cache the gpio memory descriptor struct;
	const struct mem_desc_t *gpio_desc = &driver->gpios;

	//Cache the port index;
	uint8_t port_index = io_desc->port_index;

	//Cache the bit index;
	uint8_t bit_index = io_desc->bit_index;

	//Create a temp var containing the selected register;
	volatile void *gpio_reg;

	//Switch the type of register required;
	switch (type) {
		case GPIO_OUTPUT_DATA_REGISTER:
			gpio_reg = mem_desc_get_bloc_member(gpio_desc, port_index, offsetof(struct kinetis_GPIO_memory_t, PDOR));
			break;
		case GPIO_OUTPUT_SET_REGISTER:
			gpio_reg = mem_desc_get_bloc_member(gpio_desc, port_index, offsetof(struct kinetis_GPIO_memory_t, PSOR));
			break;
		case GPIO_OUTPUT_CLEAR_REGISTER:
			gpio_reg = mem_desc_get_bloc_member(gpio_desc, port_index, offsetof(struct kinetis_GPIO_memory_t, PCOR));
			break;
		case GPIO_OUTPUT_TOGGLE_REGISTER:
			gpio_reg = mem_desc_get_bloc_member(gpio_desc, port_index, offsetof(struct kinetis_GPIO_memory_t, PTOR));
			break;
		default:
			//If no correct register type was provided, fail;
			return false;
	}

	//Assign the register pointer;
	*gpio_register = gpio_reg;

	//Determine the mask; The GPIO peripheral eases our life;
	*(uint32_t *) mask_p = (uint32_t) 1 << bit_index;

	//Complete;
	return true;

}


//----------------------------------------------------- IRQ options ----------------------------------------------------

/*
 * type_to_IRQ_bits : determines the value to set in the IRQ bits of the PCR, depending on the interrupt type;
 */

uint8_t type_to_IRQ_bits(const enum PORT_interrupt_t type) {

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
 * type_to_IRQ_bits : determines the value to set in the IRQ bits of the PCR, depending on the interrupt type;
 */

enum PORT_interrupt_t IRQ_bits_to_type(const uint8_t bits) {

	/*
	 * Direct from the kinetis datasheet;
	 */

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


//----------------------------------------------------- Pins config ----------------------------------------------------

/*
 * PORT_get_pin_config : retrieves a pin's current configuration (avoid defaults mistakes);
 */

void kinetis_PORT_get_pin_config(const struct kinetis_PORT_driver_t *const driver,
								 const struct io_desc_t *const pin,
								 struct kinetis_pin_configuration_t *const config) {

	//Cache the peripheral index and the bit index;
	uint8_t port_id = pin->port_index;
	uint8_t bit = pin->bit_index;

	//Cache the concerned port and gpio peripherals memory addresses;
	volatile struct kinetis_PORT_registers *port = mem_desc_get_bloc(&driver->ports, port_id);
	volatile struct kinetis_GPIO_memory_t *gpio = mem_desc_get_bloc(&driver->gpios, port_id);

	//Declare the configuration register to write; Set the flag bit to clear it by default;
	uint32_t config_register = port->PCR[bit];

	//Get IRQ bits;
	uint8_t IRQ_bits = (uint8_t) PORT_PCR_FROM_IRQC(config_register);

	//Update the interrupt type in the config struct;
	config->interrupt_type = IRQ_bits_to_type(IRQ_bits);

	//Update the interrupt function if the interrupt is enabled;
	if (IRQ_bits) {

		//TODO INTERRUPT FUNCTION;

	}

	//Set the multiplexer channel;
	config->mux_channel = (uint8_t) PORT_PCR_FROM_MUX(config_register);

	//If the direction is output, set it. Otherwise, set input;
	if (gpio->PDDR & (uint32_t) 1 << bit) {
		config->direction = PORT_OUTPUT;
	} else {
		config->direction = PORT_INPUT;
	}


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

	//If the active filtering is enabled :
	if (port->DFER & (uint32_t) (1 << bit)) {

		//Set the type and the filtering length;
		config->input_filter = PORT_DIGITAL_FILTERING;
		config->filtering_length = (uint8_t) port->DFWR;

	} else {

		//Reset the filtering length;
		config->filtering_length = 1;

		//If the passive filtering is enabled, mark it. Otherwise, mark the non filtering sequences_initialised;
		if (config_register & PORT_PCR_PFE) {
			config->input_filter = PORT_PASSIVE_FILTERING;
		} else {
			config->input_filter = PORT_NO_FILTERING;
		}

	}



	/*
	 * Output mode :
	 *
	 * Set the output mode. The section is left verbose to clearly sequences_initialised that all cases are handled;
	 *  Compiler optimised;
	 */

	//If the output is in open drain :
	if (config_register & PORT_PCR_ODE) {
		config->output_mode = PORT_OPEN_DRAIN;
	} else if (config_register & PORT_PCR_DSE) {
		config->output_mode = PORT_HIGH_DRIVE;
	} else {
		config->output_mode = PORT_PUSH_PULL;
	}

	//Determine and save the slew rate;
	if (config_register & PORT_PCR_SRE) {
		config->slew_rate = PORT_HIGH_RATE;
	} else {
		config->slew_rate = PORT_LOW_RATE;
	}

}


/*
 * PORT_set_pin_configuration : sets all registers to fit the required configuration;
 *
 *  An invalid configuration generates an error;
 */

void kinetis_PORT_configure_pin(const struct kinetis_PORT_driver_t *const driver,
								const struct io_desc_t *const pin,
								struct kinetis_pin_configuration_t *const config) {

	//First, Update the mux channel;
	config->mux_channel = pin->mux_channel;

	//Cache the peripheral index and the bit index;
	uint8_t port_id = pin->port_index;
	uint8_t bit = pin->bit_index;

	//Cache the concerned port and gpio peripherals memory addresses;
	volatile struct kinetis_PORT_registers *port = mem_desc_get_bloc(&driver->ports, port_id);
	volatile struct kinetis_GPIO_memory_t *gpio = mem_desc_get_bloc(&driver->gpios, port_id);

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
		 * Filtering :
		 */

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


