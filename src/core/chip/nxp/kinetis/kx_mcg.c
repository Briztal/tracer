//
// Created by root on 8/23/18.
//

#include "kx_mcg.h"

#include <stdint.h>
#include <core/debug.h>

#define MCG_C1          ((volatile uint8_t *)0x40064000)
#define C1_CLKS         ((uint8_t)0xC0)
#define C1_FRDIV         ((uint8_t) ((1<<3)|(1<<4)|(1<<5)))
#define C1_CLKS_INT     ((uint8_t)0xD0)
#define C1_CLKS_EXT        ((uint8_t)0xE0)
#define C1_IREFS        ((uint8_t) (1<<2))
#define C1_IRCLKEN        ((uint8_t) (1<<1))
#define C1_IREFSTEN        ((uint8_t) (1<<0))


#define MCG_C2            ((volatile uint8_t *)0x40064001)
#define C2_IRCS         ((uint8_t) (1<<0))
#define C2_LP           ((uint8_t) (1<<1))
#define C2_EREFS        ((uint8_t) (1<<2))
#define C2_HGO            ((uint8_t) (1<<3))
#define C2_RANGE        ((uint8_t) ((1<<4) | (1<<5)))
#define C2_FCFTRIM      ((uint8_t) (1<<6))
#define C2_LOCRE0        ((uint8_t) (1<<7))


#define MCG_C3            ((volatile uint8_t *)0x40064002)


//C4 : FLL and Ftrim;
#define MCG_C4            ((volatile uint8_t *)0x40064003)
#define C4_SCFTRIM        ((uint8_t) (1<<0))
#define C4_FCTRIM        ((uint8_t)((1<<1) | (1<<2)| (1<<3)| (1<<4)))
#define C4_DRST_DRS        ((uint8_t)((1<<5)| (1<<6)))
#define C4_DMX32        ((uint8_t) (1<<7))


//C5 : the PLL first config register;
#define MCG_C5            ((volatile uint8_t *)0x40064004)
#define C5_PLLSTEN0        ((uint8_t) (1<<5))
#define C5_PLLCLKEN0        ((uint8_t) (1<<6))


#define MCG_C6          ((volatile uint8_t *)0x40064005)
#define C6_CME0            ((uint8_t) (1<<5))
#define C6_PLLS            ((uint8_t) (1<<6))
#define C6_LOLIE0       ((uint8_t) (1<<7))

//The MCG status register, RO;
#define MCG_S            ((volatile uint8_t *)0x40064006)
#define S_IRCST            ((uint8_t) (1<<0))
#define S_OSCINIT0      ((uint8_t) (1<<1))
#define S_IREFST        ((uint8_t) (1<<4))
#define S_PLLST        ((uint8_t) (1<<5))
#define S_LOCK0        ((uint8_t) (1<<6))
#define S_LOLS0        ((uint8_t) (1<<7))


#define MCG_SC            ((volatile uint8_t *)0x40064008)
#define SC_FCRDIV       ((uint8_t) 0x0E)

#define MCG_ATCVH        ((volatile uint8_t *)0x4006400A)
#define MCG_ATCVL        ((volatile uint8_t *)0x4006400B)

#define MCG_C7            ((volatile uint8_t *)0x4006400C)
#define C7_OSCSEL            ((uint8_t) 3)


//C8 : Enhanced monitoring register;
#define MCG_C8                ((volatile uint8_t *)0x4006400D)
#define C8_LOCS1            ((uint8_t) (1<<0))
#define C8_CME1        ((uint8_t) (1<<5))
#define C8_LOLRE        ((uint8_t) (1<<6))
#define C8_LOCRE1        ((uint8_t) (1<<7))





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
	if (frequency < 8000000)
		return (uint8_t) 1;

	//From 8MHz to 32MHz is mode 2;
	if (frequency < 32000000)
		return (uint8_t) 2;

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
	if (fmode == (uint8_t) -1) {

		return false;

	}

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
 * 	- actual frequency to 0;
 *
 * 	Fail-safe;
 */

static void stop_OSC() {

	//Disable output, disable in stop mode, disable all capacitors;
	*OSC_CR = 0;

	//Reset C2 OSC bits. External clock selected, low power, low freq;
	*MCG_C2 &= ~(C2_EREFS | C2_HGO | C2_RANGE);

	//Reset the frequency;
	osc_frequency = 0;

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
			while (1);

			//Stop here;
			return;

		}


		//Set EREFS to select the internal oscillator;
		//Save the last two bits of the frequency range;
		C2 |= ((fmode << 4) & C2_RANGE) | C2_EREFS;

		//If in high power mode, set HGO bit;
		if (!low_power) { C2 |= C2_HGO; }


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

	//Write CR and C2's values;
	*MCG_C2 = C2;

	*OSC_CR = CR;

	//Wait till OSC has completed its initialisation cycles;
	while (!(*MCG_S & S_OSCINIT0));

	//Update the frequency;
	osc_frequency = frequency;

}


/*
 * -------------------------------------------------------- FLL --------------------------------------------------------
 */

/**
 * The Frequency Lock Loop is a hardware component that receives and locks an input frequency between
 * 	31250 and 39062.5 Hz, to generate an output signal whose frequency is proportional to the input frequency;
 *
 * 	The output frequency range can be selected between four ranges (expressed in MHz), low (20-25), mid (40-50),
 * 	mid-high (60-75), and high (80-100);
 *
 * 	The input signal can also be selected, between the OSC external ref and the RTC slow clock;
 *
 * 	If the OSC external reference is selected, its frequency may not be in the required range. To support this case,
 * 	a frequency divider is included, to ensure that the input frequency is in the correct range.
 *
 * 	This divisor must be determined properly after the oscillator is selected;
 *
 * 	The configuration of the FLL comprises two steps :
 * 	- The hardware config, made when the FLL is not engaged; multiplication factor is selected, and clock divisor is
 * 		computed;
 * 	- The selection, made when the FLL is engaged by the MCG. The source is selected, and the actual output frequency
 * 		computed;
 */


//The FLL input frequency bounds;
#define FLL_INPUT_MIN 31250
#define FLL_INPUT_MAX 39062

//The fll reference input frequency;
#define FLL_INPUT_REF 32786

//The tolerance around the fll frequency;
#define FLL_INPUT_REF_TOLERANCE 2



//The FLL input freq; At init, in FEI mode;
static uint32_t mcg_fll_input_frequency = IRC_SLOW_FREQ;

//The FLL output frequency; At init, at FEI mode with precise ref, fll factor = 640;
#define FLL_OUTPUT_INIT (IRC_SLOW_FREQ * 640)
static uint32_t mcg_fll_output_frequency = FLL_OUTPUT_INIT;


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

void mcg_configure_fll_input(const bool external_ref, const uint8_t eref_divider_id) {


	//Cache C1;
	uint8_t C1 = *MCG_C1;

	//clear FRDIV bts;
	C1 &= ~C1_FRDIV;


	//Cache the input frequency;
	uint32_t input_f;

	//If the external reference is selected :
	if (external_ref) {

		//Clear IREFS;
		C1 &= ~C1_IREFS;

		//Cache the osc frequency;
		input_f = osc_frequency;

		//The PLL depends on the OSC external reference. If the OSC is not configured :
		if (input_f == 0) {

			//TODO ERROR OSC NOT INITIALISED;
			while (1);

			//Never reached
			return;

		}

		//Cache the divider index;
		const uint8_t divider_id = eref_divider_id;

		//If the divider is invalid :
		if (divider_id > 7) {

			//TODO ERROR Invalid divider;
			while (1);

			//Never reached
			return;

		}

		//Copy FRDIV in C1;
		C1 |= (divider_id << 3);


		/*
		 * Determine the frequency;
		 */

		//If range is 0 (low frequency), or if the slow oscillator is selected :
		if (((*MCG_C7 & C7_OSCSEL) == 2) || (((*MCG_C2 & C2_RANGE) >> 4) == 0)) {

			//The frequency division factor starts at 1;
			input_f >>= divider_id;

		} else {

			//The frequency division starts at 32 (= 2^5, shift 5 times more);
			input_f >>= (divider_id + 5);

		}


	} else {

		//If internal slow reference is selected :

		//Set IREFS;
		C1 |= C1_IREFS;

		//Update the input frequency;
		input_f = IRC_SLOW_FREQ;
	}

	//If the input frequency is not in the required bounds :
	if ((input_f < FLL_INPUT_MIN)  || (input_f > FLL_INPUT_MAX)) {

		//TODO ERROR PLL CANT WORK
		while (1);

		//Never reached
		return;

	}

	//Update C1;
	*MCG_C1 = C1;

	//Save the input frequency;
	mcg_fll_input_frequency = input_f;

}


/**
 * mgc_configure_fll_output : configures the FLL output to be in the required range. If the input frequency is known
 * 	to be the exact reference frequency (in a tolerance range), the FLL factor can be modified so that a precise output
 * 	frequency is generated;
 *
 * 	If the FLL output is not initialised, or if a too large error is observated when @exact_32786_ref is set, a core
 * 	error is issued;
 *
 * @param f_range : the required output range;
 * @param exact_32768_ref : must the FLL factor be adjusted for a standard output frequency ?
 */

void mgc_configure_fll_output(const enum mcg_fll_frequency_range f_range, const bool exact_32768_ref) {

	//Cache the input frequenct;
	const uint32_t input_f = mcg_fll_input_frequency;

	//If the FLL was not properly initialised :
	if (!input_f) {

		//TODO ERROR FLL NOT INITIALISED;
		while (1);

		//Never reached
		return;

	}

	//Cache C4, and clear DMX32 and DCO Range;
	uint8_t C4 = *MCG_C4 & ~(C4_DMX32 | C4_DRST_DRS);

	//If the exact frequency must be selected :
	if (exact_32768_ref) {

		//If the input frequency is not in the tolerance bounds :
		if ((input_f < FLL_INPUT_REF - FLL_INPUT_REF_TOLERANCE) ||
			(input_f > FLL_INPUT_REF + FLL_INPUT_REF_TOLERANCE)) {

			//TODO ERROR FLL MISCONFIGURATION;
			while (1);

			//Never reached
			return;


		}

		//If the input frequency is appropriate :

		//Set the DMX32 flag;
		C4 |= C4_DMX32;

	}

	//Save DCO range Select;
	C4 |= (f_range & 3) << 5;

	/*
	 * Determine the output frequency;
	 */

	//Declare the FLL factor;
	uint32_t fll_factor = 0;

	switch (f_range) {

		case FLL_RANGE_20_25_ref24_MHz:
			fll_factor = (exact_32768_ref) ? 732 : 640;
			break;
		case FLL_RANGE_40_50_ref48_MHz:
			fll_factor = (exact_32768_ref) ? 1464 : 1280;
			break;
		case FLL_RANGE_60_75_ref72_MHz:
			fll_factor = (exact_32768_ref) ? 2197 : 1920;
			break;
		case FLL_RANGE_80_100_ref96_MHz:
			fll_factor = (exact_32768_ref) ? 2929 : 2560;
			break;
		default:
			//TODO ERROR FLL BAD PARAMETER;
			while (1);

			//Never reached
			return;

	}

	//Write C4;
	*MCG_C4 = C4;

	//Determine and save the output frequency;
	mcg_fll_output_frequency = input_f * fll_factor;

}



/*
 * -------------------------------------------------------- PLL --------------------------------------------------------
 */

//the current frequency of the PLL, 0 at init, as PLL is disabled at startup;
static uint32_t pll_frequency = 0;


/**
 * mcg_configure_pll : initialises the PLL;
 *
 * 	Before attempting anything, it verifies that the OSC is configured, and that conversion factors are valid.
 *
 * 	Then, it verifies that the input frequency is between 2MHz and 4MHz, and if so, determines the output frequency;
 *
 * 	Any error during the previous section issues a core error;
 *
 * 	After that, it resets the PLL registers, to disable any clock output / interrupt trigger during configuration;
 *
 * 	It then updates C5 and C6, and waits for the lock to be acquired, and clears Loss of lock flag;
 *
 * 	If required, it then enables the loss of lock interrupt;
 *
 * @param config : the configuration struct;
 */

void mcg_configure_pll(const struct mcg_pll_config *config) {

	//Cache the osc frequency;
	uint32_t osc_f = osc_frequency;

	//The PLL depends on the OSC external reference. If the OSC is not configured :
	if (osc_f == 0) {

		//TODO ERROR OSC NOT INITIALISED;
		while (1);

		//Never reached
		return;

	}

	//Cache the divider;
	uint8_t divider = config->external_divide_factor;

	//If PRDIV0 is invalid :
	if ((divider == 0) || (divider > 25)) {

		//TODO ERROR Invalid external divider;
		while (1);

		//Never reached
		return;
	}

	//Determine the frequency after the input divider;
	uint32_t input_f = osc_f / divider;

	//If the frequency below 2MHz or beyond 4MHz :
	if ((input_f < 2000000) || (input_f > 4000000)) {

		//TODO ERROR input frequency;
		while (1);

		//Never reached
		return;

	}

	//Cache the output multiplication factor;
	uint8_t output_factor = config->output_multiplication_factor;

	//If the output factor is below 24 or beyond 55 :
	if ((output_factor < 24) || (output_factor > 55)) {

		//TODO ERROR invalid output factor;
		while (1);

		//Never reached
		return;

	}



	/*
	 * Reset the PLL so that setup do not cause interrupts or parasites hardware;
	 */

	//Disable loss of lock interrupt, select FLL, disable clock monitor;
	*MCG_C6 &= ~(C6_LOLIE0 | C6_PLLS | C6_CME0);

	//Disable PLLCLKEN0 and disable in stop mode;
	*MCG_C5 &= ~(C5_PLLCLKEN0 | C5_PLLSTEN0);


	/*
	 * C5 calculation;
	 */

	//Determine PRDIV0 and save it in the new C5 value;
	uint8_t C5 = (uint8_t) (divider - (uint8_t) 1);

	//Determine the output frequency;
	pll_frequency = input_f * output_factor;

	//If the PLL clock output must be enabled, set the appropriate bit;
	if (config->enable_mcg_pllclk) C5 |= C5_PLLCLKEN0;

	//If the PLL clock output must stay enabled during stop mode, set the appropriate bit;
	if (config->enable_during_stop_mode) C5 |= C5_PLLSTEN0;

	//Write C5;
	*MCG_C5 = C5;


	/*
	 * C6 calculation;
	 */

	//Determine VDIV0 and initialise C6.
	//subtract 24 and save only 5 lsb (unnecessary with previous check, but safer...);
	//External clock monitor disabled, FLL Selected;
	*MCG_C6 = ((uint8_t) (output_factor - (uint8_t) 24)) & (uint8_t) 0x1F;

	//Wait till PLL is locked;
	while (!(*MCG_S & S_LOCK0));

	//Clear loss of lock status;
	mcg_clear_pll_loss_of_lock_flag();

	//If the loss of lock must generate an interrupt :
	if (config->loss_of_lock_generates_interrupt) {

		//Enable the interrupt request in C6;
		*MCG_C6 |= C6_LOLIE0;

		//If a reset request must be generated :
		if (config->loss_of_lock_generates_reset) {

			//Enable the reset request in C8;
			*MCG_C8 |= C8_LOLRE;

		}

	}

}


/**
 * mcg_clear_pll_loss_of_lock_flag : clears the loss of lock flag in the status register;
 */

void mcg_clear_pll_loss_of_lock_flag() {

	//Write 1 to LOLS0 (W1C);
	*MCG_S |= S_LOLS0;

}



/*
 * ------------------------------------------- MCG operation modes selection -------------------------------------------
 */

//The current MGCOUT frequency; At startup, the FLL, clocked by the slow internal frequency, is selected;
static uint32_t mcg_out_frequency = FLL_OUTPUT_INIT;

/**
 * PLL Engaged External (PEE) : required OSC and PLL to be initialised;
 *
 * 	PLL Engaged External (PEE) mode is entered when all the following conditions occur:
 *
 * 	00 is written to C1[CLKS].
 * 	0 is written to C1[IREFS].
 * 	1 is written to C6[PLLS].
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

	//First, cache the PLL frequency;
	uint32_t freq = pll_frequency;

	//If the PLL is not initialised :
	if (freq == 0) {

		//TODO ERROR invalid output factor;
		while (1);

		//Never reached
		return;

	}

	//TODO FREQUENCY RANGE CHECK;

	//Cache C1;
	uint8_t C1 = *MCG_C1;

	//Clear CLKS bits;
	C1 &= ~C1_CLKS;

	//Clear IREFS;
	C1 &= ~C1_IREFS;

	//debug_led_low();
	//debug_delay_ms(100);

	//debug_led_high();

	debug_led_dump(*MCG_C5);

	//Select PLL;
	*MCG_C6 |= C6_PLLS;

	debug_led_high();


	while (1);

	while (!(*MCG_S & S_PLLST));


	debug_delay_ms(100);

	debug_led_dump(*MCG_S);


	//Update the frequency;
	mcg_out_frequency = freq;

	//Update C1;
	*MCG_C1 = C1;


	while (1) {
		debug_led_high();
		debug_delay_ms(100);
		debug_led_low();
		debug_delay_ms(50);
	}


}