//
// Created by root on 10/7/18.
//


void __proc_init(void) {
	
	/*
	 * First, disable the watchdog;
	 */
	
	//Disable the watchdog;
	k64_wdog_disable();
	
	//Enable gating for all ports;
	sim_enable_PORTA_clock_gating();
	sim_enable_PORTB_clock_gating();
	sim_enable_PORTC_clock_gating();
	sim_enable_PORTD_clock_gating();
	sim_enable_PORTE_clock_gating();
	
	startup_initialise_globals();
	
	__kernel_init();
	
}