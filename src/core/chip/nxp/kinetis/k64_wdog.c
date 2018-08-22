//
// Created by root on 8/22/18.
//

#include "k64_wdog.h"

#include <stdint.h>


//------------------------------------------------------- Macros -------------------------------------------------------

#define STCTRLH_WDOGEN ((uint16_t)(1<<0))



//------------------------------------------------------- Structs ------------------------------------------------------

/**
 * The k64_wdog struct only contains the map of the module's register region;
 */

struct  __attribute__ ((packed)) k64_wdog_registers {

	//Status and Control Register High;
	volatile uint16_t STCTRLH;

	//Status and Control Register Low;
	volatile uint16_t STCTRLL;

	//Time-out Value Register High;
	volatile uint16_t TOVALH;

	//Time-out Value Register Low;
	volatile uint16_t TOVALL;

	//Window Register High;
	volatile uint16_t WINH;

	//Window Register Low;
	volatile uint16_t WINL;

	//Refresh Register;
	volatile uint16_t REFRESH;

	//Unlock Register;
	volatile uint16_t UNLOCK;

	//Timer Output Register High;
	volatile uint16_t TMROUTH;

	//Timer Output Register Low;
	volatile uint16_t TMROUTL;

	//Reset Count Register;
	volatile uint16_t RSTCNT;

	//Prescaler Register;
	volatile uint16_t PRESC;

};

//The watchdog timer has a single instance;
static struct k64_wdog_registers *const wdog = TODO

//------------------------------------------------------ Functions -----------------------------------------------------

//Initialise the watchdog;
void k64_wdog_init() {
	//TODO SIM ENABLE;
}


/**
 * k64_wdog_disable : disables the watchdog;
 */

void k64_wdog_disable() {

	//Clear the enable bit in the watchdog;
	wdog->STCTRLH &= ~STCTRLH_WDOGEN;

}