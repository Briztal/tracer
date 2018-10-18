/*
  kx_fll.c Part of TRACER

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

#include "kx_mcg.h"

#include <kernel/panic.h>
#include <kernel/debug/debug.h>


/*
 * -------------------------------------------------------- FLL --------------------------------------------------------
 */


/**
 * mcg_configure_fll_input : configures the input source of the FLL and determines the consequent input frequency;
 *
 * 	If the external reference is selected in input, a check will be made on if the oscillator is enabled, and
 * 	if the provided divider id determines an input frequency inside the PLL working window;
 *
 * 	If the slow internal reference is selected, @eref_divider_id is not used;
 *
 * @param external_ref : set if the external reference sould be used as input;
 * @param eref_divider_id : FRDIV in case of external ref selection;
 */

void kx_fll_configure(struct kx_fll_config config) {
	
	/*
	 * FLL input config : C1
	 */
	
	//If the external reference is selected :
	if (config.external_ref) {
	
		//Cache the divider index;
		const uint8_t divider_id = config.eref_divider_id;
		
		//If the divider is invalid :
		if (divider_id > 7) {
			
			//Throw a core error;
			kernel_panic("kx_mc.c : mcg_configure_fll_input : invalid divider;");
			
			//Never reached
			return;
			
		}
		
		//Cache C1;
		uint8_t C1 = *MCG_C1;
		
		//clear FRDIV bts;
		C1 &= ~C1_FRDIV;
		
		//Copy FRDIV in C1;
		C1 |= (divider_id << 3);
		
		//Update C1;
		*MCG_C1 = C1;
		
	}
	
	
	/*
	 * FLL output config : C4
	 */
	
	//Cache C4, and clear DMX32 and DCO Range;
	uint8_t C4 = *MCG_C4 & ~(C4_DMX32 | C4_DRST_DRS);
	
	//If the exact frequency must be selected :
	if (config.exact_32768_ref) {
		
		//If the input frequency is appropriate :
		
		//Set the DMX32 flag;
		C4 |= C4_DMX32;
		
	}
	
	//Save DCO range Select;
	C4 |= (config.f_range & 3) << 5;
	
	//Write C4;
	*MCG_C4 = C4;
	
}


//Wait till FLL acquires the PLLS clock;
void kx_fll_acquire_plls() {
	
	//Clear the PLLS bit in C6 to select the FLL;
	*MCG_C6 &= ~C6_PLLS;
	
	//While the PLL owns PLLST (S_PLLST set);
	while ((*MCG_S) & S_PLLST);
	
}


void kx_fll_set_external_clocking() {
	
	//Clear IREFS;
	*MCG_C1 &= ~C1_IREFS;
	
	//Wait till IREFST is set;
	while((*MCG_S) & S_IREFST);
	
}


void kx_fll_set_internal_clocking() {
	
	//Set IREFS;
	*MCG_C1 |= C1_IREFS;
	
	//Wait till IREFST is set;
	while(!((*MCG_S) & S_IREFST));
	
	//Wait 1 ms, so that FLL locks the internal reference frequency;
	debug_delay_ms(1);
	
}

