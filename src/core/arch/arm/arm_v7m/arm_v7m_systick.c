//
// Created by root on 9/1/18.
//

/*
 * All functions from arm_v7m.h are static inline. This file makes proper definitions;
 */

#include <stdint.h>
#include <core/core.h>

#include "arm_v7m.h"

#include "nvic.h"


//Start the core timer;
void core_timer_start() {
	armv7m_systick_enable();
}

//Stop the core timer;
void core_timer_stop() {
	armv7m_systick_disable();
}

//Enable the core interrupt;
void core_timer_int_enable() {
	armv7m_systick_int_enable();
}

//Disable the core interrupt;
void core_timer_int_disable() {
	armv7m_systick_int_disable();
}


//Set the core timer interrupt frequency. The core frequency must be available
void core_timer_int_set_frequency(uint32_t frequency) {

	//TODO. FOR INSTANCE, ONLY THE 10MS IS LOADED;
	//armv7m_systick_set_reload(armv7m_systick_get_10ms_reload());
	armv7m_systick_set_reload(3);



}


//Set the core timer interrupt priority;
extern void core_timer_int_set_priority(uint8_t priority) {

	armv7m_set_systick_priority(priority);

	core_log("prio : ");
	core_log_int(priority);



}


//Set the core timer interrupt handler;
void core_timer_int_set_handler(void (*handler)()) {

	//TODO ERROR or log if null;

	nvic_set_exception_handler(NVIC_SYSTICK, handler);

	core_log("prio : ");
	core_log_int(handler);

}
