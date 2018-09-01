//
// Created by root on 8/23/18.
//

#ifndef TRACER_KX_MGC_H
#define TRACER_KX_MGC_H


#include <stdbool.h>
#include <stdint.h>

//TODO EXTERNAL CLOCKS (PLLOUT FLLOUT EREFOUT)
//TODO EXTERNAL CLOCKS (PLLOUT FLLOUT EREFOUT)
//TODO EXTERNAL CLOCKS (PLLOUT FLLOUT EREFOUT)
//TODO EXTERNAL CLOCKS (PLLOUT FLLOUT EREFOUT)
//TODO EXTERNAL CLOCKS (PLLOUT FLLOUT EREFOUT)
//TODO EXTERNAL CLOCKS (PLLOUT FLLOUT EREFOUT)
//TODO EXTERNAL CLOCKS (PLLOUT FLLOUT EREFOUT)
//TODO EXTERNAL CLOCKS (PLLOUT FLLOUT EREFOUT)
//TODO EXTERNAL CLOCKS (PLLOUT FLLOUT EREFOUT)
//TODO EXTERNAL CLOCKS (PLLOUT FLLOUT EREFOUT)
//TODO EXTERNAL CLOCKS (PLLOUT FLLOUT EREFOUT)
//TODO EXTERNAL CLOCKS (PLLOUT FLLOUT EREFOUT)
//TODO EXTERNAL CLOCKS (PLLOUT FLLOUT EREFOUT)
//TODO EXTERNAL CLOCKS (PLLOUT FLLOUT EREFOUT)
//TODO EXTERNAL CLOCKS (PLLOUT FLLOUT EREFOUT)

/*
 * -------------------------------------------------------- IRC --------------------------------------------------------
 */


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


/*
 * The OSC module must be initialised. This operation should be executed at startup before any config is done;
 * 	Parameters provided in this struct will not be changed by config;
 */

struct mcg_osc0_init {

	//The frequency of the oscillator 0;
	uint32_t osc0_frequency;

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

//Initialise the OSC module; Must be done before any config attempt;
void mcg_initialise_osc0(const struct mcg_osc0_init *init);


/*
 * The OSC module must be configured, to determine the status, and which clock is selected;
 */

struct mcg_osc_config {

	//Set to enable the osc output; can be oscillator or external reference clock;
	bool output_enabled;

	//Set to maintain enabled in stop mode;
	bool stay_enabled_in_stop_mode;

	//Should a loss of clock generate a reset ?
	bool loss_of_clock_generates_reset;

	//Set if the external clock; is selected for output. Clear if the internal oscillator is selected for output;
	bool external_clock_selected;

	//Used if the external clock is not selected. Set if OSC0 is selected;
	bool osc0_selected;

};


//Configure the oscillator;
void mcg_configure_osc(const struct mcg_osc_config *config);

//Clear the osc loss of lock status;TODO
void mcg_clear_osc_loss_of_lock_flag();


/*
 * ------------------------------------------------------ OSC32K -------------------------------------------------------
 */

#define OSC32K_FREQ 32768

//TODO CONFIGURATION OF LOSS OF CLOCK FOR RTC OSC;


//Clear the RTC OSC loss of lock status;TODO
void mcg_clear_rtcosc_loss_of_lock_flag();


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

//Configure the PLL input. If the extenal ref is selected, the OSC must have been initialised;
//The fll input is actually changed;
//void mcg_configure_fll_input(bool external_ref, uint8_t eref_divider_id);


/**
 * The mcg fll configuration enum allows to select the output frequency range of the FLL;
 *
 * 	The input signal selection is realised by the MCG;
 */

enum mcg_fll_frequency_range {

	//Between 20 and 25 MHz, with 246MHz when the ref clock is selected;
		FLL_RANGE_20_25_ref24_MHz = 0,

	//Between 40 and 50 MHz, with 48MHz when the ref clock is selected;
		FLL_RANGE_40_50_ref48_MHz = 1,

	//Between 60 and 75 MHz, with 72MHz when the ref clock is selected;
		FLL_RANGE_60_75_ref72_MHz = 2,

	//Between 80 and 100 MHz, with 96MHz when the ref clock is selected;
		FLL_RANGE_80_100_ref96_MHz = 3,

};

//Configure the FLL output frequency; OSC must have been configured first;
//void mcg_configure_fll_output(enum mcg_fll_frequency_range f_range, bool exact_32768_ref);

struct fll_input_config {

	//Set this flag if the OSC external ref must be selected as input;
	bool external_ref;

	//If the external ref is selected, contains the divider index;
	uint8_t eref_divider_id;

};

//TODO STATIC IN SOURCE;
//TODO DOC;
void mcg_configure_fll_input(struct fll_input_config config);


struct fll_output_config {

	//The desired output frequency range;
	enum mcg_fll_frequency_range f_range;

	//Set this flag to mention that the input signal has a frequency of 32768 Hertz;
	//FLL Factor will be adjusted to provide a standard output frequency; See mcg_fll_frequency_range;
	bool exact_32768_ref;

};

//TODO STATIC IN SOURCE;
//TODO DOC;
void mcg_configure_fll_output(struct fll_output_config config);


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
	bool loss_of_lock_generates_interrupt;

	//Shall a reset be triggered when a loss of lock is detected ?
	//Used only if loss_of_lock_generates_interrupt is set;
	bool loss_of_lock_generates_reset;

	//Enable during stop mode;
	bool enable_during_stop_mode;
	//The external reference divide factor, between 1 and 24;
	uint8_t external_divide_factor;

	//The PLL output multiply factor, between 24 and 55;
	uint8_t output_multiplication_factor;

};

//Configure the PLL; OSC Must have been configured before.
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

//TODO ENABLE MCG CLOCKS
//TODO GET MCG FREQUENCIES

//----------------------------------------------------------------------------------------------------------------------

/**
 * FLL Engaged Internal (FEI) :
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

void mcg_enter_FEI(enum mcg_fll_frequency_range range);


//----------------------------------------------------------------------------------------------------------------------

/**
 * FLL Engaged External (FEE) :
 *
 * 	This mode can be entered from FBE;
 *
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
 *
 * 	The IRC slow frequency is not at 32786 Hertz, the only field required is the frequency range;
 */

struct FEE_config {

	//The identifier of the external divider;
	uint8_t eref_id;

	//The desired output frequency range;
	enum mcg_fll_frequency_range f_range;

	//Set this flag to mention that the input signal has a frequency of 32768 Hertz;
	//FLL Factor will be adjusted to provide a standard output frequency; See mcg_fll_frequency_range;
	bool exact_32768_ref;

};

//Enter in FEE mode.
void mcg_enter_FEE(struct FEE_config config);


//----------------------------------------------------------------------------------------------------------------------

/**
 * FLL Bypassed Internal (FBI) :
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
void mcg_enter_FBI();


//----------------------------------------------------------------------------------------------------------------------

/**
 * FLL Bypassed External (FBE) :
 *
 * This mode can be entered from FBI, FEE, PBE or BLPE;
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
void mcg_enter_FBE();



//----------------------------------------------------------------------------------------------------------------------

/**
 * PLL Engaged External (PEE) : //TODO INIT PLL AT CALL TIME;
 *
 * 	This mode can be entered from PBE mode;
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

void mcg_enter_PEE(struct mcg_pll_config *config);


//----------------------------------------------------------------------------------------------------------------------

/**
 * PLL Bypassed External (PBE) : //TODO INIT PLL AT CALL TIME;
 *
 * 	This mode can be entered from FBE and BLPE mode;
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

void mcg_enter_PBE();

//----------------------------------------------------------------------------------------------------------------------

/*
 * Bypass Low Power Internal (BLPI) :
 *
 * 	This mode can be entered from FBI;
 *
 *  In BLPI mode, MCGOUTCLK is derived from the internal reference clock. The FLL is disabled and PLL is disabled
 *  even if C5[PLLCLKEN] is set to 1.
 */

void mcg_enter_BLPI();


//----------------------------------------------------------------------------------------------------------------------

/*
 * Bypass Low Power External (BLPE) :
 *
 * 	This mode can be entered from PBE or FBE;
 *
 *  In BLPE mode, MCGOUTCLK is derived from the OSCSEL external reference clock. The FLL is disabled and PLL is
 *  disabled even if the C5[PLLCLKEN] is set to 1.
 */

void mcg_enter_BLPE();



/*
 * ------------------------------------------- MCG Autotune -------------------------------------------
 */

/**
 *
 * Init : in a determined MCG state.
 *
 * TODO PROVIDE INTERRUPT CONFIGURATION AT MCG INIT;
 *
 */
#endif //TRACER_KX_MGC_H
