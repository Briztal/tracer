/*
  mk64fx512.c Part of TRACER

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


#include "kinetis.h"

#include "mk64fx512.h"

#include <core/startup.h>
#include <core/debug.h>

#define KINETISK
#define __MK64FX512__
#define F_CPU 120000000



//------------------------------------------------------- Debug --------------------------------------------------------

/**
 * debug_led_high : hardware code for turning on the led;
 */

void debug_led_high() {

	SIM_SCGC5 |= (SIM_SCGC5_PORTA | SIM_SCGC5_PORTB | SIM_SCGC5_PORTC | SIM_SCGC5_PORTD | SIM_SCGC5_PORTE);

	//Output
	*(volatile uint32_t *) 0x400FF094 = 1 << 5;

	//ALT 1
	*(volatile uint32_t *) 0x4004B014 |= 1 << 8;

	//Set
	*(volatile uint32_t *) 0x400FF084 = 1 << 5;

}


/**
 * debug_led_low : hardware code for turning off the led;
 */

void debug_led_low() {

	//Output
	*(volatile uint32_t *) 0x400FF094 = 1 << 5;

	//ALT 1
	*(volatile uint32_t *) 0x4004B014 |= 1 << 8;

	//Clear
	*(volatile uint32_t *) 0x400FF088 = 1 << 5;

}


/**
 * debug_delay : hardware code for waiting a certain number of milliseconds;
 */

void debug_delay_ms(uint32_t ms_counter) {

	while (ms_counter--) {
		//Count to;
		for (volatile uint32_t i = 350; i--;);
	}

}




/**
 * debug_delay : hardware code for waiting a certain number of milliseconds;
 */

void debug_delay_us(uint32_t us_counter) {

	while (us_counter--) {
		//Count to;
		for (volatile uint32_t i = 1; i--;);
	}

}


/*
 * --------------------------------------- PORT ---------------------------------------
 */

//TODO PJRC TRIBUTE

//PORT memory locations. Start at 0x40049000, spaces of 0x1000 bytes
#define PORT_A_REGISTERS    (volatile void *)0x40049000
#define PORT_B_REGISTERS    (volatile void *)0x4004A000
#define PORT_C_REGISTERS    (volatile void *)0x4004B000
#define PORT_D_REGISTERS    (volatile void *)0x4004C000
#define PORT_E_REGISTERS    (volatile void *)0x4004D000

//PORT memory locations. Start at 0x400FF000, spaces of 0x40 bytes
#define GPIO_A_REGISTERS    (volatile void *)0x400FF000
#define GPIO_B_REGISTERS    (volatile void *)0x400FF040
#define GPIO_C_REGISTERS    (volatile void *)0x400FF080
#define GPIO_D_REGISTERS    (volatile void *)0x400FF0C0
#define GPIO_E_REGISTERS    (volatile void *)0x400FF100

struct kinetis_PORT_driver_t *PORT;


/*
 * --------------------------------------- PIT ---------------------------------------
 */

/*
 * The mk64fx512 comprises 4 pits;
 */
#define PIT_MCR_REG (void *)0x40037000
#define PIT_REGISTERS (void *)0x40037100
#define PIT_NB_TIMERS 4
#define PIT_SPACING (size_t)0x10

//Declare the PIT driver;
struct kinetis_PIT_driver *PIT;


/*
 * --------------------------------------- UART ---------------------------------------
 */

//The mk64fx512 supports 6 UARTS;
//KINETIS_UART_DEFINE(0, 0x4006A000, F_CPU, 8, 8, IRQ_UART0_STATUS, IRQ_UART0_ERROR)

#define UART0_REGISTERS 0x4006A000

/*
struct kinetis_UART_driver_t *UART0;

void UART0_status_link() {
	kinetis_UART_status_interrupt(UART0);
}

void UART0_error_link() {
	kinetis_UART_error_interrupt(UART0);
}
 */


/*
KINETIS_UART_DEFINE(1, 0x4006B000, F_CPU, 8, 8, IRQ_UART1_STATUS, IRQ_UART1_ERROR)
KINETIS_UART_DEFINE(2, 0x4006C000, F_BUS, 1, 1, IRQ_UART2_STATUS, IRQ_UART2_ERROR)
KINETIS_UART_DEFINE(3, 0x4006D000, F_BUS, 1, 1, IRQ_UART3_STATUS, IRQ_UART3_ERROR)
KINETIS_UART_DEFINE(4, 0x400EA000, F_BUS, 1, 1, IRQ_UART4_STATUS, IRQ_UART4_ERROR)
KINETIS_UART_DEFINE(5, 0x400EB000, F_BUS, 1, 1, IRQ_UART5_STATUS, IRQ_UART5_ERROR)
 */


extern uint8_t _start_rodata;
extern uint8_t _end_rodata;
extern uint8_t _start_data;
extern uint8_t _start_bss;
extern uint8_t _end_bss;


void __entry_point(void) {

	/*
	 * First, disable the watchdog;
	 */

	WDOG_UNLOCK = WDOG_UNLOCK_SEQ1;
	WDOG_UNLOCK = WDOG_UNLOCK_SEQ2;

	WDOG_STCTRLH = WDOG_STCTRLH_ALLOWUPDATE;

	SIM_SCGC5 |= (SIM_SCGC5_PORTA | SIM_SCGC5_PORTB | SIM_SCGC5_PORTC | SIM_SCGC5_PORTD | SIM_SCGC5_PORTE);


	/*
	 * Then, initialise global variables;
	 */

	__asm__ volatile("cpsid i":::"memory");

	debug_error("SUUS");

	while (1) {
		debug_led_high();
		debug_delay_ms(500);
		debug_led_low();
		debug_delay_ms(100);
	}

	startup_initialise_globals();



}

//---------------------------------------- Hardware Initialisation ---------------------------------------

void mk64fx512_hardware_init() {

	/*

	//Enable PORT clock gating;
	SIM_SCGC5 |= (SIM_SCGC5_PORTA | SIM_SCGC5_PORTB | SIM_SCGC5_PORTC | SIM_SCGC5_PORTD | SIM_SCGC5_PORTE);

	//Define the PORT driver;
	PORT = kinetis_PORT_create(PORT_A_REGISTERS, GPIO_A_REGISTERS, 5, 0x1000, 0x40);

	//Create the int channels array;
	uint8_t int_channels[4] = {IRQ_PIT_CH0, IRQ_PIT_CH1, IRQ_PIT_CH2, IRQ_PIT_CH3};

	//Create the specs struct;
	struct kinetis_PIT_specs PIT_specs = {
		.MCR = PIT_MCR_REG,
		.nb_PITs = PIT_NB_TIMERS,
		.first_area = PIT_REGISTERS,
		.spacing = PIT_SPACING,
		.int_channels = int_channels,
		.clock_frequency = F_BUS,
	};

	//Define PIT chip;
	PIT = (struct kinetis_PIT_driver *) kinetis_PIT_create(&PIT_specs);


	//Declare the K64 UART hardware config;
	struct kinetis_UART_hw uart0_hw = {
		.registers =  (struct kinetis_UART_registers *const) UART0_REGISTERS,
		.clock_frequency = F_CPU,
		.status_int_channel = IRQ_UART0_STATUS,
		.error_int_channel = IRQ_UART0_ERROR,
		.status_link = &UART0_status_link,
		.error_link = &UART0_error_link
	};

	//Turn on the UART0 clock;
	SIM_SCGC4 |= SIM_SCGC4_UART0;

	//Create the UART;
	UART0 = kinetis_UART_create(&uart0_hw);

	 */

}

