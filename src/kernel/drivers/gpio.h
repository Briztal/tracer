//
// Created by root on 7/23/18.
//

#ifndef TRACER_GPIO_H
#define TRACER_GPIO_H

#include <stdbool.h>

#include <stdint.h>

#include <stddef.h>

#include "io_desc.h

#include <kernel/memory/memory_descriptor.h>



struct gpio_driver_t {

	//The structure that references all consecutive gpio ports;
	const struct mem_desc_t gpios;

	//The size of a gpio register;
	const size_t gpio_register_size;


	/*
	 * Offsets to query gpio registers;
	 */

	//The data register offset (-1 for disable);
	const size_t data_register_offset;

	//The set register offset  (-1 for disable);
	const size_t set_register_offset;

	//The clear register offset  (-1 for disable);
	const size_t clear_register_offset;

	//The toggle register offset (-1 for disable);
	const size_t toggle_register_offset;



	//Get the descriptor for a single IO pin : the register and the appropriate mask for the pin
	bool (*const get_descriptor)(const struct gpio_driver_t *, enum gpio_register_type_e type,
								 const struct io_desc_t *,
								 volatile void **gpio_register, void *mask_p);


	//The function that must be called to delete the gpio driver;
	void (*const destructor)(struct gpio_driver_t *);

};


//Shortcut for descriptor query;
inline bool gpio_driver_get_descriptor(const struct gpio_driver_t * driver, enum gpio_register_type_e type,
							 const struct io_desc_t *io_desc,
							 volatile void **gpio_register, void *mask_p) {
	return (*(driver->get_descriptor))(driver, type, io_desc, gpio_register, mask_p);
}


//Shortcut for destructor;
inline void gpio_driver_delete(struct gpio_driver_t *driver) {
	(*(driver->destructor))(driver);
}


#endif //TRACER_GPIO_H
