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


#include <kernel/kernel.h>
#include "kinetis_GPIO.h"


//----------------------------------------------------- Memory map -----------------------------------------------------

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


//Delete a kinetis GPIO driver;
void kinetis_PORT_destructor(struct gpio_driver_t *driver_data);


//Get the descriptor for a single IO pin : the register and the appropriate mask for the pin
bool kinetis_PORT_get_gpio_descriptor(const struct gpio_driver_t *, enum gpio_register_type_e type,
								 const struct io_desc_t *,
								 volatile void **gpio_register, void *mask_p);


//------------------------------------------------- Creation - Deletion ------------------------------------------------

//Create a kinetis GPIO driver;
struct gpio_driver_t *kinetis_GPIO_create(struct kinetis_GPIO_memory_t *register_zones, uint8_t nb_peripherals,
										  size_t memory_spacing) {

	//Create the initializer;
	const struct gpio_driver_t init = {

		//Initialise the memory zone;
		.gpios = {
			.start_address = register_zones,
			.bloc_spacing = memory_spacing,
			.nb_blocs = nb_peripherals,
			.bloc_size = sizeof(struct kinetis_GPIO_memory_t)
		},

		//gpio registers are 32 bits long;
		.gpio_register_size = 4,

		//Give the offset of PDOR
		.data_register_offset = offsetof(struct kinetis_GPIO_memory_t, PDOR),

		//Give the offset of PSOR,
		.set_register_offset = offsetof(struct kinetis_GPIO_memory_t, PSOR),

		//Give the offset of PCOR,
		.clear_register_offset = offsetof(struct kinetis_GPIO_memory_t, PCOR),

		//Give the offset of PTOR,
		.toggle_register_offset = offsetof(struct kinetis_GPIO_memory_t, PTOR),

		//Provide access to the descriptor provider;
		.get_descriptor = &kinetis_GPIO_get_descriptor,

		//Provide access to the destructor;
		.destructor = &kinetis_GPIO_destructor,

	};

	//Allocate, initialise, and return the gpio driver struct;
	return kernel_malloc_copy(sizeof(struct gpio_driver_t), &init);

}


/*
 * kinetis_GPIO_destructor : deletes the kinetis GPIO driver struct;
 */

void kinetis_GPIO_destructor(struct gpio_driver_t *driver_data) {

	//The kinetis GPIO driver doesn't own any dynamic data, we can just free the structure;
	return kernel_free(driver_data);

}


//------------------------------------------------- Descriptor access ------------------------------------------------

//Get the descriptor for a single IO pin : the register and the appropriate mask for the pin
bool kinetis_GPIO_get_descriptor(const struct gpio_driver_t *driver, enum gpio_register_type_e type,
								 const struct io_desc_t *io_desc,
								 volatile void **gpio_register, void *mask_p) {


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
		case GPIO_DATA_REGISTER:
			gpio_reg = mem_desc_get_bloc_member(gpio_desc, port_index, driver->data_register_offset);
			break;
		case GPIO_SET_REGISTER:
			gpio_reg = mem_desc_get_bloc_member(gpio_desc, port_index, driver->set_register_offset);
			break;
		case GPIO_CLEAR_REGISTER:
			gpio_reg = mem_desc_get_bloc_member(gpio_desc, port_index, driver->clear_register_offset);
			break;
		case GPIO_TOGGLE_REGISTER:
			gpio_reg = mem_desc_get_bloc_member(gpio_desc, port_index, driver->toggle_register_offset);
			break;
		default:
			//If no correct register type was provided, fail;
			return false;
	}

	//Assign the register pointer;
	*gpio_register = gpio_reg;

	//Determine the mask; The GPIO peripheral eases our life;
	*(uint32_t *) mask_p = (uint32_t) 1 << bit_index;

}
