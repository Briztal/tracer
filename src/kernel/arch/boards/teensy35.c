/*
  teensy35.c Part of TRACER

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


#include "teensy35.h"


/*
 * --------------------------------------- PORT ---------------------------------------
 */

//TODO PJRC TRIBUTE

//PORT memory locations. Start at 0x40049000, spaces of 0x1000 bytes
#define PORT_A_MEMORY    (volatile void *)0x40049000
#define PORT_B_MEMORY    (volatile void *)0x4004A000
#define PORT_C_MEMORY    (volatile void *)0x4004B000
#define PORT_D_MEMORY    (volatile void *)0x4004C000
#define PORT_E_MEMORY    (volatile void *)0x4004D000

//PORT memory locations. Start at 0x400FF000, spaces of 0x40 bytes
#define GPIO_A_MEMORY    (volatile void *)0x400FF000
#define GPIO_B_MEMORY    (volatile void *)0x400FF040
#define GPIO_C_MEMORY    (volatile void *)0x400FF080
#define GPIO_D_MEMORY    (volatile void *)0x400FF0C0
#define GPIO_E_MEMORY    (volatile void *)0x400FF100

struct kinetis_PORT_driver_t *PORT;


/*
 * --------------------------------------- PIT ---------------------------------------
 */

//The teensy35 supports 4 PITs.
//KINETIS_PIT_DEFINE(0, 0x40037100, IRQ_PIT_CH0, F_BUS)
//KINETIS_PIT_DEFINE(1, 0x40037110, IRQ_PIT_CH1, F_BUS)
//KINETIS_PIT_DEFINE(2, 0x40037120, IRQ_PIT_CH2, F_BUS)
//KINETIS_PIT_DEFINE(3, 0x40037130, IRQ_PIT_CH3, F_BUS)


/*
 * --------------------------------------- UART ---------------------------------------
 */

//Include the kinetis driver header;
#include <kernel/arch/peripherals/kinetis/kinetis_UART.h>

//The teensy35 supports 6 UARTS;
//KINETIS_UART_DEFINE(0, 0x4006A000, F_CPU, 8, 8, IRQ_UART0_STATUS, IRQ_UART0_ERROR)


/*
KINETIS_UART_DEFINE(1, 0x4006B000, F_CPU, 8, 8, IRQ_UART1_STATUS, IRQ_UART1_ERROR)
KINETIS_UART_DEFINE(2, 0x4006C000, F_BUS, 1, 1, IRQ_UART2_STATUS, IRQ_UART2_ERROR)
KINETIS_UART_DEFINE(3, 0x4006D000, F_BUS, 1, 1, IRQ_UART3_STATUS, IRQ_UART3_ERROR)
KINETIS_UART_DEFINE(4, 0x400EA000, F_BUS, 1, 1, IRQ_UART4_STATUS, IRQ_UART4_ERROR)
KINETIS_UART_DEFINE(5, 0x400EB000, F_BUS, 1, 1, IRQ_UART5_STATUS, IRQ_UART5_ERROR)
 */

//---------------------------------------- Hardware Initialisation ---------------------------------------

void teensy35_hardware_init() {

	//Declare the PORT driver;
	PORT = kinetis_PORT_create(PORT_A_MEMORY, GPIO_A_MEMORY, 5, 0x1000, 0x40);


}



// --------------------------------------- ERROR HANDLING ---------------------------------------

#include <kernel/systick.h>

void arch_handle_error(const char *msg) {

	arch_blink(50);

}

#include <kernel/arch/peripherals/kinetis/kinetis_mux.h>

//Notify that an error occurred;
void arch_blink(uint16_t delay) {

	//We will only use the kinetis PORT driver as a general port driver. Cache it as it;
	struct port_driver_t *port = (struct port_driver_t *)PORT;

	core_enable_interrupts();

	//Cache a pin configuration for pin C5 (LED);
	struct kinetis_pin_configuration_t config;

	//Cache the LED pin, on PORT C bit 5
	struct io_desc_t pin = PTC5;



	//Get the current configuration;
	port_driver_get_pin_config(port, &pin, &config);

	//Update the configuration for led blink
	config.mux_channel = 1;
	config.direction = PORT_OUTPUT;
	config.output_mode = PORT_HIGH_DRIVE;


	//Update the configuration;
	port_driver_configure_pin(port, &pin, &config);

	//Cache gpio registers;
	volatile uint32_t *set_register, *clr_register;

	//Cache set mask and clear mask;
	uint32_t set_mask, clr_mask;


	//Get set register and mask;
	port_driver_get_gpio_descriptor(port, GPIO_OUTPUT_SET_REGISTER, &pin, (volatile void **) &set_register, &set_mask);
	port_driver_get_gpio_descriptor(port, GPIO_OUTPUT_CLEAR_REGISTER, &pin, (volatile void **) &clr_register, &clr_mask);


	//Indefinately :
	while (true) {

		//Turn on the led;
		*set_register = set_mask;

		//Wait 10 ms;
		systick_wait(delay);

		//Turn off the LED;
		*clr_register = clr_mask;

		//Wait 10 ms;
		systick_wait(delay);

	}

}


//Notify that an error occurred;
void arch_count(size_t count) {

	//We will only use the kinetis PORT driver as a general port driver. Cache it as it;
	struct port_driver_t *port = (struct port_driver_t *)PORT;

	core_enable_interrupts();

	//Cache a pin configuration for pin C5 (LED);
	struct kinetis_pin_configuration_t config;

	//Cache the LED pin, on PORT C bit 5
	struct io_desc_t pin = PTC5;

	//Get the current configuration;
	port_driver_get_pin_config(port, &pin, &config);

	//Update the configuration for led blink
	config.mux_channel = 1;
	config.direction = PORT_OUTPUT;
	config.output_mode = PORT_HIGH_DRIVE;

	//Update the configuration;
	port_driver_configure_pin(port, &pin, &config);

	//Cache gpio registers;
	volatile uint32_t *set_register, *clr_register;

	//Cache set mask and clear mask;
	uint32_t set_mask, clr_mask;

	//Get set register and mask;

	port_driver_get_gpio_descriptor(port, GPIO_OUTPUT_SET_REGISTER, &pin, (volatile void **) &set_register, &set_mask);
	port_driver_get_gpio_descriptor(port, GPIO_OUTPUT_CLEAR_REGISTER, &pin, (volatile void **) &clr_register, &clr_mask);

	//Indefinately :
	while (true) {
		for (size_t c = count; c--;) {

			//Turn on the LED;
			*set_register = set_mask;

			//Wait 10 ms;
			systick_wait(250);

			//Turn off the LED;
			*clr_register = clr_mask;

			//Wait 10 ms;
			systick_wait(250);

		}

		systick_wait(2000);

	}

}



