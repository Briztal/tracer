//
// Created by root on 8/23/18.
//

#ifndef TRACER_KX_MGC_H
#define TRACER_KX_MGC_H


#include <stdbool.h>
#include <stdint.h>




/*
 * -------------------------------------------------------- IRC --------------------------------------------------------
 */

//The Internal Reference Clock provided two clocks, the fast IRC at 4MHz and the slow IRC at 32kHz
#define IRC_FAST_FREQ ((uint32_t) 4000000)
#define IRC_SLOW_FREQ ((uint32_t) 32000)


enum mcg_fast_irc_divider {
	IRC_DIV_1 = 0,
	IRC_DIV_2 = 1,
	IRC_DIV_4 = 2,
	IRC_DIV_8 = 3,
	IRC_DIV_16 = 4,
	IRC_DIV_32 = 5,
	IRC_DIV_64 = 6,
	IRC_DIV_128 = 7,
};


/**
 * The mcg irc configuration structure;
 *
 */
struct mcg_irc_config {

	//The fast clock is selected;
	bool fast_IRC_selected;

	/*
	 * The following option will be used only if fast_IRC_selected is set;
	 */

	//The divide factor log of the first clock;
	enum mcg_fast_irc_divider fast_irc_divider;


};

//Configure the irc;
void mcg_configure_irc(const struct mcg_irc_config *config);

//TODO AUTO TRIM

/*
 * -------------------------------------------------------- OSC --------------------------------------------------------
 */

/*
 * The different type of crystal/resonator connections;
 */

enum mcg_osc_connection {

	//Single crystal/resonator, no external resistor or capacitor;
	OSC_CONNECTION_1,

	//Crystal/resonator and resistor in parallel;
	OSC_CONNECTION_2,

	//Crystal/resonator, resistor and a couple of capacitor;
	OSC_CONNECTION_3,

};


/**
 * The mcg osc configuration structure.
 *
 * 	The OSC can be enabled or disabled, and maintained active or not in stop mode;
 *
 * 	Internal capacitors can be added to adjust filtering properties.
 *
 * 	The frequency of the filtering bloc between XTAL and EXTAL must be provided.
 *
 * 	The power mode (high gain or low power) must also be provided;
 *
 * 	As there are different connections, listed in the enum behind, the type of connection must be provided;
 *
 * 	The initialisation function will do some check to verify that the required parameter combination
 * 	is possible, as the connection type restricts the couple (frequency range, power mode);
 * 	TODO.
 */

struct mcg_osc_config {

	//The frequency of the oscillator; If zero, external clock reference will be selected;
	uint32_t osc_frequency;

	/*
	 * All Following options will only be evaluated if the triplet (frequency, power_mode, connection_id) is valid;
	 */

	//Set to enable the osc output; can be oscillator or external reference clock;
	bool output_enabled;

	//Set to maintain enabled in stop mode;
	bool stay_enabled_in_stop_mode;

	//Should a loss of clock generate a reset ?
	bool loss_of_clock_generates_reset;

	/*
	 * All Following options will only be evaluated if the frequency is not null;
	 */

	//The required power mode, low_power of high_gain if not;
	bool low_power_mode;

	//The type of the connection;
	enum mcg_osc_connection connection_id;


	//Add 2pF capacitor to the oscillator load;
	bool capacitor_2pf_enabled;

	//Add 4pF capacitor to the oscillator load;
	bool capacitor_4pf_enabled;

	//Add 8pF capacitor to the oscillator load;
	bool capacitor_8pf_enabled;

	//Add 16pF capacitor to the oscillator load;
	bool capacitor_16pf_enabled;

};

//Configure the oscillator;
void mcg_configure_osc(const struct mcg_osc_config *config);


/*
 * ------------------------------------------------------ OSC32K -------------------------------------------------------
 */

#define OSC32K_FREQ 32768

//TODO CONFIGURATION OF LOSS OF CLOCK FOR RTC OSC;


/*
 * -------------------------------------------------------- PLL --------------------------------------------------------
 */

/**
 * The mcg pll configuration structure contains all information required to configure the PLL completely.
 *
 * 	It has a loss of lock detection capability, that can generate an interrupt request;
 *
 * 	It can be routed to MGCOUT, or to MGCPLLCLK exit;
 *
 * 	It can be enabled or disabled during stop mode;
 *
 * 	It has frequency alteration capabilities :
 * 	- the external clock freqyency is divided by a factor D between 1 and 24, so that the PLL input frequency is
 * 		between 2MHz and 4MHz;
 * 	- the PLL's VCO output frequency is divided by a factor M between 24 and 55, so that the PLL output
 * 		frequency is M times the PLL input frequency;
 *
 * 	During the PLL configuration, a check will be made to determine if the PLL input frequency is between
 * 	2MHz and 4Mhz. If it is not, a core error will be issued;
 */

struct mcg_pll_config {

	//Shall an interrupt be triggered when a loss of lock is detected ?
	bool loss_of_lock_interrupt;

	//Enable MGCPLLCLK;
	bool enable_mcg_pllclk;

	//Enable during stop mode;
	bool enable_during_stop_mode;

	//The external reference divide factor, between 1 and 24;
	uint8_t external_divide_factor;

	//The PLL output multiply factor;
	uint8_t output_multiplication_factor;


};

//Configure the PLL;
void mcg_configure_pll(const struct mcg_pll_config *config);

//Clear the pll loss of lock status;
void mcg_clear_pll_loss_of_lock_flag();



/*
 * ------------------------------------------- MCG operation modes selection -------------------------------------------
 */

/*
 * The MCG has different modes of operation, determined by the clocking path, and the power state of
 * 	the PLL / FLL
 *
 * 	Following functions will attempt to enter the MCG in the required mode.
 *
 * 	Some transitions are not guaranteed to work, and it is preferable to go back in the safe mode, for example TODO
 * 	before entering any other mode;
 */


/**
 * PLL Engaged External (PEE) :
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

void mcg_enter_PEE();

#endif //TRACER_KX_MGC_H
