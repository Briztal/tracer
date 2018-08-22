//
// Created by root on 8/22/18.
//

#ifndef TRACER_K64_SIM_H
#define TRACER_K64_SIM_H

#include <stdint.h>

//Enable The voltage regulator;
void sim_enable_usb_voltage_regulator();

//TODO USB VOLTAGE STATUS IN LOW POWER MODES, AFTER PMGR

//Different clock sources for the low power timers;
enum sim_lptmr_clk_src {

	//System Oscillator, 32kHz
	LPTMR_SRC_SYSTEM_OSC,

	//RTC, 32.768kHz
	LPTMR_SRC_RTC,

	//LPO, 1kHz;
	LPTMR_SRC_LPO,

};

//Select the Low Power Timer clock source;
void sim_select_lptmr_clock_source(enum sim_lptmr_clk_src src);


//Get the amount of system RAM available on the device;
uint32_t sim_get_system_ram_amount();


#endif //TRACER_K64_SIM_H
