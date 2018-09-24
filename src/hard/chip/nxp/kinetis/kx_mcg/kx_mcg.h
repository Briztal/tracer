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


#ifndef TRACER_KX_MGC_H
#define TRACER_KX_MGC_H


#include <stdbool.h>
#include <stdint.h>


#define U32_DIST(a, b) (((a) < (b)) ? ((b) - (a)) : ((a) - (b)))


#define MCG_C1          ((volatile uint8_t *)0x40064000)
#define C1_CLKS         ((uint8_t)0xC0)

#define C1_CLKS_INT      ((uint8_t) (1<<6))
#define C1_CLKS_EXT        ((uint8_t) (1<<7))

#define C1_FRDIV         ((uint8_t) ((1<<3)|(1<<4)|(1<<5)))
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


//The Internal Reference Clock provided two clocks, the fast IRC at 4MHz and the slow IRC at 32kHz
#define IRC_FAST_FREQ ((uint32_t) 4000000)
#define IRC_SLOW_FREQ ((uint32_t) 32000)

//The array containing different IRC frequencies;
extern const uint32_t irc_frequencies[9];

//Configure the IRC;
void kx_irc_configure(uint8_t channel_id);


/*
 * -------------------------------------------------------- OSC --------------------------------------------------------
 */

//Available OSC frequencies;
extern const uint32_t osc_frequencies[2];


//Configure the oscillator to use a particular channel;
void kx_osc_configure(uint8_t channel);

//Stop the oscillator module;
void kx_osc_stop();

//Clear the osc loss of lock status;TODO
void mcg_clear_osc_loss_of_lock_flag();


/*
 * -------------------------------------------------------- PLL --------------------------------------------------------
 */

struct kx_pll_config {
	
	//The external reference divide factor, between 1 and 24;
	uint8_t external_divide_factor;
	
	//The PLL output multiply factor, between 24 and 55;
	uint8_t output_multiplication_factor;
	
};


//Configure the PLL; OSC Must have been configured before.
void kx_pll_configure(struct kx_pll_config config);

//Stop the PLL;
void kx_pll_stop();

//Wait till PLL lock is acquired;
void kx_pll_acquire_lock();

//Clear the pll loss of lock status;
void kx_clear_pll_loss_of_lock_flag();

//Wait till pll acquires the PLLS clock;
void kx_pll_acquire_plls();


/*
 * -------------------------------------------------------- FLL --------------------------------------------------------
 */


struct kx_fll_config {
	
	//Set this flag if the OSC external ref must be selected as input;
	bool external_ref;
	
	//If the external ref is selected, contains the divider index;
	uint8_t eref_divider_id;
	
	//Set this flag to mention that the input signal has a frequency of 32768 Hertz;
	//FLL Factor will be adjusted to provide a standard output frequency; See mcg_fll_frequency_range;
	bool exact_32768_ref;
	
	//The desired output frequency range;
	uint8_t f_range;
	
	
};

//TODO DOC;
void kx_fll_configure(struct kx_fll_config config);


//Wait till FLL acquires the PLLS clock;
void kx_fll_acquire_plls();

void kx_fll_set_internal_clocking();

void kx_fll_set_external_clocking();


//TODO FLL STOP;
//TODO FLL STOP;
//TODO FLL STOP;
//TODO FLL STOP;
//TODO FLL STOP;
//TODO FLL STOP;
//TODO FLL STOP;
//TODO FLL STOP;
//TODO FLL STOP;
//TODO FLL STOP;
//TODO FLL STOP;


//---------------------------------------------------- Configuration ---------------------------------------------------

/*
 * Different states of the MCG;
 */

enum mcg_mode {
	
	/*
	 * Level 0 modes : they can be entered directly from FBI;
	 */
	
	//FLL Bypassed Internal, safe mode. Configuration can be executed when mcg is in this mode;
		FBI = 0,
	
	//FLL Engaged Internal,
		FEI = 1,
	
	//Bypassed Low Power Internal,
		BLPI = 2,
	
	//FLL Bypassed External,
		FBE = 3,
	
	
	/*
	 * Level 1 modes : a transition to FBE is required;
	 */
	
	//FLL Engaged External,
		FEE = 4,
	
	//Bypassed Low Power External,
		BLPE = 5,
	
	//PLL Bypassed External,
		PBE = 6,
	
	
	/*
	 * Level 2 modes : another transition to PBE is required
	 */
	
	
	//PLL Engaged External,
		PEE = 7,
	
};


struct mcg_config {
	
	//The operation mode;
	enum mcg_mode mode;
	
	//The resulting mcg frequency;
	uint32_t frequency;
	
	/*
	 * OSC;
	 */
	
	//OSC enabled ?
	bool osc_enabled;
	
	//OSC channel;
	uint8_t osc_channel;
	
	
	/*
	 * IRC
	 */
	
	//Is the IRC used ?
	bool irc_used;
	
	//The IRC channel;
	uint8_t irc_channel;
	
	
	/*
	 * FLL
	 */
	
	//If the FLL used ?
	bool fll_used;
	
	struct kx_fll_config fll_configuration;
	
	
	/*
	 * PLL
	 */
	
	//If the PLL used ?
	bool pll_used;
	
	struct kx_pll_config pll_configuration;
	
};


//------------------------------------------------ Configuration finding -----------------------------------------------

//Evaluate all configurations, and update the mcg config, if a better config that the current is found;
bool kx_irc_find_configuration(uint32_t target_frequency, struct mcg_config *config);

//Evaluate all configurations, and update the mcg config, if a better config that the current is found;
bool kx_osc_find_configuration(uint32_t target_frequency, struct mcg_config *config);

//Evaluate all configurations, and update the mcg config, if a better config that the current is found;
bool kx_fll_find_configuration(uint32_t target_frequency, struct mcg_config *config);


//------------------------------------------------ Frequency tuning -----------------------------------------------

void mcg_autotune(uint32_t frequency_target);


#endif //TRACER_KX_MGC_H
