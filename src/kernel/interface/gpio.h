//
// Created by root on 9/13/18.
//

#ifndef TRACER_GPIO_H
#define TRACER_GPIO_H

#include "stdint.h"

#include "stddef.h"

//--------------------------------------------------------- GPIO -------------------------------------------------------

/*
 * gpio_operations : a set of functions that directly access
 */

struct gpio_port_operations {

	//Set some bits in the GPIO register;
	void (*set)(size_t mask);

	//Clear some bits in the GPIO register;
	void (*clear)(size_t mask);

	//Toogle some bits in the GPIO register;
	void (*toggle)(size_t mask);

	//Write the port;
	void (*write)(size_t data);

	//Read the port;
	size_t (*read)();

};


/*
 * gpio_interface : contains all data required to interface with a gpio pin;
 */

struct gpio_interface {

	//The port identifier. Interfaces with the same identifier have the same operations set, and can be combined;
	const size_t port_identifier;

	//The set of operations;
	const struct gpio_port_operations *operations;

	//The mask of the required pin;
	const size_t pin_mask;

};


//Static inline shortcut generator;
#define GPIO_SHORTCUT(name, type)\
static inline type gpio_##name(struct gpio_interface *gpio, size_t mask) {\
	(*(gpio->operations->name))(mask);\
}


GPIO_SHORTCUT(set, void);
GPIO_SHORTCUT(clear, void);
GPIO_SHORTCUT(toggle, void);
GPIO_SHORTCUT(write, void);
GPIO_SHORTCUT(read, size_t);


#undef GPIO_SHORTCUT

#endif //TRACER_GPIO_H
