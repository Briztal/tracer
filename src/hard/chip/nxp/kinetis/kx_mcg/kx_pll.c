//
// Created by root on 9/23/18.
//

//--------------------------------------------------- Make parameters --------------------------------------------------

/*
 * Makefile must provide :
 * 	- STOP_MODE_EN : if set, the PLL stays enabled in stop mode;
 * 	- LOL0_INT : if set, an interrupt is triggered if the PLL lock is lost;
 * 	- LOL0_RESET : if set, a reset is triggered if the PLL lock is lost;
 */

#if !defined(STOP_MODE_EN) || !defined(LOL0_INT) || !defined(LOL0_RESET)

//Log
#error "Error, at least one macro argument hasn't been provided. Check the makefile;"

//Define macros. Allows debugging on IDE environment;
#define        STOP_MODE_EN  1

#define        LOL0_INT  1

#define        LOL0_RESET  1


#endif //!defined(OSC0_CONN_ID) || !defined(OSC0_FREQUENCY) || !defined(STOP_MODE_EN) || !defined(LOC0_RESET)


//------------------------------------------------------ Headers -------------------------------------------------------

#include "kx_mcg.h"

#include <kernel/panic.h>

/*
 * -------------------------------------------------------- PLL --------------------------------------------------------
 */


/**
 * mcg_configure_pll : initialises the PLL;
 *
 * 	Before attempting anything, it verifies that the OSC is configured, and that conversion factors are valid.
 *
 * 	Then, it verifies that the input frequency is between 2MHz and 4MHz, and if so, determines the output frequency;
 *
 * 	Any error during the previous section throws a core error;
 *
 * 	After that, it resets the PLL registers, to disable any clock output / interrupt trigger during configuration;
 *
 * 	It then updates C5 and C6, and waits for the lock to be acquired, and clears Loss of lock flag;
 *
 * 	If required, it then enables the loss of lock interrupt;
 *
 * @param config : the configuration struct;
 */

void kx_pll_configure(struct kx_pll_config config) {
	
	/*
	 * Check configuration parameters;
	 */
	
	//If PRDIV0 is invalid :
	if ((config.external_divide_factor == 0) || (config.external_divide_factor > 25)) {
		
		//Throw a core error;
		kernel_panic("kx_mc.c : kx_pll_configure : invalid external divide factor;");
		
		//Never reached
		return;
		
	}
	
	//If the output factor is below 24 or beyond 55 :
	if ((config.output_multiplication_factor < 24) || (config.output_multiplication_factor > 55)) {
		
		//Throw a core error;
		kernel_panic("kx_mc.c : kx_pll_configure : invalid output multiplication factor;");
		
		//Never reached
		return;
		
	}
	
	
	/*
	 * Reset the PLL so that setup do not cause interrupts or parasites hardware;
	 */
	
	//Disable loss of lock interrupt, select FLL, disable clock monitor;
	*MCG_C6 &= ~(C6_LOLIE0  | C6_CME0);
	
	
	/*
	 * C5 calculation;
	 */
	
	//Determine PRDIV0 and save it in the new C5 value;
	uint8_t C5 = (uint8_t) (config.external_divide_factor - (uint8_t) 1);
	
	
	//If the PLL clock output must be enabled, set the appropriate bit;
	//if (config->enable_mcg_pllclk) C5 |= C5_PLLCLKEN0;
	
	//If the PLL clock output must stay enabled during stop mode :
	#if (STOP_MODE_EN== 1)
	
	//Set the appropriate bit;
	C5 |= C5_PLLSTEN0;
	
	#endif //(STOP_MODE_EN== 1)
	
	
	//Write C5;
	*MCG_C5 = C5;
	
	
	/*
	 * C6 calculation;
	 */
	
	//Determine VDIV0 and initialise C6.
	//subtract 24 and save only 5 lsb (unnecessary with previous check, but safer...);
	//External clock monitor disabled, FLL Selected;
	*MCG_C6 = ((uint8_t) (config.output_multiplication_factor - (uint8_t) 24)) & (uint8_t) 0x1F;
	
	
	/*
	 * Loss of lock setup
	 */
	
	//Clear loss of lock status;
	kx_clear_pll_loss_of_lock_flag();
	
	//If the loss of lock must generate an interrupt :
	#if (LOL0_INT== 1)
	
	//Enable the interrupt request in C6;
	*MCG_C6 |= C6_LOLIE0;
	
	//If a reset request must be generated :
	#if (LOL0_RESET == 1)
	
	//Enable the reset request in C8;
	*MCG_C8 |= C8_LOLRE;
	
	#endif //(LOL0_RESET == 1)
	
	
	#endif //(LOL0_INT== 1)
	
	
}


//Stop the PLL;
void kx_pll_stop() {
	
	//Disable stop mode maintain;
	*MCG_C5 |= C5_PLLSTEN0;
	
	//Disable loss of lock interrupt, disable clock monitoring;
	*MCG_C6 &= ~(C6_LOLIE0  | C6_CME0);
	
	//Enable the reset request in C8;
	*MCG_C8 |= C8_LOLRE;
	
}


//Wait till PLL lock is acquired;
void kx_pll_acquire_lock() {
	
	//Wait till PLL is locked;
	while (!(*MCG_S & S_LOCK0));
	
}

/**
 * mcg_clear_pll_loss_of_lock_flag : clears the loss of lock flag in the status register;
 */

void kx_clear_pll_loss_of_lock_flag() {
	
	//Write 1 to LOLS0 (W1C);
	*MCG_S |= S_LOLS0;
	
}


//Wait till PLL acquires the PLLS clock;
void kx_pll_acquire_plls() {
	
	//While the FLL owns PLLST (S_PLLST clear);
	while (!(*MCG_S & S_PLLST));
}



