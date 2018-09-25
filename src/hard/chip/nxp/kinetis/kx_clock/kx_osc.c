/*
  kx_osc.c Part of TRACER

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

//--------------------------------------------------- Make parameters --------------------------------------------------

/*
 * Makefile must provide :
 *  - OSC0_CONN_ID : the resonator connection configuration. 0 for external clock, and 1, 2 or 3 for
 *  	related internal configurations.
 * 	- OSC0_FREQUENCY : the OSC resonator frequency;
 * 	- STOP_MODE_EN : if set, the oscillator stays enabled in stop mode;
 * 	- LOC0_RESET : if set, a reset is triggered if the OSC0's clock is lost;
 */

#if !defined(OSC0_CONN_ID) || !defined(OSC0_FREQUENCY) || !defined(STOP_MODE_EN) || !defined(LOC0_RESET)

//Log
#error "Error, at least one macro argument hasn't been provided. Check the makefile;"

//Define macros. Allows debugging on IDE environment;
#define        OSC0_FREQUENCY  16000000

#define        OSC0_CONN_ID  1

#define        STOP_MODE_EN  1

#define        LOC0_RESET  1


#endif //!defined(OSC0_CONN_ID) || !defined(OSC0_FREQUENCY) || !defined(STOP_MODE_EN) || !defined(LOC0_RESET)



/*
 * 	When OSC0_CONN_ID is not zero (internal clocking), the two following parameters must be updated :
 *	- LOW_POWER : the power mode of the oscillator, 1 for low power, 0 for high power;
 *	- CAPACITORS : a 4 bits mask, defining capacitors enabled : CP16 (bit 3) | CP8 (bit 2) | CP4 (bit 1) | CP2 (bit 0)
 */

#if (OSC0_CONN_ID != 0)

#if !defined(LOW_POWER) || !defined(CAPACITORS)

//Log
#error "Error, at least one macro argument hasn't been provided. Check the makefile;"

//Define macros. Allows debugging on IDE environment;
#define    LOW_POWER 1

#define    CAPACITORS 5

#endif //defined(LOW_POWER) || !defined(CAPACITORS)

#else // (CONNECTION_ID != 0)
//if the external clocking is seleted, define capacitors to 0;

#define CAPACITORS 0

#endif // (OSC0_CONN_ID != 0)


//------------------------------------------------- Internal parameters ------------------------------------------------

//The OSC1 frequency;
#define OSC1_FREQUENCY 32768

//The OSC control Register, RW;
//TODO MACRO PARAMETER;
#define OSC_CR            ((volatile uint8_t *)0x40065000)
#define CR_SC16P        ((uint8_t ) (1 << 0))
#define CR_SC8P        ((uint8_t ) (1 << 1))
#define CR_SC4P        ((uint8_t ) (1 << 2))
#define CR_SC2P        ((uint8_t ) (1 << 3))
#define CR_EREFSTEN        ((uint8_t ) (1 << 5))
#define CR_ERCLKEN        ((uint8_t ) (1 << 7))


//------------------------------------------------- Frequency modes ------------------------------------------------

//From 1kHz to 32786Hz is mode 0;
#define LOW_FREQUENCY_MODE 0

//From 3MHz to 8MHz is mode 1;
#define MEDIUM_FREQUENCY_MODE 1

//From 8MHz to 32MHz is mode 2;
#define HIGH_FREQUENCY_MODE 2


//------------------------------------------------- OSC0_CONN_ID check ------------------------------------------------

#if (OSC0_CONN_ID < 0) || (OSC0_CONN_ID > 3)

#error "Invalid connection id, check your makefile;"

#endif


//------------------------------------------------- Frequency mode check ------------------------------------------------

#if (OSC0_FREQUENCY < 1000)
//The frequency must not be lower than 1kHz;

#error "The osc frequency must not be lower than 1KHz, check your makefile;"

#elif (OSC0_FREQUENCY <= 32786)
//From 1kHz to 32786Hz is mode 0;

#define FMODE LOW_FREQUENCY_MODE

#elif (OSC0_FREQUENCY < 3000000)
//From 32786Hz to 3MHz is not supported;

#error "The osc frequency must not be between 32787 Hz and 3MHz, check your makefile;"

#elif (OSC0_FREQUENCY <= 8000000)
//From 3MHz to 8MHz is mode 1;

#define FMODE MEDIUM_FREQUENCY_MODE

#elif (OSC0_FREQUENCY <= 32000000)
//From 8MHz to 32MHz is mode 2;

#define FMODE HIGH_FREQUENCY_MODE

#else
//Beyond 32MHz is not supported;

#error "The osc frequency must not be superior to 32MHz, check your makefile;"

#endif

//-------------------------------------------------- OSC0 internal setup check --------------------------------------------------

#if (OSC0_CONN_ID != 0)

/*
 * If OSC0 internal crystal channel is selected, a check must be done to ensure :
 * 	- capacitors mask is consistent;
 * 	- connection type, power mode and frequency range are compatible;
 */


/*
 * ---------------- Capacitors check ----------------
 */

//The capacitors value must be a 4 bits value;
#if (CAPACITORS > 15)

#error "The capacitors mask must not be greater that 15, check your makefile;"

#endif


/*
 * ---------------- connection - power mode - frequency mode compatibility check ----------------
 */

//If the low frequency range is selected :
#if (FMODE == 0)


#if (LOW_POWER == 1)
//If the low power mode is selected :

	
	//If the connection id is not 1, the configuration is invalid;
#if (OSC0_CONN_ID != 1)

#error "Low frequency range and low power selected, connection must be of type 1"

#endif //(OSC0_CONN_ID != 1)


#else //(LOW_POWER == 1)
//If the high power mode is selected :


	//If the connection id is 1, the configuration is invalid;
#if (OSC0_CONN_ID == 1)

#error "Low frequency range and high power selected, connection must not be of type 1"

#endif //(OSC0_CONN_ID == 1)

#endif //(LOW_POWER == 1)


#else //(FMODE == 0)
//If the high frequency range is selected :

#if (LOW_POWER == 1)
//If the low power mode is selected :

//If the connection id is 2, the configuration is invalid;
#if (OSC0_CONN_ID == 2)

#error "High frequency range and low power selected, connection must not be of type 2"

#endif //(OSC0_CONN_ID == 2)


#else //(LOW_POWER == 1)
//If the high power mode is selected :

		//If the connection id is 1, the configuration is invalid;
#if (OSC0_CONN_ID == 1)

#error "High frequency range and high power selected, connection must not be of type 1"

#endif //(OSC0_CONN_ID == 1)

#endif //(LOW_POWER == 1)


#endif //(FMODE == 0)


/*
 * End of the OSC0 check;
 */
#endif //(OSC0_CONN_ID != 0)


//------------------------------------------------------ Headers -------------------------------------------------------

#include <kernel/panic.h>
#include "kx_mcg.h"

//TODO RTC LOSS OF CLOCK
//TODO RTC LOSS OF CLOCK
//TODO RTC LOSS OF CLOCK
//TODO RTC LOSS OF CLOCK
//TODO RTC LOSS OF CLOCK
//TODO RTC LOSS OF CLOCK
//TODO RTC LOSS OF CLOCK
//TODO RTC LOSS OF CLOCK
//TODO RTC LOSS OF CLOCK


//------------------------------------------------------ Channels ------------------------------------------------------

/*
 * Oscillator channels, OSC0, and OSC1  available to the MCG;
 */

const uint32_t osc_frequencies[2] = {OSC0_FREQUENCY, OSC1_FREQUENCY};


//-------------------------------------------------- OSC0 internal setup check --------------------------------------------------


/**
 * stop_OSC : resets the OSC in its default value :
 * 	- output disabled;
 * 	- disabled in stop mode;
 * 	- all capacitors disabled;
 * 	- external clock selected;
 * 	- low power;
 * 	- low frequency;
 * 	- actual frequency to 0;
 *
 * 	Fail-safe;
 */

void kx_osc_stop() {
	
	//Disable output, disable in stop mode, disable all capacitors;
	*OSC_CR = 0;
	
	//Reset C2 OSC bits. External clock selected, low power, low freq;
	*MCG_C2 &= ~(C2_EREFS | C2_HGO | C2_RANGE);
	
};


/**
 * mcg_configure_osc : configures the OSC.
 *
 * 	If an invalid configuration is provided or a stop is required, the OSC is stopped;
 *
 * 	If not, the OSC is effectively configured.
 *
 * 	If it must be configured as an effective oscillator, all config parameters are used.
 *
 * 	If it must be configured as a relay for external clock source, oscillator fields are reset to their default values;
 *
 * @param config : the OSC configuration struct;
 */

void kx_osc_configure(uint8_t channel) {
	
	
	/*
	 * C2 configuration
	 */
	
	//Declare the frequency and range;
	uint8_t f_mode;
	
	//Cache C2 and reset OSC bits. External clock selected, low power, low freq;
	uint8_t C2 = *MCG_C2 & ~(C2_EREFS | C2_HGO | C2_RANGE | C2_LOCRE0);
	
	
	//If OSC0 clock must be selected :
	if (channel == 0) {
		
		//Select oscillator 0;
		*MCG_C7 = 0;
		
		//Select the frequency mode;
		f_mode = (uint8_t) FMODE;
		
		//If the connection is not zero (internal crystal clocking)
		#if (OSC0_CONN_ID != 0)
		
		//Set EREFS to select the internal oscillator;
		C2 |= C2_EREFS;
		
		
		//If OSC0 in high power mode :
		#if (LOW_POWER == 0)
		
		set HGO bit;
		C2 |= C2_HGO;
		
		#endif //(LOW_POWER == 0)
		
		#endif //(OSC0_CONN_ID != 0)
		
	} else {
		//If OSC1 clock must be selected :
		
		//Select oscillator 1;
		*MCG_C7 = 1;
		
		//Select the low frequency range;
		f_mode = (uint8_t) LOW_FREQUENCY_MODE;
		
	}
	
	
	//Save the last two bits of the frequency range;
	C2 |= ((f_mode << 4) & C2_RANGE);
	
	
	//If reset must be triggered on OSC0 loss of clock,
	#if (LOC0_RESET == 1)
	
	//set the appropriate bit;
	C2 |= C2_LOCRE0;
	
	#endif //(LOC0_RESET == 1)
	
	//Write CR and C2's values;
	*MCG_C2 = C2;
	
	
	
	/*
	 * CR configuration
	 */
	
	//Declare the CR new value; External clock will be enabled;
	uint8_t CR = CR_ERCLKEN | (uint8_t) CAPACITORS;
	
	
	//If ext ref must stay enabled during stop mode,
	#if (STOP_MODE_EN == 1)
	
	//set the appropriate bit;
	CR |= CR_EREFSTEN;
	
	#endif //(STOP_MODE_EN == 1)
	
	
	//Save CR;
	*OSC_CR = CR;
	
	
	
	/*
	 * If the OSC0 signal is selected, and configured to use the internal crystal, we must wit till init is done;
	 */
	
	//If the internal crystal selected :
	#if (OSC0_CONN_ID != 0)
	
	//If OSC0 is selected :
	if (channel == 0) {
		
		//Wait till OSC has completed its initialisation cycles;
		while (!(*MCG_S & S_OSCINIT0));
		
	}
	
	#endif //(OSC0_CONN_ID != 0)
	
}
