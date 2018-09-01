//
// Created by root on 6/15/18
//

//TODO PJRC TRIBUTE


#ifndef TRACER_KINETIS_PIT_H
#define TRACER_KINETIS_PIT_H


#include <stdint.h>

#include <stddef.h>

#include <kernel/arch/drivers/driver.h>


struct K64_PIT_specs {

	//The address of the PIT Module Control Register;
	volatile uint32_t *const MCR;

	//The number of timer in the module;
	const uint8_t nb_PITs;

	//The address of the first PIT registers area;
	volatile void *const first_area;

	//The spacing between each PIT registers area:
	const size_t spacing;

	//The set of interrupt channels, of size @nb_PITs;
	const uint8_t *const int_channels;

	//The module's clock frequency;
	const uint32_t clock_frequency;

};


struct K64_PIT_driver {

	//The kernel driver base, referencing init and prempt;
	struct kernel_driver driver;

	//The hardware specs set;
	const struct K64_PIT_specs hw_specs;

	//The timer interfaces array;
	struct timer_interface **const ifaces;

};


/**
 * K64_PIT_create : creates a driver to interact with the K64 PIT timer;
 *
 * @param specs : the set of hardware specifications;
 *
 * @return the initialised driver;
 */

struct kernel_driver *K64_PIT_create(struct K64_PIT_specs *specs);


#endif //TRACER_KINETIS_PIT_H
