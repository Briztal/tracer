//
/* Created by root on 9/13/18.*/
/**/

#ifndef TRACER_GPIO_H
#define TRACER_GPIO_H

#include "stdint.h"

#include "stddef.h"

/*--------------------------------------------------------- GPIO -------------------------------------------------------*/

/*
 * gpio_operations : a set of functions that directly access
 */

struct gpio_port_operations {

	/*Set some bits in the GPIO register;*/
	void (*const set)(size_t mask);

	/*Clear some bits in the GPIO register;*/
	void (*const clear)(size_t mask);

	/*Toogle some bits in the GPIO register;*/
	void (*const toggle)(size_t mask);

	/*Write the port;*/
	void (*const write)(size_t data);

	/*Read the port;*/
	size_t (*const read)();

};


/*
 * gpio_if : contains all data required to if with a gpio pin;
 */

struct gpio_if {

	/*The port identifier. Interfaces with the same identifier have the same operations set, and can be combined;*/
	const size_t port_identifier;

	/*The mask of the required pin;*/
	const size_t pin_mask;
	
	/*The set of operations;*/
	const struct gpio_port_operations *operations;

};


/*The gpio lib includes a neutral gpio if, so that gpio drivers can prevent access to their functions;*/
extern const struct gpio_if neutral_gpio_interface;


/*Static inline shortcut generator;*/
#define GPIO_SHORTCUT(name, type, end)\
static inline type gpio_##name(const struct gpio_if *gpio, size_t mask) {\
	end (*(gpio->operations->name))(mask);\
}


GPIO_SHORTCUT(set, void,)
GPIO_SHORTCUT(clear, void,)
GPIO_SHORTCUT(toggle, void,)
GPIO_SHORTCUT(write, void,)
GPIO_SHORTCUT(read, size_t, return)


#undef GPIO_SHORTCUT

#endif /*TRACER_GPIO_H*/
