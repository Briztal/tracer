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


//#include "kinetis.h"

#include "mk64fx512.h"

#include <core/startup.h>
#include <core/debug.h>
#include <core/chip/nxp/kinetis/kx_wdog.h>
#include <core/chip/nxp/kinetis/kx_mcg.h>
#include <core/chip/nxp/kinetis/kx_sim.h>
#include <core/core.h>



//------------------------------------------------------- Debug --------------------------------------------------------

/**
 * core_led_high : hardware code for turning on the led;
 */

void core_led_high() {

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

void core_led_low() {

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

void core_delay_ms(uint32_t ms_counter) {

	while (ms_counter--) {
		//Count to;
		//for (volatile uint32_t i = 15000; i--;);
		for (volatile uint32_t i = 3000; i--;);
	}

}




/**
 * core_delay : hardware code for waiting a certain number of milliseconds;
 */

void core_delay_us(uint32_t us_counter) {

	while (us_counter--) {
		//Count to;
		for (volatile uint32_t i = 2; i--;);
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


extern void __program_start();


void __entry_point(void) {

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

	struct mcg_osc0_init osc_init = {

		//16M freq;
		.osc0_frequency = 16000000,

		//Low power;
		.low_power_mode = true,

		//Connection 1;
		.connection_id = OSC_CONNECTION_1,

		//Enable 2 and 8pf for teensy;
		.capacitor_2pf_enabled = true,
		.capacitor_4pf_enabled = false,
		.capacitor_8pf_enabled = true,
		.capacitor_16pf_enabled = false,

	};

	mcg_initialise_osc0(&osc_init);

	//Create the OSC initialisation struct;
	struct mcg_osc_config osc_conf = {

		//Enable output;
		.output_enabled = true,

		//External clock not selected;
		.external_clock_selected = false,

		//OSC0 selected;
		.osc0_selected = true,

		//Stay enabled in stop mode;
		.stay_enabled_in_stop_mode = true,

		//No interrupt for loss of clock;
		.loss_of_clock_generates_reset = false,

	};

	//Configure the oscillator;
	mcg_configure_osc(&osc_conf);



/*

	struct mcg_pll_config pll_conf = {

		//Enable external clock;
		.enable_mcg_pllclk = false,

		//No interrupt for loss of clock;
		.loss_of_lock_generates_interrupt = false,

		//No reset at loss of clock;
		.loss_of_lock_generates_reset = false,

		//Enable during stop mode;
		.enable_during_stop_mode = true,

		//OSC output is 16MHz, 16/4 = 4MHz;
		.external_divide_factor = 4,

		//Expected output frequency : 120MHz = 4MHz * 30;
		.output_multiplication_factor = 30,

	};

	//Configure the PLL;
	mcg_configure_pll(&pll_conf);

	//TODO CONGIGURE IRC;

	//Engage the PLL on external ref;
	mcg_enter_PEE();



	while (1) {
		core_led_high();
		core_delay_ms(100);
		core_led_low();
		core_delay_ms(50);
	}
 */


	//TODO PLL ENGAGED;

	//TODO DEBUG INIT;

	//TODO CORE BUS FLEX FLASH CLOCK DIVIDER;

	//TODO RENAME INTO CORE_INIT
	//TODO IN STARTUP.H

	__program_start();

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

	//Create the specs types;
	types kinetis_PIT_specs PIT_specs = {
		.MCR = PIT_MCR_REG,
		.nb_PITs = PIT_NB_TIMERS,
		.first_area = PIT_REGISTERS,
		.spacing = PIT_SPACING,
		.int_channels = int_channels,
		.clock_frequency = F_BUS,
	};

	//Define PIT chip;
	PIT = (types kinetis_PIT_driver *) kinetis_PIT_create(&PIT_specs);


	//Declare the K64 UART hardware config;
	types kinetis_UART_hw uart0_hw = {
		.registers =  (types kinetis_UART_registers *const) UART0_REGISTERS,
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
