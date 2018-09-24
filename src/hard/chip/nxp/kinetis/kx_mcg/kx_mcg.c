/*
  kx_mcg.h Part of TRACER

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


#include <kernel/panic.h>
#include <kernel/log.h>
#include <kernel/debug/debug.h>
#include "kx_mcg.h"


//TODO ENABLE CLOCK MONITORING
//TODO ENABLE CLOCK MONITORING
//TODO ENABLE CLOCK MONITORING
//TODO ENABLE CLOCK MONITORING
//TODO ENABLE CLOCK MONITORING
//TODO ENABLE CLOCK MONITORING
//TODO ENABLE CLOCK MONITORING
//TODO ENABLE CLOCK MONITORING
//TODO ENABLE CLOCK MONITORING
//TODO ENABLE CLOCK MONITORING
//TODO ENABLE CLOCK MONITORING
//TODO ENABLE CLOCK MONITORING
//TODO ENABLE CLOCK MONITORING
//TODO ENABLE CLOCK MONITORING
//TODO ENABLE CLOCK MONITORING

/*
 * ------------------------------------------- MCG operation modes selection -------------------------------------------
 */

//The current MGCOUT frequency; At startup, the FLL (factor of 640), clocked by the slow IRC (32000Hz), is selected;
static uint32_t mcg_out_frequency = 20480000;




//----------------------------------------------------------------------------------------------------------------------

/**
 * FLL Engaged Internal (FEI) :
 *
 *  FLL engaged internal (FEI) is the default mode of operation and is entered when all the following condtions occur:
 *  - 00 is written to C1[CLKS];
 *  - 1 is written to C1[IREFS];
 *  - 0 is written to C6[PLLS];
 *
 * 	This mode can be entered from FBI;
 *
 * 	In FEI mode, MCGOUTCLK is derived from the FLL clock (DCOCLK) that is controlled by the 32
 * 	kHz Internal Reference Clock (IRC). The FLL loop will lock the DCO frequency to the FLL factor, as
 * 	selected by C4[DRST_DRS] and C4[DMX32] bits, times the internal reference frequency. See the
 * 	C4[DMX32] bit description for more details. In FEI mode, the PLL is disabled in a low-power state
 * 	unless C5[PLLCLKEN] is set .
 */

/**
 * Entering in FEI mode will select the IRC slow signal as input, and update the output frequency range;
 *
 * 	The IRC slow frequency is not at 32786 Hertz, the only field required is the frequency range;
 */

void mcg_enter_FEI() {
	
	kx_fll_set_internal_clocking();
	
	//To engage the FLL, we must wait till the PLL acquires the PLLS clock;
	kx_fll_acquire_plls();
	
	debug_delay_ms(1000);
	
	
	kernel_log("Clearing C1 %h", *MCG_S);
	
	//Clear CLKS bits; Will stay like this;
	*MCG_C1 &= ~C1_CLKS;
	
	
}


//----------------------------------------------------------------------------------------------------------------------

/**
 * FLL Engaged External (FEE) :
 *
 * 	This mode can be entered from FBE;
 *
 *  FLL engaged external (FEE) mode is entered when all the following conditions occur:
 *  - 00 is written to C1[CLKS];
 *  - 0 is written to C1[IREFS];
 *  - C1[FRDIV] must be written to divide external reference clock to be within the range of 31.25 kHz to 39.0625 kHz;
 *  - 0 is written to C6[PLLS];

 *  In FEE mode, MCGOUTCLK is derived from the FLL clock (DCOCLK) that is controlled by the
 *  external reference clock.
 *
 *  The FLL loop will lock the DCO frequency to the FLL factor, as selected by C4[DRST_DRS] and C4[DMX32] bits,
 *  times the external reference frequency, as specified by C1[FRDIV] and C2[RANGE].
 *
 *  See the C4[DMX32] bit description for more details. In FEE mode, the PLL is disabled in a low-power state
 *  unless C5[PLLCLKEN] is set.
 */


/**
 * Entering in FEE mode will select the OSC external reference signal as input, and update the output frequency range;
 */

void mcg_enter_FEE() {
	
	kernel_log("ENTERING FEE %h", *MCG_S);
	
	
	//Set the FLL in external clocking;
	kx_fll_set_external_clocking();
	
	kernel_log_("CLEARING PLLS");
	
	//Clear the PLLS bit in C6 to select the FLL;
	*MCG_C6 &= ~C6_PLLS;
	
	kernel_log_("ACQUIRING");
	
	//To engage the FLL, we must wait till the PLL acquires the PLLS clock;
	kx_fll_acquire_plls();
	
	
	
	kernel_log_("CACHING C1");
	
	//Cache C1;
	uint8_t C1 = *MCG_C1;
	
	//Clear CLKS bits; Will stay like this;
	C1 &= ~C1_CLKS;
	
	
	
	kernel_log_("WRITING C1");
	
	//Write C1;
	*MCG_C1 = C1;
	
}


//----------------------------------------------------------------------------------------------------------------------

/**
 * FLL Bypassed Internal (FBI) :
 *
 * 	FLL bypassed internal (FBI) mode is entered when all the following conditions occur :
 * 	- 01 is written to C1[CLKS];
 * 	- 1 is written to C1[IREFS];
 * 	- 0 is written to C6[PLLS];
 * 	- 0 is written to C2[LP];
 *
 * This mode can be entered from FBE, FEI or BLPI;
 *
 * In FBI mode, the MCGOUTCLK is derived either from the slow (32 kHz IRC) or fast (4 MHz IRC) internal reference
 * clock, as selected by the C2[IRCS] bit. The FLL is operational but its output is not used.
 *
 * This mode is useful to allow the FLL to acquire its target frequency while the MCGOUTCLK is driven from the
 * C2[IRCS] selected internal reference clock.
 *
 * The FLL clock (DCOCLK) is controlled by the slow internal reference clock, and the DCO clock frequency locks to
 * a multiplication factor, as selected by C4[DRST_DRS] and C4[DMX32] bits, times the internal reference frequency.
 *
 * See the C4[DMX32] bit description for more details.
 *
 * In FBI mode, the PLL is disabled in a low-power state unless C5[PLLCLKEN] is set .
 */

//Enter in FBI mode. The clock selected is determined by the current IRC configuration;
void mcg_enter_FBI() {
	
	//Cache C1;
	uint8_t C1 = *MCG_C1;
	
	//Clear CLKS bits;
	C1 &= ~C1_CLKS;
	
	//Set internal clocking;
	C1 |= C1_CLKS_INT;
	
	//Clear IREFS;
	C1 |= C1_IREFS;
	
	//Write C1;
	*MCG_C1 = C1;
	
	//Select the FLL;
	*MCG_C6 &= ~C6_PLLS;
	
	//Disable low power;
	*MCG_C2 &= ~C2_LP;
	
}


//----------------------------------------------------------------------------------------------------------------------

/**
 * FLL Bypassed External (FBE) :
 *
 * This mode can be entered from FBI, FEE, PBE or BLPE;
 *
 *  FLL bypassed external (FBE) mode is entered when all the following conditions occur :
 * 	- 10 is written to C1[CLKS];
 * 	- 0 is written to C1[IREFS];
 * 	- C1[FRDIV] must be written to divide external reference clock to be within the range of 31.25 kHz to 39.0625 kHz;
 * 	- 0 is written to C6[PLLS];
 * 	- 0 is written to C2[LP];
 *
 * In FBE mode, the MCGOUTCLK is derived from the OSCSEL external reference clock.
 *
 * The FLL is operational but its output is not used. This mode is useful to allow the FLL to acquire its target
 * frequency while the MCGOUTCLK is driven from the external reference clock.
 *
 * The FLL clock (DCOCLK) is controlled by the external reference clock, and the DCO clock frequency locks to a
 * multiplication factor, as selected by C4[DRST_DRS] and C4[DMX32] bits, times the divided external reference
 * frequency. See the C4[DMX32] bit description for more details.
 *
 * In FBI mode, the PLL is disabled in a low-power state unless C5[PLLCLKEN] is set .
 */

//Enter in FBE mode. The OSC external ref is selected;
void mcg_enter_FBE() {
	
	kernel_log("FBE :  %h", *MCG_S);
	
	//Cache C1;
	uint8_t C1 = *MCG_C1;
	
	//Clear CLKS bits;
	C1 &= ~C1_CLKS;
	
	//Set external clocking;
	C1 |= C1_CLKS_EXT;
	
	//Clear IREFS;
	C1 &= ~C1_IREFS;
	
	//Write C1;
	*MCG_C1 = C1;
	
	//Select the FLL;
	*MCG_C6 &= ~C6_PLLS;
	
	//Disable low power;
	*MCG_C2 &= ~C2_LP;
	
	kernel_log("FBE :  %h", *MCG_S);
	
}



//----------------------------------------------------------------------------------------------------------------------

/**
 * PLL Engaged External (PEE) : //TODO INIT PLL AT CALL TIME;
 *
 * 	This mode can be entered from PBE mode;
 *
 * 	PLL Engaged External (PEE) mode is entered when all the following conditions occur :
 * 	- 00 is written to C1[CLKS];
 * 	- 0 is written to C1[IREFS];
 * 	- 1 is written to C6[PLLS];
 *
 * 	In PEE mode, the MCGOUTCLK is derived from the output of PLL which is controlled by a external
 * 	reference clock.
 *
 * 	The PLL clock frequency locks to a multiplication factor, as specified by its corresponding VDIV,
 * 	times the selected PLL reference frequency, as specified by its corresponding PRDIV.
 *
 * 	The PLL's programmable reference divider must be configured to produce a valid PLL reference clock.
 * 	The FLL is disabled in a low-power state.
 */

void mcg_enter_PEE() {
	
	//To engage the PLL, we must wait till the PLL is locked;
	kx_pll_acquire_lock();
	
	//Select the PLL;
	*MCG_C6 |= C6_PLLS;
	
	//To engage the FLL, we must wait till the PLL acquires the PLLS clock;
	kx_pll_acquire_plls();
	
	
	//Cache C1;
	uint8_t C1 = *MCG_C1;
	
	//Clear CLKS bits; Will stay like this;
	C1 &= ~C1_CLKS;
	
	//Clear IREFS;
	C1 &= ~C1_IREFS;
	
	//Write C1;
	*MCG_C1 = C1;
	
}


//----------------------------------------------------------------------------------------------------------------------

/**
 * PLL Bypassed External (PBE) : //TODO INIT PLL AT CALL TIME;
 *
 * 	This mode can be entered from FBE and BLPE mode;
 *
 * 	PLL Bypassed External (PBE) mode is entered when all the following conditions occur :
 * 	- 10 is written to C1[CLKS];
 * 	- 0 is written to C1[IREFS];
 * 	- 1 is written to C6[PLLS];
 * 	- 0 is written to C2[LP];
 *
 * 	In PBE mode, MCGOUTCLK is derived from the OSCSEL external reference clock; the PLL is operational,
 * 	but its output clock is not used.
 *
 * 	This mode is useful to allow the PLL to acquire its target frequency while MCGOUTCLK is driven from the external
 * 	reference clock.
 *
 * 	The PLL clock frequency locks to a multiplication factor, as specified by its [VDIV], times the PLL reference
 * 	frequency, as specified by its [PRDIV].
 *
 * 	In preparation for transition to PEE, the PLL's programmable reference divider must be configured to produce a
 * 	valid PLL reference clock. The FLL is disabled in a low-power state.
 */

void mcg_enter_PBE() {
	
	//Cache C1;
	uint8_t C1 = *MCG_C1;
	
	//Clear CLKS bits;
	C1 &= ~C1_CLKS;
	
	//Set external clocking;
	C1 |= C1_CLKS_EXT;
	
	//Clear IREFS;
	C1 &= ~C1_IREFS;
	
	//Select the PLL;
	*MCG_C6 |= C6_PLLS;
	
	//Write C1;
	*MCG_C1 = C1;
	
	//Disable low power;
	*MCG_C2 &= ~C2_LP;
	
}

//----------------------------------------------------------------------------------------------------------------------

/*
 * Bypass Low Power Internal (BLPI) :
 *
 * 	This mode can be entered from FBI;
 *
 * 	Bypassed Low Power Internal (BLPI) mode is entered when all the following conditions occur :
 * 	- 01 is written to C1[CLKS];
 * 	- 1 is written to C1[IREFS];
 * 	- 0 is written to C6[PLLS];
 * 	- 1 is written to C2[LP];
 *
 *  In BLPI mode, MCGOUTCLK is derived from the internal reference clock. The FLL is disabled and PLL is disabled
 *  even if C5[PLLCLKEN] is set to 1.
 */

void mcg_enter_BLPI() {
	
	//Cache C1;
	uint8_t C1 = *MCG_C1;
	
	//Clear CLKS bits;
	C1 &= ~C1_CLKS;
	
	//Set internal clocking;
	C1 |= C1_CLKS_INT;
	
	//Set IREFS;
	C1 |= C1_IREFS;
	
	//Write C1;
	*MCG_C1 = C1;
	
	//Select the FLL;
	*MCG_C6 &= ~C6_PLLS;
	
	//Enable low power;
	*MCG_C2 |= C2_LP;
	
	
}


//----------------------------------------------------------------------------------------------------------------------

/*
 * Bypass Low Power External (BLPE) :
 *
 * 	This mode can be entered from PBE or FBE;
 *  Bypassed Low Power External (BLPE) mode is entered when all the following conditions occur :
 *  - 10 is written to C1[CLKS];
 *  - 0 is written to C1[IREFS];
 *  - 1 is written to C2[LP];
 *
 *  In BLPE mode, MCGOUTCLK is derived from the OSCSEL external reference clock. The FLL is disabled and PLL is
 *  disabled even if the C5[PLLCLKEN] is set to 1.
 */

void mcg_enter_BLPE() {
	
	//Cache C1;
	uint8_t C1 = *MCG_C1;
	
	//Clear CLKS bits;
	C1 &= ~C1_CLKS;
	
	//Set external clocking;
	C1 |= C1_CLKS_EXT;
	
	//Clear IREFS;
	C1 &= ~C1_IREFS;
	
	//Write C1;
	*MCG_C1 = C1;
	
	//Enable low power;
	*MCG_C2 |= C2_LP;
	
}


//------------------------------------------------------ Autotune ------------------------------------------------------

//There are 8 modes;
#define NB_MODES 8

//The current state of the MCG;
static enum mcg_mode current_mcg_mode = FEI;


//Functions to enter in different modes;
void (*enter_mode[NB_MODES])() = {
	
	//Level 0;
	&mcg_enter_FBI,
	&mcg_enter_FEI,
	&mcg_enter_BLPI,
	&mcg_enter_FBE,
	
	//Level 1;
	&mcg_enter_FEE,
	&mcg_enter_BLPE,
	&mcg_enter_PBE,
	
	//Level 2;
	&mcg_enter_PEE,
	
};


/*
 * TODO
 */

void mcg_return_FBI() {
	
	//First, configure the IRC to use the fast clock;
	kx_irc_configure(1);
	
	//Depending on the current state, different paths must be traversed;
	switch (current_mcg_mode) {
		
		//If PEE, go to PBE;
		case PEE:
			mcg_enter_PBE();
			
			//If PBE, FEE (comprised previous case) or BLPE, enter FBE;
		case PBE:
		case FEE:
		case BLPE:
			mcg_enter_FBE();
			
			//Now, we can safely enter FBI;
		default:
			mcg_enter_FBI();
		
	}
	
	//Update the current mode;
	current_mcg_mode = FBI;
	
}

/*
 * mcg_enter_mode : this function assumes the current mode is FBI (safe configuration mode), and traverses the
 * 	appropriate path to enter the required mode;
 */

void mcg_enter_mode(const enum mcg_mode mode) {
	
	//If the current mode is not FBI :
	if (current_mcg_mode != FBI) {
		
		//Panic, not supposed to be called when not in FBI;
		kernel_panic("mcg_enter_mode : called while not in FBI mode;");
		
	}
	
	/*
	 * If the mode can't be entered directly, we must execute transitions;
	 */
	//If target mode is in level 1 :
	if (mode > FBE) {
		
		kernel_log_("FBE");
		
		//Enter FBE;
		mcg_enter_FBE();
		
		//If target mode is in level 2 :
		if (mode == PEE) {
			kernel_log_("PBE");
			
			
			//Enter FBE;
			mcg_enter_PBE();
			
		}
		
	}
	
	//Enter to this mode directly;
	(*(enter_mode[mode]))();
	
	//Update the current mode;
	current_mcg_mode = mode;
	
}


static void mcg_configure_hardware(struct mcg_config *config) {
	
	/*
	 * IRC configuration :
	 */
	
	//If the IRC is used :
	if (config->irc_used) {
		
		//Configure the IRC to use the required channel;
		kx_irc_configure(config->irc_channel);
		
	}
	
	
	/*
	 * OSC configuration :
	 */
	
	
	kernel_log_("OSC");
	
	
	//If the OSC must be enabled :
	if (config->osc_enabled) {
		
		kernel_log_("OSC config");
		
		//Configure the OSC;
		kx_osc_configure(config->osc_channel);
		
	} else {
		
		kernel_log_("OSC stop");
		
		//If the OSC is not used, turn it off;
		kx_osc_stop();
		
	}
	
	
	/*
	 * FLL configuration;
	 */
	
	
	
	kernel_log_("FLL");
	
	//If the FLL is used :
	if (config->fll_used) {
		
		kernel_log_("FLL config");
		
		kx_fll_configure(config->fll_configuration);
		
	}
	
	
	/*
	 * PLL configuration;
	 */
	
	kernel_log_("PLL");
	
	//If the PLL is used :
	if (config->pll_used) {
		
		kernel_log_("PLL config");
		
		
		kx_pll_configure(config->pll_configuration);
		
	}
	
}


//tODO
#define NB_CONFIGURATION_FINDER 3

/*
 * This static array contains functions that can be used to search an adapted configuration;
 */

static bool (*const config_search[NB_CONFIGURATION_FINDER])(uint32_t target, struct mcg_config *config) = {
	
	/*
	 * First, internal clocking configs will be evaluated;
	 *
	 * !!!!!!!!!!!!!!!!!!! IMPORTANT !!!!!!!!!!!!!!!!!
	 *
	 * The IRC can always work and provide a proper configuration. It can be called with an
	 * empty config (config.frequency = 0).
	 * All others will trigger a kernel panic if they get called with an empty configuration;
	 */
	
	&kx_irc_find_configuration,
	
	//External clocking configs will be evaluated after;
	&kx_osc_find_configuration,
	
	//FLL clocking configs will then be evaluated;
	&kx_fll_find_configuration,
	
	/*
	//PLL clocking configs will finally be evaluated;
	&kx_pll_find_configuration,
	 */
	
};


void mcg_autotune(const uint32_t frequency_target) {
	
	//TODO NOTE : STATUS CHANGES DURING MODE TRANSITION (INTERNAL TO EXTERNAL RESETS THE INTERNAL REF CLOCK);
	//TODO NOTE : STATUS CHANGES DURING MODE TRANSITION (INTERNAL TO EXTERNAL RESETS THE INTERNAL REF CLOCK);
	//TODO NOTE : STATUS CHANGES DURING MODE TRANSITION (INTERNAL TO EXTERNAL RESETS THE INTERNAL REF CLOCK);
	//TODO NOTE : STATUS CHANGES DURING MODE TRANSITION (INTERNAL TO EXTERNAL RESETS THE INTERNAL REF CLOCK);
	//TODO NOTE : STATUS CHANGES DURING MODE TRANSITION (INTERNAL TO EXTERNAL RESETS THE INTERNAL REF CLOCK);
	//TODO NOTE : STATUS CHANGES DURING MODE TRANSITION (INTERNAL TO EXTERNAL RESETS THE INTERNAL REF CLOCK);
	//TODO NOTE : STATUS CHANGES DURING MODE TRANSITION (INTERNAL TO EXTERNAL RESETS THE INTERNAL REF CLOCK);
	//TODO NOTE : STATUS CHANGES DURING MODE TRANSITION (INTERNAL TO EXTERNAL RESETS THE INTERNAL REF CLOCK);
	//TODO NOTE : STATUS CHANGES DURING MODE TRANSITION (INTERNAL TO EXTERNAL RESETS THE INTERNAL REF CLOCK);
	//TODO NOTE : STATUS CHANGES DURING MODE TRANSITION (INTERNAL TO EXTERNAL RESETS THE INTERNAL REF CLOCK);
	//TODO NOTE : STATUS CHANGES DURING MODE TRANSITION (INTERNAL TO EXTERNAL RESETS THE INTERNAL REF CLOCK);
	//TODO NOTE : STATUS CHANGES DURING MODE TRANSITION (INTERNAL TO EXTERNAL RESETS THE INTERNAL REF CLOCK);
	
	//Declare the target configuration;
	struct mcg_config config;
	
	//TODO;
	bool exact = false;
	
	
	debug_delay_ms(1000);
	
	kernel_log_("Evaluating ...");
	
	//For each configuration finder :
	for (uint8_t finder_id = 0; finder_id < NB_CONFIGURATION_FINDER; finder_id++) {
		
		/*
		 * Search for a config that hits the target frequency;
		 * If a config closer than the current one is found, the config will be updated;
		 * If a config matching the target frequency is found, 1 will be returned, and 0 if not;
		 */
		
		exact = (*(config_search[finder_id]))(frequency_target, &config);
		
		//If a matching solution is found, stop here;
		if (exact)
			break;
		
	}
	
	kernel_log_("Done");
	
	debug_delay_ms(1000);
	
	kernel_log("Exact config found : %d, frequency : %d, mode %d", exact, config.frequency, config.mode);
	
	//TODO DISABLE CLOCKING MONITORING HERE;
	
	
	kernel_log_("Returning to FBI");
	
	//Now, go back to FLL bypassed Internal, to configure the hardware;
	mcg_return_FBI();
	
	kernel_log_("Configuring hardware ...");
	
	//Configure the required hardware;
	mcg_configure_hardware(&config);
	
	kernel_log("Entering mode %d ...", config.mode);
	
	//Enter the required mcg mode;
	mcg_enter_mode(config.mode);
	
	//Update the mcg frequency;
	mcg_out_frequency = config.frequency;
	
	debug_led_blink(1000);
	
	kernel_log_("STOPPING");
	
	
	while (1);
	
	
}