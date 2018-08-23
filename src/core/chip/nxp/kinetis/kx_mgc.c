//
// Created by root on 8/23/18.
//

#include "kx_mgc.h"

#include <stdint.h>

#define MCG_C1            ((volatile uint8_t *)0x40064000)
#define C1_CLKS            ((uint8_t)0xC0)
#define C1_CLKS_FPLL    ((uint8_t)0xC0)
#define C1_CLKS_INT    ((uint8_t)0xD0)
#define C1_CLKS_EXT    ((uint8_t)0xE0)


#define MCG_C2         	((volatile uint8_t *)0x40064001)
#define C2_IRCS         ((uint8_t) (1<<0))
#define C2_LP           ((uint8_t) (1<<1))
#define C2_EREFS       	((uint8_t) (1<<2))
#define C2_HGO         	((uint8_t) (1<<3))
#define C2_RANGE        ((uint8_t) ((1<<4) | (1<<5)))
#define C2_FCFTRIM      ((uint8_t) (1<<6))
#define C2_LOCRE0     	((uint8_t) (1<<7))



#define MCG_C3            ((volatile uint8_t *)0x40064002)
#define MCG_C4            ((volatile uint8_t *)0x40064003)
#define MCG_C5            ((volatile uint8_t *)0x40064004)


#define MCG_C6            ((volatile uint8_t *)0x40064005)
#define C6_PLLS        ((uint8_t) (1<<6))

//The MCG status register, RO;
#define MCG_S          	((volatile uint8_t *)0x40064006)
#define S_IRCST        	((uint8_t) (1<<0))
#define S_OSCINIT0       ((uint8_t) (1<<1))
#define S_IREFST       	((uint8_t) (1<<4))
#define S_PLLST       	((uint8_t) (1<<5))
#define S_LOCK0       	((uint8_t) (1<<6))
#define S_LOLS0       	((uint8_t) (1<<7))



#define MCG_SC        	((volatile uint8_t *)0x40064008)
#define SC_FCRDIV       ((uint8_t) 0x0E)

#define MCG_ATCVH        ((volatile uint8_t *)0x4006400A)
#define MCG_ATCVL        ((volatile uint8_t *)0x4006400B)

#define MCG_C7            ((volatile uint8_t *)0x4006400C)
#define MCG_C8            ((volatile uint8_t *)0x4006400D)



/*
 * -------------------------------------------------------- IRC --------------------------------------------------------
 */

//The current frequency of the IRC clock. At init, the slow clock is selected;
static uint32_t irc_frequency = IRC_SLOW_FREQ;


/**
 * mcg_configure_irc : configures the IRC;
 *
 * 	The slow or fast clock can be selected, and a divider can be set for the fast clock;
 *
 * 	Because the fast clock can't be set up when it is selected, the slow clock is selected when the function begins;
 *
 * 	The irc clock frequency global is updated according to the config.
 *
 * @param config : the required configuration;
 */

void mcg_configure_irc(const struct mcg_irc_config *config) {

	/*
	 * Select the slow internal clock;
	 *
	 * This operation is made in both cases, as the alteration of the fast clock divider is forbidden when
	 * 	the fast clock is selected;
	 */

	//Select the slow internal clock;
	*MCG_C2 &= ~C2_IRCS;

	//Set IRC freq to the slow frequency; Leaves times for IRCS sync and maintains integrity;
	irc_frequency = IRC_SLOW_FREQ;

	//If the fast IRC must be selected :
	if (config->fast_IRC_selected) {

		//Cache the index of the divider (log2(divider));
		uint8_t divisor = config->fast_irc_divider;

		//Zero unused divisor bits for safety;
		divisor &= (uint8_t) 0x7;

		//Get SC value;
		uint8_t SC = *MCG_SC;

		//Reset FCRDIV value;
		SC &= ~SC_FCRDIV;

		//Update the FRDIV value;
		SC |= divisor << 1;

		//Save SC;
		*MCG_SC = SC;

		//Determine the value of the IRC clock. The divisor is a power of 2 that we know the log2 of;
		irc_frequency = IRC_FAST_FREQ >> divisor;

		//Select the fast internal clock;
		*MCG_C2 |= C2_IRCS;

	}

}


/*
 * -------------------------------------------------------- OSC --------------------------------------------------------
 */

//The OSC control Register, RW;
#define OSC_CR            ((volatile uint8_t *)0x40065000)
#define CR_SC16P        ((uint8_t ) (1 << 0))
#define CR_SC8P        ((uint8_t ) (1 << 1))
#define CR_SC4P        ((uint8_t ) (1 << 2))
#define CR_SC2P        ((uint8_t ) (1 << 3))
#define CR_EREFSTEN        ((uint8_t ) (1 << 5))
#define CR_ERCLKEN        ((uint8_t ) (1 << 7))

//The current frequency of the OSC clock. At init, the oscillator is disabled;
static uint32_t osc_frequency = 0;


/**
 * osc_get_frequency_mode : determines the frequency mode of the provided frequency;
 *
 * @param frequency : the frequency to check;
 * @return -1 if frequency not supported, and 0 1 or 2 if valid, depending on the frequency value;
 */

static uint8_t osc_get_frequency_mode(const uint32_t frequency) {

	//The frequency must not be lower than 1kHz;
	if (frequency < 1000)
		return (uint8_t) -1;

	//From 1kHz to 32786Hz is mode 0;
	if (frequency < 32786)
		return 0;

	//From 32786Hz to 3MHz is not supported;
	if (frequency < 3000000)
		return (uint8_t) -1;

	//From 3MHz to 8MHz is mode 1;
	if (frequency < 3000000)
		return (uint8_t) -1;

	//From 8MHz to 32MHz is mode 2;
	if (frequency < 3000000)
		return (uint8_t) -1;

	//Beyond 32MHz is not supported;
	return (uint8_t) -1;

};

/**
 * osc_combination_valid : direct transcription of table "External Caystal/Resonator Connections"
 * 	(sorry for the typo but it is the actual title of the table. The datasheet has not been reviewed enough it seems!);
 *
 * @param fmode : the frequency mode, -1 if invalid, 0, 1 or 2 if valid;
 * @param low_power : must the oscillator be set in low power mode ?
 * @param cid : the id of the connection, as defined in the table and related chapter;
 * @return true if the combination is valid;
 */

static bool osc_combination_valid(const uint8_t fmode, const bool low_power, const enum mcg_osc_connection cid) {

	//If the frequency is invalid, fail;
	if (fmode == (uint8_t) -1)
		return false;

	if (fmode == 0) {

		if (low_power) {
			//Low frequency, low power must have connection 1;
			return cid == OSC_CONNECTION_1;

		} else {
			//Low frequency, high power must not have connection 1;
			return cid != OSC_CONNECTION_1;
		}

	} else {

		if (low_power) {
			//High frequency low power must not have connection 2;
			return cid != OSC_CONNECTION_2;

		} else {
			//High frequency high power must not have connection 1;
			return cid != OSC_CONNECTION_1;
		}

	}

	//Never reached;
	return false;

}


/**
 * stop_OSC : resets the OSC in its default value :
 * 	- output disabled;
 * 	- disabled in stop mode;
 * 	- all capacitors disabled;
 * 	- external clock selected;
 * 	- low power;
 * 	- low frequency;
 *
 * 	Failsafe;
 */

static void stop_OSC() {

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

void mcg_configure_osc(const struct mcg_osc_config *const config) {

	//If the OSC must be stopped :
	if (!config->output_enabled) {

		//Stop the osc;
		stop_OSC();

		//Stop here;
		return;

	}

	//Cache the frequency;
	const uint32_t frequency = config->osc_frequency;

	/*
	 * CR is used in both scenarios, initialise it now;;
	 */

	//Declare the CR new value; External clock will be enabled;
	uint8_t CR = CR_ERCLKEN;

	//If ext ref must stay enabled during stop mode, set the appropriate bit;
	if (config->stay_enabled_in_stop_mode) CR |= CR_EREFSTEN;


	/*
	 * C2 is used in both scenarios, initialise it now;;
	 */

	//Cache C2 and reset OSC bits. External clock selected, low power, low freq;
	uint8_t C2 = *MCG_C2 & ~(C2_EREFS | C2_HGO | C2_RANGE);

	//If reset must be triggered on OSC0 loss of clock, set the appropriate bit;
	if (config->loss_of_clock_generates_reset) C2 |= C2_LOCRE0;


	//If the internal oscillator must be selected :
	if (config->oscillator_selected) {

		//Determine the frequency mode;
		const uint8_t fmode = osc_get_frequency_mode(frequency);

		//Cache the power mode;
		const bool low_power = config->low_power_mode;

		/*
		 * A check is required to verity that the power / frequency / connection is valid;
		 */

		//If the test fails :
		 if (!osc_combination_valid(fmode, low_power, config->connection_id)) {

			 //Stop the osc;
			 stop_OSC();

			 //TODO ERROR;

			 //Stop here;
			 return;

		 }

		//Set EREFS to select the internal oscillator;
		//Save the last two bits of the frequency range;
		C2 |= ((fmode << 4) & ~C2_RANGE) | C2_EREFS;

		//If in high power mode, set HGO bit;
		if (!low_power) {C2 |= C2_HGO;}


		//If capacitors must be added, set their bit;
		if (config->capacitor_2pf_enabled) CR |= CR_SC2P;
		if (config->capacitor_4pf_enabled) CR |= CR_SC4P;
		if (config->capacitor_8pf_enabled) CR |= CR_SC8P;
		if (config->capacitor_16pf_enabled) CR |= CR_SC16P;

	}

	/*
	 * If the external clock must be selected :
	 * 	Don't touch C2, default config resets OSC and external clock is selected;
	 * 	Don't touch CR, all capacitors will be disabled;
	 */

	//Write CR and C2's values;;
	*OSC_CR = CR;
	*MCG_C2 = C2;

	//Wait till OSC has completed its initialisation cycles;
	while (!(*MCG_S &~ S_OSCINIT0));

	//Update the frequency;
	osc_frequency = frequency;

}


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

	//Enable MGCPLLCLK;
	bool enable_mcg_pllclk;

	//Shall an interrupt be triggered when a loss of lock is detected ?
	bool loss_of_lock_interrupt;

	//Enable during stop mode;
	bool enable_during_stop_mode;

	//The external reference divide factor, between 1 and 24;
	uint8_t external_divide_factor;

	//The PLL output multiply factor, between 24 and 55;
	uint8_t output_multiplication_factor;

};

//Configure the PLL;
void mcg_configure_pll(const struct mcg_pll_config *config) {

	//TODO CHECK THAT OSC IS CONFUGURED
	//TODO CHECK THAT OSC IS CONFUGURED
	//TODO CHECK THAT OSC IS CONFUGURED
	//TODO CHECK THAT OSC IS CONFUGURED
	//TODO CHECK THAT OSC IS CONFUGURED
	//TODO CHECK THAT OSC IS CONFUGURED
	//TODO CHECK THAT OSC IS CONFUGURED
	//TODO CHECK THAT OSC IS CONFUGURED
	//TODO CHECK THAT OSC IS CONFUGURED
	//TODO CHECK THAT OSC IS CONFUGURED
	//TODO CHECK THAT OSC IS CONFUGURED
	//TODO CHECK THAT OSC IS CONFUGURED
	//TODO CHECK THAT OSC IS CONFUGURED
	//TODO CHECK THAT OSC IS CONFUGURED
	//TODO CHECK THAT OSC IS CONFUGURED
	//TODO CHECK THAT OSC IS CONFUGURED
	//TODO CHECK THAT OSC IS CONFUGURED
	//TODO CHECK THAT OSC IS CONFUGURED
	//TODO CHECK THAT OSC IS CONFUGURED

	//

}

//Clear the pll loss of lock status;
void mcg_clear_pll_loss_of_lock_flag();