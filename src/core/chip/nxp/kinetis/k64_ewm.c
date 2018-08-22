//
// Created by root on 8/22/18.
//

#include "k64_ewm.h"

#include <stdint.h>



//------------------------------------------------------- Macros -------------------------------------------------------

#define CTRL_EWMEN ((uint8_t)(1<<0))
#define CTRL_ASSIN ((uint8_t)(1<<1))
#define CTRL_INEN ((uint8_t)(1<<2))
#define CTRL_INTEN ((uint8_t)(1<<3))


//------------------------------------------------------- Structs ------------------------------------------------------

/**
 * The k64_ewm struct only contains the map of the module's register region;
 */

struct __attribute((packed)) k64_ewm_registers {

	//Control Register;
	volatile uint8_t CTRL;

	//Service Register;
	volatile uint8_t SRV;

	//Compare Low Register;
	volatile uint8_t CMPL;

	//Compare High Register;
	volatile uint8_t CMPH;

};


static struct k64_ewm_registers *const ewm = (void *)0x4061000;//TODO CONST GLOBAL;


//------------------------------------------------------ Functions -----------------------------------------------------

//Initialise the watchdog;
void k64_ewm_init() {

	//TODO SIM;
	//TODO DISABLE;
}

//Disable the watchdog timer;
void k64_ewm_disable() {

	//Clear the enable bit;
	ewm->CTRL &= ~CTRL_EWMEN;

}

//TODO OTHER FUNCTIONNALITIES