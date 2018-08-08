//
// Created by root on 6/15/18
//

//TODO PJRC TRIBUTE


#ifndef TRACER_KINETIS_PIT_H
#define TRACER_KINETIS_PIT_H

#include <kernel/drivers/timer.h>


//----------------------------------------- Structs declarations - definitions -----------------------------------------

/*
 * The kinetis Periodic Interrupt Timer memory struct; Maps the memory structure of a hardware PIT;
 */
struct __attribute__ ((packed)) kinetis_PIT_registers {

	//The Load Value;
	volatile uint32_t LDVAL;

	//Current Value;
	volatile uint32_t CVAL;

	//Timer Control Register
	volatile uint32_t TCTRL;

	//Timer Flag;
	volatile uint32_t TFLG;

};


/*
 * The kinetis Periodic Interrupt Timer data struct; contains a memory zone pointer and a clock frequency;
 */
struct kinetis_PIT_hw {

	//The PIT's memory zone
	struct kinetis_PIT_registers *const registers;

	//The interrupt channel;
	const uint8_t reload_int_channel;

	//The PIT's frequency;
	const uint32_t clock_frequency;

};


/*
 * The kinetis PIT driver contains a set of hardware specs and
 */

struct kinetis_PIT_driver {

	//The hardware specs set;
	struct kinetis_PIT_hw *hw_specs;

	//The current timer interface;
	struct timer_interface *iface;

};


//The kinetis PIT only has its base frequency configurable;
struct kinetis_PIT_config {

	//The base frequency (Hz);
	uint32_t base_frequency;

};


//------------------------------------------------- Creation - Deletion ------------------------------------------------

//Create a kinetis PIT driver from all hardware peripherals;
struct kinetis_PIT_driver *kinetis_PIT_create(void *registers, uint8_t int_channel, uint32_t clock_frequency);

//Delete a kinetis PIT driver;
void kinetis_PIT_delete(struct kinetis_PIT_driver *driver);


//---------------------------------------------------- Start - Stop ----------------------------------------------------

//Start the driver, providing the configuration;
void kinetis_PIT_start(struct kinetis_PIT_driver *driver, struct kinetis_PIT_config *config);

//Stop the driver. Timer interface will be deleted;
void kinetis_PIT_stop(struct kinetis_PIT_driver *driver);



#endif //TRACER_KINETIS_PIT_H
