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


#include "kx_mcg.h"

#include <stdint.h>
#include <kernel/proc.h>
#include <kernel/panic.h>

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


//The Internal Reference Clock provided two clocks, the fast IRC at 4MHz and the slow IRC at 32kHz
#define IRC_FAST_FREQ ((uint32_t) 4000000)
#define IRC_SLOW_FREQ ((uint32_t) 32000)


//The current frequency of the IRC clock. At init, the slow clock is selected;
static uint32_t mcg_irc_frequency = IRC_SLOW_FREQ;


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
	mcg_irc_frequency = IRC_SLOW_FREQ;

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
		mcg_irc_frequency = IRC_FAST_FREQ >> divisor;

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
static uint32_t mcg_osc_frequency = 0;


//OSC OSC0 frequency;
static uint32_t mcg_osc_osc0_frequency = 0;

//OSC OSC0 power mode;
bool mcg_osc_osc0_low_power = 0;

//OSC OSC0 frequency range;
static uint8_t mcg_osc_osc0_f_range = 0;


//OSC external frequency;
static uint32_t mcg_osc_ext_frequency = 0;

//OSC external frequency range;
static uint8_t mcg_osc_ext_f_range = 0;


/**
 * osc_get_frequency_mode : determines the frequency mode of the provided frequency;
 *
 * @param frequency : the frequency to check;
 * @return -1 if frequency not supported, and 0 1 or 2 if valid, depending on the frequency value;
 */

static uint8_t osc_get_frequency_range(const uint32_t frequency) {

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
	mcg_osc_frequency = 0;

};


void mcg_initialise_osc_ext(const uint32_t ext_frequency) {

	//Determine the frequency mode;
	const uint8_t ext_mode = osc_get_frequency_range(ext_frequency);

	//If the external freq is invalid :
	if (ext_mode == (uint8_t) -1) {

		//Mark external mode unusable;
		mcg_osc_ext_frequency = 0;
		mcg_osc_ext_f_range = 0;

	} else {

		//If the external freq is valid, save its data;
		mcg_osc_ext_frequency = ext_frequency;
		mcg_osc_ext_f_range = ext_mode;
	}

}


void mcg_initialise_osc0(const struct mcg_osc0_init *init) {


	//Cache the osc0 frequency;
	const uint32_t osc0_freq = init->osc0_frequency;

	//Determine the frequency mode;
	const uint8_t fmode = osc_get_frequency_range(osc0_freq);

	//Cache the power mode;
	const bool low_power = init->low_power_mode;


	/*
	 * A check is required to verity that the power / frequency / connection is valid;
	 */

	//If the test fails :
	if (!osc_combination_valid(fmode, low_power, init->connection_id)) {

		//Mark the OSC0 unusable;
		mcg_osc_osc0_frequency = 0;
		mcg_osc_osc0_low_power = 0;
		mcg_osc_osc0_f_range = 0;

	} else {

		//Mark the OSC0 valid;
		mcg_osc_osc0_frequency = osc0_freq;
		mcg_osc_osc0_low_power = low_power;
		mcg_osc_osc0_f_range = fmode;

	}

	uint8_t CR = 0;

	//If capacitors must be added, set their bit;
	if (init->capacitor_2pf_enabled) CR |= CR_SC2P;
	if (init->capacitor_4pf_enabled) CR |= CR_SC4P;
	if (init->capacitor_8pf_enabled) CR |= CR_SC8P;
	if (init->capacitor_16pf_enabled) CR |= CR_SC16P;

	//Write CR to save capacitors values;
	*OSC_CR = CR;

}


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


	//Declare the frequency and range;
	uint32_t frequency;
	uint8_t f_range;

	//Cache C2 and reset OSC bits. External clock selected, low power, low freq;
	uint8_t C2 = *MCG_C2 & ~(C2_EREFS | C2_HGO | C2_RANGE | C2_LOCRE0);

	//If the OSC's external clock must be selected :
	if (config->external_clock_selected) {

		//Select the external frequency, and the external f_range;
		frequency = mcg_osc_ext_frequency;
		f_range = mcg_osc_ext_f_range;

	} else {

		//Set EREFS to select the internal oscillator;
		C2 |= C2_EREFS;

		//If OSC0 must be selected :
		if (config->osc0_selected) {

			//Select OSC0 frequency, and the external f_range;
			frequency = mcg_osc_osc0_frequency;
			f_range = mcg_osc_osc0_f_range;

			//If OSC0 in high power mode, set HGO bit;
			if (!mcg_osc_osc0_low_power)  C2 |= C2_HGO;

		} else {

			//Select OSC0 frequency, and the external f_range;
			frequency = OSC32K_FREQ;
			f_range = mcg_osc_osc0_f_range;

		}

	}

	//If the required frequency is null (module not initialised or not working) :
	if (!frequency) {

		//Throw a core error;
		kernel_panic("kx_mc.c : mcg_configure_osc : null frequency, OSC not initialised for the required mode;");

		//Never reached
		return;

	}

	//Update the oscillator frequency;
	mcg_osc_frequency = frequency;


	/*
	 * C2
	 */

	//Save the last two bits of the frequency range;
	C2 |= ((f_range << 4) & C2_RANGE);

	//If reset must be triggered on OSC0 loss of clock, set the appropriate bit;
	if (config->loss_of_clock_generates_reset) C2 |= C2_LOCRE0;

	//Write CR and C2's values;
	*MCG_C2 = C2;


	/*
	 * CR
	 */

	//Declare the CR new value; External clock will be enabled;
	uint8_t CR = CR_ERCLKEN;

	//If ext ref must stay enabled during stop mode, set the appropriate bit;
	if (config->stay_enabled_in_stop_mode) CR |= CR_EREFSTEN;

	//Save CR;
	*OSC_CR = CR;


	//Wait till OSC has completed its initialisation cycles;
	while (!(*MCG_S & S_OSCINIT0));

	//Update the frequency;
	mcg_osc_frequency = frequency;

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
static uint32_t mcg_fll_input_frequency = 0;//IRC_SLOW_FREQ;

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

void mcg_configure_fll_input(struct fll_input_config config) {


	//Cache C1;
	uint8_t C1 = *MCG_C1;

	//clear FRDIV bts;
	C1 &= ~C1_FRDIV;


	//Cache the input frequency;
	uint32_t input_f;

	//If the external reference is selected :
	if (config.external_ref) {

		//Clear IREFS;
		C1 &= ~C1_IREFS;

		//Cache the osc frequency;
		input_f = mcg_osc_frequency;

		//The FLL depends on the OSC external reference. If the OSC is not configured :
		if (input_f == 0) {

			//Throw a core error;
			kernel_panic("kx_mc.c : mcg_configure_fll_input : null frequency, OSC not initialised for the required mode;");

			//Never reached
			return;

		}

		//Cache the divider index;
		const uint8_t divider_id = config.eref_divider_id;

		//If the divider is invalid :
		if (divider_id > 7) {

			//Throw a core error;
			kernel_panic("kx_mc.c : mcg_configure_fll_input : invalid divider;");

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
	if ((input_f < FLL_INPUT_MIN) || (input_f > FLL_INPUT_MAX)) {

		//Throw a core error;
		kernel_panic("kx_mc.c : mcg_configure_fll_input : invalid input frequency;");

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
 * 	error is thrown;
 *
 * @param f_range : the required output range;
 * @param exact_32768_ref : must the FLL factor be adjusted for a standard output frequency ?
 */

void mcg_configure_fll_output(struct fll_output_config config) {

	//Cache the input frequenct;
	const uint32_t input_f = mcg_fll_input_frequency;

	//If the FLL was not properly initialised :
	if (!input_f) {

		//Throw a core error;
		kernel_panic("kx_mc.c : mcg_configure_fll_output : FLL input not initialised;");

		//Never reached
		return;

	}

	//Cache C4, and clear DMX32 and DCO Range;
	uint8_t C4 = *MCG_C4 & ~(C4_DMX32 | C4_DRST_DRS);

	//If the exact frequency must be selected :
	if (config.exact_32768_ref) {

		//If the input frequency is not in the tolerance bounds :
		if ((input_f < FLL_INPUT_REF - FLL_INPUT_REF_TOLERANCE) ||
			(input_f > FLL_INPUT_REF + FLL_INPUT_REF_TOLERANCE)) {

			//Throw a core error;
			kernel_panic("kx_mc.c : mcg_configure_fll_output : input not clocked at reference frequency;");

			//Never reached
			return;


		}

		//If the input frequency is appropriate :

		//Set the DMX32 flag;
		C4 |= C4_DMX32;

	}

	//Save DCO range Select;
	C4 |= (config.f_range & 3) << 5;

	/*
	 * Determine the output frequency;
	 */

	//Declare the FLL factor;
	uint32_t fll_factor = 0;

	switch (config.f_range) {

		case FLL_RANGE_20_25_ref24_MHz:
			fll_factor = (config.exact_32768_ref) ? 732 : 640;
			break;
		case FLL_RANGE_40_50_ref48_MHz:
			fll_factor = (config.exact_32768_ref) ? 1464 : 1280;
			break;
		case FLL_RANGE_60_75_ref72_MHz:
			fll_factor = (config.exact_32768_ref) ? 2197 : 1920;
			break;
		case FLL_RANGE_80_100_ref96_MHz:
			fll_factor = (config.exact_32768_ref) ? 2929 : 2560;
			break;
		default:

			//Throw a core error;
			kernel_panic("kx_mc.c : mcg_configure_fll_output : invalid output range;");

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
static uint32_t mcg_pll_frequency = 0;


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

void mcg_configure_pll(const struct mcg_pll_config *config) {

	//Cache the osc frequency;
	uint32_t osc_f = mcg_osc_frequency;

	//The PLL depends on the OSC external reference. If the OSC is not configured :
	if (osc_f == 0) {

		//Throw a core error;
		kernel_panic("kx_mc.c : mcg_configure_pll : OSC not configured;");

		//Never reached
		return;

	}

	//Cache the divider;
	uint8_t divider = config->external_divide_factor;

	//If PRDIV0 is invalid :
	if ((divider == 0) || (divider > 25)) {

		//Throw a core error;
		kernel_panic("kx_mc.c : mcg_configure_pll : invalid external divide factor;");

		//Never reached
		return;

	}

	//Determine the frequency after the input divider;
	uint32_t input_f = osc_f / divider;

	//If the frequency below 2MHz or beyond 4MHz :
	if ((input_f < 2000000) || (input_f > 4000000)) {

		//Throw a core error;
		kernel_panic("kx_mc.c : mcg_configure_pll : invalid input frequency;");

		//Never reached
		return;

	}

	//Cache the output multiplication factor;
	uint8_t output_factor = config->output_multiplication_factor;

	//If the output factor is below 24 or beyond 55 :
	if ((output_factor < 24) || (output_factor > 55)) {

		//Throw a core error;
		kernel_panic("kx_mc.c : mcg_configure_pll : invalid output multiplication factor;");

		//Never reached
		return;

	}



	/*
	 * Reset the PLL so that setup do not cause interrupts or parasites hardware;
	 */

	//Disable loss of lock interrupt, select FLL, disable clock monitor;
	*MCG_C6 &= ~(C6_LOLIE0 | C6_PLLS | C6_CME0);



	/*
	 * C5 calculation;
	 */

	//Determine PRDIV0 and save it in the new C5 value;
	uint8_t C5 = (uint8_t) (divider - (uint8_t) 1);

	//Determine the output frequency;
	mcg_pll_frequency = input_f * output_factor;

	//If the PLL clock output must be enabled, set the appropriate bit;
	//if (config->enable_mcg_pllclk) C5 |= C5_PLLCLKEN0;

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


/*
 * The frequency selection function (defined by a macro) will update the mcg_out_frequency to the required one if
 * 	the frequency is not null.
 *
 * 	If it is, a core error will be thrown;
 */

void mcg_out_select_frequency(uint32_t frequency) {

	//If the frequency is null :
	if (!frequency) {

		//Throw a core error;
		kernel_panic("kx_mc.c : mcg_out_select_frequency : null frequency;");

		//Never reached
		return;

	}

	//Update the mcg out frequency;
	mcg_out_frequency = frequency;

}


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

void mcg_enter_FEI(enum mcg_fll_frequency_range range) {

	//TODO FORBIDDEN MODES;
	//TODO FREQUENCY UPDATE;


	//The FLL will be clocked internally;
	struct fll_input_config input_config = {
		.external_ref = false,
	};

	//Configure the fll input;
	mcg_configure_fll_input(input_config);

	//The FLL will have the required range, with no exact ref;
	struct fll_output_config output_config = {
		.exact_32768_ref = false,
		.f_range = range,
	};

	//Configure the fll output frequency range;
	mcg_configure_fll_output(output_config);
	//TODO WAIT TILL FLL LOCK ?

	//Update the output frequency to the PLL's one;
	mcg_out_select_frequency(mcg_fll_output_frequency);

	//Cache C1;
	uint8_t C1 = *MCG_C1;

	//Clear CLKS bits; Will stay like this;
	C1 &= ~C1_CLKS;

	//Set IREFS;
	C1 |= C1_IREFS;

	//Clear the PLLS bit in C6 to select the FLL;
	*MCG_C6 &= ~C6_PLLS;

	//Write C1;
	*MCG_C1 = C1;

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

void mcg_enter_FEE(struct FEE_config config) {

//TODO FORBIDDEN MODES;
	//TODO FLL CONFIG

	//TODO FREQUENCY UPDATE;


	//The FLL will be clocked externally with the provided divider;
	struct fll_input_config input_config = {
		.external_ref = true,
		.eref_divider_id = config.eref_id,
	};

	//Configure the fll input;
	mcg_configure_fll_input(input_config);

	//The FLL will have the required range, with eventually the exact ref;
	struct fll_output_config output_config = {
		.exact_32768_ref = false,
		.f_range = config.f_range,
	};

	//Configure the fll output frequency range;
	mcg_configure_fll_output(output_config);

	//Update the output frequency to the PLL's one;
	mcg_out_select_frequency(mcg_fll_output_frequency);

	//Cache C1;
	uint8_t C1 = *MCG_C1;

	//Clear CLKS bits; Will stay like this;
	C1 &= ~C1_CLKS;

	//Clear IREFS;
	C1 &= ~C1_IREFS;

	//Clear the PLLS bit in C6 to select the FLL;
	*MCG_C6 &= ~C6_PLLS;

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

	//Update the output frequency to the IRC's one
	mcg_out_select_frequency(mcg_irc_frequency);

	//Cache C1;
	uint8_t C1 = *MCG_C1;

	//Clear CLKS bits;
	C1 &= ~C1_CLKS;

	//Set internal clocking;
	C1 |= C1_CLKS_INT;

	//Clear IREFS;
	C1 |= C1_IREFS;

	//Select the FLL;
	*MCG_C6 &= ~C6_PLLS;

	//Write C1;
	*MCG_C1 = C1;

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

	//Update the output frequency to the OSC's one
	mcg_out_select_frequency(mcg_osc_frequency);

	//Cache C1;
	uint8_t C1 = *MCG_C1;

	//Clear CLKS bits;
	C1 &= ~C1_CLKS;

	//Set external clocking;
	C1 |= C1_CLKS_EXT;

	//Clear IREFS;
	C1 &= ~C1_IREFS;

	//Select the FLL;
	*MCG_C6 &= ~C6_PLLS;

	//Write C1;
	*MCG_C1 = C1;

	//Disable low power;
	*MCG_C2 &= ~C2_LP;

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

void mcg_enter_PEE(struct mcg_pll_config *config) {

	//Update the output frequency to the PLL's one
	mcg_out_select_frequency(mcg_pll_frequency);

	//Cache C1;
	uint8_t C1 = *MCG_C1;

	//Clear CLKS bits; Will stay like this;
	C1 &= ~C1_CLKS;

	//Clear IREFS;
	C1 &= ~C1_IREFS;

	//Select the PLL;
	*MCG_C6 |= C6_PLLS;

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

	//Update the output frequency to the OSC's one
	mcg_out_select_frequency(mcg_osc_frequency);

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

	//Update the output frequency to the IRC one
	mcg_out_select_frequency(mcg_irc_frequency);

	//Cache C1;
	uint8_t C1 = *MCG_C1;

	//Clear CLKS bits;
	C1 &= ~C1_CLKS;

	//Set internal clocking;
	C1 |= C1_CLKS_INT;

	//Set IREFS;
	C1 |= C1_IREFS;

	//Select the FLL;
	*MCG_C6 &= ~C6_PLLS;

	//Write C1;
	*MCG_C1 = C1;

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

	//Update the output frequency to the OSC's one
	mcg_out_select_frequency(mcg_osc_frequency);

	//Cache C1;
	uint8_t C1 = *MCG_C1;

	//Clear CLKS bits;
	C1 &= ~C1_CLKS;

	//Set external clocking;
	C1 |= C1_CLKS_INT;

	//Clear IREFS;
	C1 &= ~C1_IREFS;

	//Write C1;
	*MCG_C1 = C1;

	//Enable low power;
	*MCG_C2 |= C2_LP;

}

