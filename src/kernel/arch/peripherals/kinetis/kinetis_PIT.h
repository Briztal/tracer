//
// Created by root on 6/15/18
//

//TODO PJRC TRIBUTE

#ifndef TRACER_KINETIS_PIT_H
#define TRACER_KINETIS_PIT_H

#include <kinetis.h>
#include <kernel/drivers/timer.h>


//----------------------------------------- Structs declarations - definitions -----------------------------------------

/*
 * The kinetis Periodic Interrupt Timer memory struct; Maps the memory structure of a hardware PIT;
 */
typedef struct __attribute__ ((packed)) kinetis_PIT_registers {

	//The Load Value;
	volatile uint32_t LDVAL;

	//Current Value;
	volatile uint32_t CVAL;

	//Timer Control Register
	volatile uint32_t TCTRL;

	//Timer Flag;
	volatile uint32_t TFLG;

};


/*
 * The kinetis Periodic Interrupt Timer data struct; contains a memory zone pointer and a clock frequency;
 */
struct kinetis_PIT_hw {

	//The PIT's memory zone
	kinetis_PIT_memory_t *const memory;

	//The interrupt channel;
	const uint8_t reload_int_channel;

	//The PIT's frequency;
	const uint32_t clock_frequency;

} ;


//------------------------------------------- PITs declarations - definitions ------------------------------------------

//Declare a port (for arch header);
#define KINETIS_PIT_DECLARE(id) extern kinetis_PIT_data_t PIT_##id;

//Define a port (for arch source);
#define KINETIS_PIT_DEFINE(id, memory_start, reload_interrupt_channel, clock_freq)\
     kinetis_PIT_data_t PIT_##id = {\
        .memory = (kinetis_PIT_memory_t *) (memory_start),\
        .reload_int_channel = (const uint8_t) (reload_interrupt_channel),\
        .clock_frequency = (const uint8_t) (clock_freq),\
    };


//---------------------------------------------------- Timer legacy ----------------------------------------------------

//Initialise the PIT as an interrupt timer;
timer_base_t *kinetis_PIT_init_as_timer(kinetis_PIT_data_t *PIT_data, uint32_t base_frequency);


//-------------------------------------------- PITs public inline functions --------------------------------------------

//Enable the timer;
inline void kinetis_PIT_enable_timer(kinetis_PIT_data_t *data) {
	//Set bit 0 of TCTRL;
	data->memory->TCTRL |= 1;
}

//Disable the timer;
inline void kinetis_PIT_disable_timer(kinetis_PIT_data_t *data) {
	//Clear bit 0 of TCTRL;
	data->memory->TCTRL &= ~1;
}

//Is the timer enabled ?
inline bool kinetis_PIT_timer_enabled(kinetis_PIT_data_t *data) {
	//Is bit 0 of TCTRL set ?
	return (bool) (data->memory->TCTRL & ~(uint32_t) 1);
}


//Enable the interrupt;
inline void kinetis_PIT_enable_interrupt(kinetis_PIT_data_t *data) {
	//Set bit 1 of TCTRL;
	data->memory->TCTRL |= 2;
}

//Disable the interrupt;
inline void kinetis_PIT_disable_interrupt(kinetis_PIT_data_t *data) {
	//Clear bit 1 of TCTRL;
	data->memory->TCTRL &= ~2;
}

//Is the interrupt enabled ?
inline bool kinetis_PIT_interrupt_enabled(kinetis_PIT_data_t *data) {
	//Is bit 1 of TCTRL set ?
	return (bool) (data->memory->TCTRL & ~(uint32_t) 2);
}


//Enable the chaining;
inline void kinetis_PIT_enable_chaining(kinetis_PIT_data_t *data) {
	//Set bit 2 of TCTRL;
	data->memory->TCTRL |= 4;
}

//Disable the chaining;
inline void kinetis_PIT_disable_chaining(kinetis_PIT_data_t *data) {
	//Clear bit 2 of TCTRL;
	data->memory->TCTRL &= ~4;
}

//Is the chaining enabled ?
inline bool kinetis_PIT_chaining_enabled(kinetis_PIT_data_t *data) {
	//Is bit 2 of TCTRL set ?
	return (bool) (data->memory->TCTRL & ~(uint32_t) 4);
}


//Clear the interrupt flag;
inline void kinetis_PIT_clear_flag(kinetis_PIT_data_t *data) {
	//Set bit 0 of TFLG;
	data->memory->TFLG = 1;
}


//Is the flag set ?
inline bool kinetis_PIT_flag_set(kinetis_PIT_data_t *data) {
	//Is bit 0 of TFLG set ?
	return (bool) (data->memory->TFLG);
}


//Get the reload value;
inline uint32_t kinetis_PIT_get_LDVAL(kinetis_PIT_data_t *data) {
	return data->memory->LDVAL;
}

//Update the reload value;
inline void kinetis_PIT_set_LDVAL(kinetis_PIT_data_t *data, uint32_t LDVAL) {
	data->memory->LDVAL = LDVAL;
}


//Get the current value;
inline uint32_t kinetis_PIT_get_CVAL(kinetis_PIT_data_t *data) {
	return data->memory->CVAL;
}

//Update the reload value;
inline void kinetis_PIT_set_CVAL(kinetis_PIT_data_t *data, uint32_t CVAL) {
	data->memory->CVAL = CVAL;
}


#endif //TRACER_KINETIS_PIT_H
