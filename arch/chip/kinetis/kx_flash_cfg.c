//
// Created by root on 10/18/18.
//

#include <stdint.h>

//--------------------------------------------------- Make parameters --------------------------------------------------

/*
 * kx.mk must provide, in addition to channel parameters :
 * 	- REFERENCE_SYMBOL : name of the channel;
 * 	- INT_CHANNEL : interrupt channel;
 * 	- REG : start of the channel's registers area;
 */

//If one of the macro was not provided :
#if !defined(BACKDOOR_CMP_KEY) || !defined(F_OPT) || !defined(F_SEC)

//Log
#error "Error, at least one macro argument hasn't been provided. Check the makefile;"

//Define macros. Allows debugging on IDE environment;
#define BACKDOOR_CMP_KEY 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
#define F_OPT 0xF9
#define F_SEC 0xDE

#endif


//--------------------------------------------- Flash configuration struct ---------------------------------------------

//Not used in code, only as schematics;
struct kx_flashconf {
	
	//The backdoor comparison key;
	uint8_t backdoor_conparison_key[8];
	
	//Flash protection bytes;
	uint8_t FPROT3;
	uint8_t FPROT2;
	uint8_t FPROT1;
	uint8_t FPROT0;
	
	
	//Flash security byte;
	uint8_t FSEC;
	
	//Flash option byte;
	uint8_t FOPT;
	
	//FLexNV device : EEPROM protection byte;
	uint8_t FEPROT;
	
	//FLexNV device: data flash protection byte;
	uint8_t FDPROT;
	
	
};


//--------------------------------------------- Flash configuration array ---------------------------------------------

__attribute__ ((section(".flashconfig"), used)) const uint8_t flash_cfg[16] = {
	
	//The Backdoor comparison key;
	BACKDOOR_CMP_KEY,
	
	//Protection bytes;
	0xFF, 0xFF, 0xFF, 0xFF,
	
	//Security byte;
	F_SEC,
	
	//Security byte;
	F_OPT,
	
	//FlexNV EEPROM Protection byte;
	0xFF,
	
	//FlexNV Flash Protection byte;
	0xFF,
	
};
