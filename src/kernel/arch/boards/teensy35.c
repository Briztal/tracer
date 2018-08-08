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

//The teensy35 supports 6 UARTS;
//KINETIS_UART_DEFINE(0, 0x4006A000, F_CPU, 8, 8, IRQ_UART0_STATUS, IRQ_UART0_ERROR)

#define UART0_MEMORY 0x4006A000

struct kinetis_UART_driver_t *UART0;

void UART0_status_link() {
	kinetis_UART_status_interrupt(UART0);
}

void UART0_error_link() {
	kinetis_UART_error_interrupt(UART0);
}


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

	//Declare the kinetis UART hardware config;
	struct kinetis_UART_hw uart0_hw = {
		.registers =  (struct kinetis_UART_registers_t *const) UART0_MEMORY,
		.clock_frequency = F_CPU,
		.status_int_channel = IRQ_UART0_STATUS,
		.error_int_channel = IRQ_UART0_ERROR,
		.status_link = &UART0_status_link,
		.error_link = &UART0_error_link
	};

	//Create the UART;
	UART0 = kinetis_UART_create(&uart0_hw);

}


//-------------------------------------------------------- Debug -------------------------------------------------------

/**
 * teensy35_led_high : hardware code for turning on the led;
 */

void teensy35_led_high() {

	//Output
	*(volatile uint32_t *) 0x400FF094 = 1 << 5;

	//ALT 1
	*(volatile uint32_t *) 0x4004B014 |= 1 << 8;

	//Set
	*(volatile uint32_t *) 0x400FF084 = 1 << 5;

}


/**
 * teensy35_led_high : hardware code for turning off the led;
 */

void teensy35_led_low() {

	//Output
	*(volatile uint32_t *) 0x400FF094 = 1 << 5;

	//ALT 1
	*(volatile uint32_t *) 0x4004B014 |= 1 << 8;

	//Clear
	*(volatile uint32_t *) 0x400FF088 = 1 << 5;

}


/**
 * teensy35_led_high : hardware code for waiting a certain number of milliseconds;
 */

void teensy35_delay(uint32_t ms_counter) {

	while(ms_counter--) {
		//Count to;
		for (volatile uint32_t i = 15000; i--;);
	}

}





