//
// Created by root on 8/22/18.
//

#include "kx_sim.h"

#include <stdint.h>

/**
 * The System Interaction Module is always located at the same address;
 */

#define SIM_SOPT1 		((uint32_t *)0x40047000)
#define SIM_SOPT1CFG 	((uint32_t *)0x40047004)
#define SIM_SOPT2 		((uint32_t *)0x40047000)
#define SIM_SOPT4 		((uint32_t *)0x40047000)


struct __attribute__((packed)) k64_sim_registers_l {

	//System Option Register 1;
	volatile uint32_t SOPT1;

	//SOPT1 Configuration Register
	volatile uint32_t SOPT1CFG;


};

struct __attribute__((packed)) k64_sim_registers_h {
	
	//System Option Register 2;
	volatile uint32_t SOPT2;

	//Padding;
	volatile uint32_t padding0;

	//System Option Register 4;
	volatile uint32_t SOPT4;

	//System Option Register 5;
	volatile uint32_t SOPT5;

	//Padding;
	volatile uint32_t padding1;
	
	//System Option Register 7;
	volatile uint32_t SOPT7;

	//Padding;
	volatile uint32_t padding2[2];

	//System Device Identification Register;
	volatile uint32_t SDID;

	//System Clock Gating Control Register1;
	volatile uint32_t SCGCR1;

	//System Clock Gating Control Register2;
	volatile uint32_t SCGCR2;

	//System Clock Gating Control Register3;
	volatile uint32_t SCGCR3;

	//System Clock Gating Control Register4;
	volatile uint32_t SCGCR4;

	//System Clock Gating Control Register5;
	volatile uint32_t SCGCR5;

	//System Clock Gating Control Register6;
	volatile uint32_t SCGCR6;

	//System Clock Gating Control Register7;
	volatile uint32_t SCGCR7;

	//System Clock Divider Register 1;
	volatile uint32_t CLKDIV1;

	//System Clock Divider Register 2;
	volatile uint32_t CLKDIV2;

	//Flash Configuration Register 1;
	volatile uint32_t FCFG1;

	//Flash Configuration Register 2;
	volatile uint32_t FCFG2;

	//Unique Identification Register High
	volatile uint32_t UIDH;

	//Unique Identification Register Mid-High
	volatile uint32_t UIDMH;

	//Unique Identification Register Mid-Low
	volatile uint32_t UIDML;

	//Unique Identification Register Low
	volatile uint32_t UIDL;

};


static struct k64_sim_registers_l *sim_l = ;//TODO;

static struct k64_sim_registers_h *sim_h =; //TODO;


