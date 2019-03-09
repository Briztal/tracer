#
# This file contains all configurable parameters when building a kinetis k;
#
#	All makefiles that build chip of this kind
#
# TODO kx_dft.mk
# TODO kx_dft.mk
# TODO kx_dft.mk
# TODO kx_dft.mk
# TODO kx_dft.mk
#
# TODO DEFAULT VALUES
# TODO DEFAULT VALUES
# TODO DEFAULT VALUES
# TODO DEFAULT VALUES



#The mk64fx512 comprises 256 interrupts
NVIC_NB_EXCEPTIONS ?= 256


#------------------------------------------------- Flash configuration -------------------------------------------------

#8 bytes of the backdoor comparisond key, most significant on the right, comma separated with no comma at the end;
FLASH_CFG_BACKDOOR_COMPARISON_KEY ?= 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF

#The boot options flash byte. Formatted like below :
#	NMI (bit 2) | EZPort (bit 1) | POWER (bit 0)
#
#	NMI 	: 1 for enabled  0 for disabled;
#	EZPort	: 1 for enabled  0 for disabled;
#	POWER 	: 1 for high power, 0 for low power;

FLASH_CFG_FLASH_OPTIONS ?= 0xF9


#The flash security byte; Formatted like below :
#	KEY_EN (bits 6 7) | MEEN (bits 4 5) | FSLACC (bits 2 3) | SEC (bits 0 1)
#
#	SEC 	: 10 for unsecure, other for secure;
#	FLSACC 	: 00 or 11 for factory access granted, other for access denied;
#	MEEN 	: 10 for mass erase disabled, other for enabled;
#	KEY_EN 	: 10 for backdoor key access enabled to flash controller, other for disabled;

FLASH_CFG_FLASH_SECURITY ?= 0xDE


#------------------------------------------ Multipurpose Clock Generator OSC0 ------------------------------------------

#The idendifier of the connection for the OSC0. 0 for extenal clocking, and 1, 2 or 3 for different internal modes;
OSC0_CONNECTION_ID ?= 1

#The frequency of the OSC0 signal, crystal, or external reference;
OSC0_FREQUENCY ?= 16000000

#Should the OSC0 stay enabled in stop mode ?
OSC0_STOP_MODE_ENABLE ?= 1

#Should a loss of clock in OSC0 generate a reset ?
OSC0_LOC_RESET ?= 1

#If the OSC0 singal comes from a crystal (connection id not 0), two supplementary parameters must be supplied;

#Is the crystal in low power mode ? Connection should be set appropriately;
OSC0_CRYSTAL_LOW_POWER ?= 1

#What is the capacitors condiguration ? Provide an integer on 4 bits formatted like :
#	CP16 (bit 3) | CP8 (bit 2) | CP4 (bit 1) | CP2 (bit 0)
OSC0_CAPACITORS ?= 5


#------------------------------------------ Multipurpose Clock Generator PLL -------------------------------------------

#PLL enabled by default in stop mode ?
PLL_STOP_MODE_ENABLE ?= 1

#Should a loss of lock in the PLL generate an interrupt ?
PLL_LOL_INT ?= 0

#Should a loss of lock in the PLL generate a reset ?
PLL_LOL_RESET ?= 0
