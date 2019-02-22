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


#include <stdint.h>

#include "kx_sim.h"

#include <kernel_hooks.h>


/*------------------------------------------------------- Debug --------------------------------------------------------*/

/**
 * core_led_high : hardware code for turning on the led;
 */

void __dbg_led_high() {
	
	/*Output*/
	*(volatile uint32_t *) 0x400FF094 = 1 << 5;
	
	/*ALT 1*/
	*(volatile uint32_t *) 0x4004B014 |= 1 << 8;
	
	/*Set*/
	*(volatile uint32_t *) 0x400FF084 = 1 << 5;
}


/**
 * core_led_low : hardware code for turning off the led;
 */

void __dbg_led_low() {
	/*Output*/
	*(volatile uint32_t *) 0x400FF094 = 1 << 5;
	
	/*ALT 1*/
	*(volatile uint32_t *) 0x4004B014 |= 1 << 8;
	
	/*Clear*/
	*(volatile uint32_t *) 0x400FF088 = 1 << 5;
}



/*------------------------------------------------------ Watchdog -----------------------------------------------------*/

#define STCTRLH_WDOGEN ((uint16_t)(1<<0))

/* Watchdog Timer (WDOG)*/

#define WDOG_STCTRLH		(*(volatile uint16_t *)0x40052000) /* Watchdog Status and Control Register High*/
#define WDOG_STCTRLH_DISTESTWDOG	((uint16_t)0x4000)		/* Allows the WDOG's functional test mode to be disabled permanently.*/
#define WDOG_STCTRLH_BYTESEL(n)		((uint16_t)(((n) & 3) << 12))	/* selects the byte to be tested when the watchdog is in the byte test mode.*/
#define WDOG_STCTRLH_TESTSEL		((uint16_t)0x0800)
#define WDOG_STCTRLH_TESTWDOG		((uint16_t)0x0400)
#define WDOG_STCTRLH_WAITEN		((uint16_t)0x0080)
#define WDOG_STCTRLH_STOPEN		((uint16_t)0x0040)
#define WDOG_STCTRLH_DBGEN		((uint16_t)0x0020)
#define WDOG_STCTRLH_ALLOWUPDATE	((uint16_t)0x0010)
#define WDOG_STCTRLH_WINEN		((uint16_t)0x0008)
#define WDOG_STCTRLH_IRQRSTEN		((uint16_t)0x0004)
#define WDOG_STCTRLH_CLKSRC		((uint16_t)0x0002)
#define WDOG_STCTRLH_WDOGEN		((uint16_t)0x0001)
#define WDOG_STCTRLL		(*(volatile uint16_t *)0x40052002) /* Watchdog Status and Control Register Low*/
#define WDOG_TOVALH		(*(volatile uint16_t *)0x40052004) /* Watchdog Time-out Value Register High*/
#define WDOG_TOVALL		(*(volatile uint16_t *)0x40052006) /* Watchdog Time-out Value Register Low*/
#define WDOG_WINH		(*(volatile uint16_t *)0x40052008) /* Watchdog Window Register High*/
#define WDOG_WINL		(*(volatile uint16_t *)0x4005200A) /* Watchdog Window Register Low*/
#define WDOG_REFRESH		(*(volatile uint16_t *)0x4005200C) /* Watchdog Refresh register*/
#define WDOG_UNLOCK		(*(volatile uint16_t *)0x4005200E) /* Watchdog Unlock register*/
#define WDOG_UNLOCK_SEQ1		((uint16_t)0xC520)
#define WDOG_UNLOCK_SEQ2		((uint16_t)0xD928)
#define WDOG_TMROUTH		(*(volatile uint16_t *)0x40052010) /* Watchdog Timer Output Register High*/
#define WDOG_TMROUTL		(*(volatile uint16_t *)0x40052012) /* Watchdog Timer Output Register Low*/
#define WDOG_RSTCNT		(*(volatile uint16_t *)0x40052014) /* Watchdog Reset Count register*/
#define WDOG_PRESC		(*(volatile uint16_t *)0x40052016) /* Watchdog Prescaler register*/




/**
 * k64_wdog_disable : disables the watchdog;
 */

static void k64_wdog_disable() {

    /*Unlock the watchdog;*/
    WDOG_UNLOCK = WDOG_UNLOCK_SEQ1;
    WDOG_UNLOCK = WDOG_UNLOCK_SEQ2;
    __asm__ __volatile__ ("nop");

    /*Authorise update to the write only one WDOG registers;*/
    WDOG_STCTRLH = WDOG_STCTRLH_ALLOWUPDATE;

    /*Clear the enable bit in the watchdog;*/
    WDOG_STCTRLH &= ~STCTRLH_WDOGEN;

}

/*------------------------------------------------------ init -----------------------------------------------------*/


void __chip_init(void) {
	
	/*Disable the watchdog;*/
	k64_wdog_disable();
	
	/*Enable gating for all ports;*/
	sim_enable_PORTA_clock_gating();
	sim_enable_PORTB_clock_gating();
	sim_enable_PORTC_clock_gating();
	sim_enable_PORTD_clock_gating();
	sim_enable_PORTE_clock_gating();

	__kernel_entry();
	
}
