//
// Created by root on 8/22/18.
//

#ifndef TRACER_K64_SIM_H
#define TRACER_K64_SIM_H

#include <stdint.h>

/**
 * The System Interaction Module is always located at the same address;
 */

//----------------- SOPT1 - RW : System Options Register 1 -----------------

#define SIM_SOPT1        ((volatile uint32_t *)0x40047000)
#define SIM_SOPT1CFG    ((volatile uint32_t *)0x40047004)


//Enable The voltage regulator;
//TODO static inline void sim_enable_usb_voltage_regulator();

//TODO USB VOLTAGE REGULATOR STANDBY

//TODO LPTMR CLOCK SOURCE

//TODO RAM SIZE


//----------------- SOPT2 - RW : System Options Register 2 -----------------

#define SIM_SOPT2        ((volatile uint32_t *)0x40048004)

//TODO SDHC CLOCK

//TODO IEE1588 TIMESTAMP CLOCK SOURCE

//RMII CLOCK SOURCE

//TODO USB CLOCK SOURCE

//TODO PLL FLL CLOCK SELECT

//TODO DEBUG TRACE CLOCK SELECT

//TODO PTD7 PAD DRIVE STRENGTH

//TODO FLEXBUS SECURITY

//TODO CLOKOUT CLOCK SELECT

//TODO RTC CLOKC OUT SELECT


//----------------- SOPT3 - RW : System Options Register 3 -----------------

#define SIM_SOPT3        ((volatile uint32_t *)0x40048008)

//TODO LEGACY

//----------------- SOPT4 - RW : System Options Register 4 -----------------

#define SIM_SOPT4        ((volatile uint32_t *)0x4004800C)

//TODO FLEXTIMER SETTINGS


//----------------- SOPT5 - RW : System Options Register 5 -----------------

#define SIM_SOPT5        ((volatile uint32_t *)0x40048010)

//TODO UART 0/1 RX/TX DATA SOURCE


//----------------- SOPT6 - RW : System Options Register 6 -----------------

#define SIM_SOPT6        ((volatile uint32_t *)0x40048014)


//----------------- SOPT7 - RW : System Options Register 7 -----------------

#define SIM_SOPT7        ((volatile uint32_t *)0x40048018)

//TODO ADC 0/1 SETTINGS


//----------------- SDID - RO : System Device Identification Register -----------------

#define SIM_SDID        ((volatile uint32_t *)0x40048024)

//TODO FAMILY + SERED + REV  + DIEIE + PINCOUNT



//---------------------------------------------------- Clock Gating ----------------------------------------------------

/*
 * Clock gating enable / disable functions are all the same; In order to avoid redundancy,
 * 	a macro will be used to enable and disable gating for all peripherals;
 *
 * 	It used macros defined in each section;
 */

#define SIM_DEFINE_GATING(reg_id, peripheral, bit) \
/**
 * Enable and disable the peripheral clock gating using macros;
 */\
static inline void sim_enable_##peripheral##_clock_gating() {\
    *SIM_SCGC##reg_id |= ((uint32_t ) (1 << (bit)));\
}\
\
static inline void sim_disable_##peripheral##_clock_gating() {\
    *SIM_SCGC##reg_id &= ~((uint32_t ) (1 << (bit)));\
}\



//----------------- SCGC1 - RW : System Clock Gating Control Register 1 -----------------

#define SIM_SCGC1        ((volatile uint32_t *)0x40048028)

SIM_DEFINE_GATING(1, I2C2, 6);

SIM_DEFINE_GATING(1, UART4, 10);

SIM_DEFINE_GATING(1, UART5, 11);


//----------------- SCGC2 - RW : System Clock Gating Control Register 2 -----------------

#define SIM_SCGC2        ((volatile uint32_t *)0x4004802C)

SIM_DEFINE_GATING(2, ENET, 0);

SIM_DEFINE_GATING(2, DAC0_aips1, 12);

SIM_DEFINE_GATING(2, DAC1, 13);

//----------------- SCGC3 - RW : System Clock Gating Control Register 3 -----------------

#define SIM_SCGC3        ((volatile uint32_t *)0x40048030)

SIM_DEFINE_GATING(3, RNGA_aips1, 0);

SIM_DEFINE_GATING(3, SPI2, 12);

SIM_DEFINE_GATING(3, SDHC, 17);

SIM_DEFINE_GATING(3, FTM2_aips1, 24);

SIM_DEFINE_GATING(3, FTM3, 25);

SIM_DEFINE_GATING(3, ADC1, 27);


//----------------- SCGC4 - RW : System Clock Gating Control Register 4 -----------------

#define SIM_SCGC4        ((volatile uint32_t *)0x40048034)

SIM_DEFINE_GATING(4, EWM, 1);

SIM_DEFINE_GATING(4, CMT, 2);

SIM_DEFINE_GATING(4, I2C0, 6);

SIM_DEFINE_GATING(4, I2C1, 7);

SIM_DEFINE_GATING(4, UART0, 10);

SIM_DEFINE_GATING(4, UART1, 11);

SIM_DEFINE_GATING(4, UART2, 12);

SIM_DEFINE_GATING(4, UART3, 13);

SIM_DEFINE_GATING(4, USBOTG, 18);

SIM_DEFINE_GATING(4, CMP, 19);

SIM_DEFINE_GATING(4, VREF, 20);


//----------------- SCGC5 - RW : System Clock Gating Control Register 5 -----------------

#define SIM_SCGC5        ((volatile uint32_t *)0x40048038)

SIM_DEFINE_GATING(5, LPTMR, 0);

SIM_DEFINE_GATING(5, PORTA, 9)

SIM_DEFINE_GATING(5, PORTB, 10)

SIM_DEFINE_GATING(5, PORTC, 11)

SIM_DEFINE_GATING(5, PORTD, 12)

SIM_DEFINE_GATING(5, PORTE, 13)


//----------------- SCGC6 - RW : System Clock Gating Control Register 6 -----------------

#define SIM_SCGC6        ((volatile uint32_t *)0x4004803C)

SIM_DEFINE_GATING(6, FTF, 0);

SIM_DEFINE_GATING(6, DMAMUX, 1);

SIM_DEFINE_GATING(6, FLEXCAN0, 4);

SIM_DEFINE_GATING(6, RNGA_aips0, 9);

SIM_DEFINE_GATING(6, SPI0, 12);

SIM_DEFINE_GATING(6, SPI1, 13);

SIM_DEFINE_GATING(6, I2S, 12);

SIM_DEFINE_GATING(6, CRC, 18);

SIM_DEFINE_GATING(6, USBDCD, 21);

SIM_DEFINE_GATING(6, PDB, 22);

SIM_DEFINE_GATING(6, PIT, 23);

SIM_DEFINE_GATING(6, FTM0, 24);

SIM_DEFINE_GATING(6, FTM1, 25);

SIM_DEFINE_GATING(6, FTM2_aips0, 26);

SIM_DEFINE_GATING(6, ADC0, 27);

SIM_DEFINE_GATING(6, RTC, 29);

SIM_DEFINE_GATING(6, DAC0_aips0, 31);



//----------------- SCGC7 - RW : System Clock Gating Control Register 7 -----------------

#define SIM_SCGC7        ((volatile uint32_t *)0x40048040)

SIM_DEFINE_GATING(7, FEXBUS, 0);

SIM_DEFINE_GATING(7, DMA, 1);

SIM_DEFINE_GATING(7, MPU, 2);


#undef SIM_DEFINE_GATING


//----------------- CLKDIV1 - RW : System Clock Divider Register 1 -----------------

#define SIM_CLKDIV1        ((volatile uint32_t *)0x40048044)


/**
 * Clock dividers : a number between 1 and 16 must be provided. If not, call will be ignored;
 * @param clock_divider : the required clock divider;
 */

//All functions are the same, at name except for divider offset. A macro will take care of this;
#define SIM_DEFINE_CLOCK_SETTER(name, offset)\
static inline void sim_set_##name##_clock_divider(uint8_t clock_divider) {\
	/*Decrement the clock divider;*/\
    (clock_divider)--;\
    /*If the clock divider is not between 0 and 15, stop here;*/\
    if ((clock_divider) & 0xF0) {\
		\
        return;\
	}\
	uint32_t var = *SIM_CLKDIV1;\
    var &= ~(((uint32_t) 0x0F) << (offset));\
    var |= ~(((uint32_t) (clock_divider)) << (offset));\
    *SIM_CLKDIV1 = var;\
}

//The core clock part starts at bit 28:
SIM_DEFINE_CLOCK_SETTER(core, 28);

//The bus clock part starts at bit 24:
SIM_DEFINE_CLOCK_SETTER(bus, 24);

//The flexbus clock part starts at bit 20;
SIM_DEFINE_CLOCK_SETTER(flexbus, 20);

//The flash clock part starts at bit 16;
SIM_DEFINE_CLOCK_SETTER(flash, 26);



//----------------- CLKDIV2 - RW : System Clock Divider Register 2 -----------------

#define SIM_CLKDIV2        ((volatile uint32_t *)0x40048048)

//TODO USB CLOCK DIVIDER DIVISOR

//TODO USB CLOCK DIVIDER FRACTION



//----------------- FCFG1 - RO : Flash Configuration Register 1 -----------------

#define SIM_FCFG1        ((volatile uint32_t *)0x4004804C)

//TODO GET FLEXNVM SIZE

//TODO GET FLASH SIZE;

//TODO GET EEPROM SIZE;

//TODO DISABLE FLAH


//----------------- FCFG2 - RO : Flash Configuration Register 2 -----------------

#define SIM_FCFG2        ((volatile uint32_t *)0x40048050)




//----------------- UID - RO : Unique Identification Register -----------------

#define SIM_UIDH        ((volatile uint32_t *)0x40048054)

#define SIM_UIDMH        ((volatile uint32_t *)0x40048058)

#define SIM_UIDML        ((volatile uint32_t *)0x4004805C)

#define SIM_UIDL        ((volatile uint32_t *)0x40048060)


//TODO GET UID








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
