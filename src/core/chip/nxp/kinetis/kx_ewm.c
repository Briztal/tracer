//
// Created by root on 8/22/18.
//

#include "kx_ewm.h"

#include <stdint.h>



//------------------------------------------------------- Macros -------------------------------------------------------

// External Watchdog Monitor (EWM)

#define EWM_CTRL		(*(volatile uint8_t  *)0x40061000) // Control Register
#define EWM_SERV		(*(volatile uint8_t  *)0x40061001) // Service Register
#define EWM_CMPL		(*(volatile uint8_t  *)0x40061002) // Compare Low Register
#define EWM_CMPH		(*(volatile uint8_t  *)0x40061003) // Compare High Register

#define CTRL_EWMEN ((uint8_t)(1<<0))
#define CTRL_ASSIN ((uint8_t)(1<<1))
#define CTRL_INEN ((uint8_t)(1<<2))
#define CTRL_INTEN ((uint8_t)(1<<3))


//------------------------------------------------------ Functions -----------------------------------------------------

//Initialise the watchdog;
void k64_ewm_init() {

	//TODO SIM;
	//TODO DISABLE;
}

//Disable the watchdog timer;
void k64_ewm_disable() {

	//Clear the enable bit;
	EWM_CTRL &= ~CTRL_EWMEN;



}

//TODO OTHER FUNCTIONNALITIES