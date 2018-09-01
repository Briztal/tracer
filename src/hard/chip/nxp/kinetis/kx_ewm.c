/*
  kx_ewm.c Part of TRACER

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