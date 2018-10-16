/*
  mk64fx512.c Part of TRACER

  Copyright (c) 2018 Raphaël Outhier

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


//#include "kinetis.h"

#include <kernel/startup.h>

#include <hard/chip/nxp/kinetis/kx_wdog.h>
#include <hard/chip/nxp/kinetis/kx_sim.h>

#include <kernel/panic.h>
#include <kernel/debug/log.h>
#include <kernel/debug/debug.h>
#include <kernel/clock/clock.h>



//------------------------------------------------------- Debug --------------------------------------------------------

/**
 * core_led_high : hardware code for turning on the led;
 */

void __dbg_led_high() {
	
	//Output
	*(volatile uint32_t *) 0x400FF094 = 1 << 5;
	
	//ALT 1
	*(volatile uint32_t *) 0x4004B014 |= 1 << 8;
	
	//Set
	*(volatile uint32_t *) 0x400FF084 = 1 << 5;
}


/**
 * core_led_low : hardware code for turning off the led;
 */

void __dbg_led_low() {
	//Output
	*(volatile uint32_t *) 0x400FF094 = 1 << 5;
	
	//ALT 1
	*(volatile uint32_t *) 0x4004B014 |= 1 << 8;
	
	//Clear
	*(volatile uint32_t *) 0x400FF088 = 1 << 5;
}


/**
 * core_delay : hardware code for waiting a certain number of milliseconds;
 */

void __dbg_delay_ms(uint32_t ms_counter) {
	
	while (ms_counter--) {
		//Count to;
		//for (volatile uint32_t i = 15000; i--;);
		for (volatile uint32_t i = 3000; i--;);
	}
	
}


/**
 * core_delay : hardware code for waiting a certain number of milliseconds;
 */

void __dbg_delay_us(uint32_t us_counter) {
	
	while (us_counter--) {
		//Count to;
		for (volatile uint32_t i = 3; i--;);
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



// Flash Security Setting. On Teensy 3.2, you can lock the MK20 chip to prevent
// anyone from reading your code.  You CAN still reprogram your Teensy while
// security is set, but the bootloader will be unable to respond to auto-reboot
// requests from Arduino. Pressing the program button will cause a full chip
// erase to gain access, because the bootloader chip is locked out.  Normally,
// erase occurs when uploading begins, so if you press the Program button
// accidentally, simply power cycling will run your program again.  When
// security is locked, any Program button press causes immediate full erase.
// Special care must be used with the Program button, because it must be made
// accessible to initiate reprogramming, but it must not be accidentally
// pressed when Teensy Loader is not being used to reprogram.  To set lock the
// security change this to 0xDC.  Teensy 3.0 and 3.1 do not support security lock.
#define FSEC 0xDE

// Flash Options
#define FOPT 0xF9
__attribute__ ((section(".flashconfig"), used))
const uint8_t flashconfigbytes[16] = {
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, FSEC, FOPT, 0xFF, 0xFF
};



void __proc_init(void) {
	
	/*
	 * First, disable the watchdog;
	 */
	
	//Disable the watchdog;
	k64_wdog_disable();
	
	//Enable gating for all ports;
	sim_enable_PORTA_clock_gating();
	sim_enable_PORTB_clock_gating();
	sim_enable_PORTC_clock_gating();
	sim_enable_PORTD_clock_gating();
	sim_enable_PORTE_clock_gating();
	
	startup_initialise_globals();
	
	//TODO FLASH CONFIGURATION TO DETERMINE INITIAL CONFIGURATION
	//TODO FLASH CONFIGURATION TO DETERMINE INITIAL CONFIGURATION
	//TODO FLASH CONFIGURATION TO DETERMINE INITIAL CONFIGURATION
	//TODO FLASH CONFIGURATION TO DETERMINE INITIAL CONFIGURATION
	//TODO FLASH CONFIGURATION TO DETERMINE INITIAL CONFIGURATION
	//TODO FLASH CONFIGURATION TO DETERMINE INITIAL CONFIGURATION
	//TODO FLASH CONFIGURATION TO DETERMINE INITIAL CONFIGURATION
	
	__krnl_init();
	
}