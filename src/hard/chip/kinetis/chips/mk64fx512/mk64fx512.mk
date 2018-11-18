
#

#-------------------------------------------------- Components config --------------------------------------------------

#The k64fx512 comprises 256 interrupts
NVIC_NB_EXCEPTIONS = 256

#Set the memory map script search dir;
LDSCRIPT_MMAP_DIR := src/hard/chip/kinetis/chips/mk64fx512/


#---------------------------------------------------- Proc make unit ---------------------------------------------------

include src/hard/proc/cortex_m4f/cortex_m4f.mk


#----------------------------------------------------- Abreviations ----------------------------------------------------

#Cache the kinetis peripheral dir;
K_DIR := src/hard/chip/kinetis/periph

#Cache the current dir;
KC_DIR := src/hard/chip/kinetis/chips/mk64fx512

#Use this to compile with the kinetis periph folder included;
MKCC = $(KCC) -I$(K_DIR)


#---------------------------------------------------- Configuration ----------------------------------------------------

#Include the configuration makefile;
include $(KC_DIR)/mk64fx512_config.mk


#------------------------------------------------------ Chip Code ------------------------------------------------------

#Startup code for mk64fx512
mk64fx512:

#Compile the init source;
	@$(MKCC) -o $(KRNL_D)/mk64fx512.o -c $(KC_DIR)/mk64fx512.c

#Init function must stop watchdog at init. Compile it;
	@$(MKCC) -o $(KRNL_D)/kx_wdog.o -c $(K_DIR)/kx_wdog.c

KRNL_RULES += mk64fx512 flash_cfg

#----------------------------------------------------- Flash config ----------------------------------------------------

fcfg_flags := -DBACKDOOR_CMP_KEY=$(FLASH_CFG_BACKDOOR_COMPARISON_KEY)
fcfg_flags += -DF_OPT=$(FLASH_CFG_FLASH_OPTIONS)
fcfg_flags += -DF_SEC=$(FLASH_CFG_FLASH_SECURITY)

flash_cfg:
	@$(MKCC) -o $(KRNL_D)/kx_flash_cfg.o -c $(K_DIR)/kx_flash_cfg.c $(fcfg_flags)


#------------------------------------------------------ MCG module -----------------------------------------------------

osc_args := -DOSC0_CONN_ID=$(OSC0_CONNECTION_ID)
osc_args += -DOSC0_FREQUENCY=$(OSC0_FREQUENCY)
osc_args += -DSTOP_MODE_EN=$(OSC0_STOP_MODE_ENABLE)
osc_args += -DLOC0_RESET=$(OSC0_LOC_RESET)
osc_args += -DLOW_POWER=$(OSC0_CRYSTAL_LOW_POWER)

ifneq ($(OSC0_CONNECTION_ID), 0)
osc_args += -DCAPACITORS=$(OSC0_CAPACITORS)
endif

pll_args := -DSTOP_MODE_EN=$(PLL_STOP_MODE_ENABLE)
pll_args += -DLOL0_INT=$(PLL_LOL_INT)
pll_args += -DLOL0_RESET=$(PLL_LOL_RESET)

clock:

	@$(MKCC) -o $(MODS_D)/kx_irc.o -c $(K_DIR)/kx_clock/kx_irc.c
	@$(MKCC) -o $(MODS_D)/kx_osc.o -c $(K_DIR)/kx_clock/kx_osc.c $(osc_args)
	@$(MKCC) -o $(MODS_D)/kx_fll.o -c $(K_DIR)/kx_clock/kx_fll.c
	@$(MKCC) -o $(MODS_D)/kx_pll.o -c $(K_DIR)/kx_clock/kx_pll.c $(pll_args)
	@$(MKCC) -o $(MODS_D)/kx_mcg.o -c $(K_DIR)/kx_clock/kx_mcg.c $(pll_args)
	@$(MKCC) -o $(MODS_D)/kc_clock.o -c $(K_DIR)/kx_clock/kx_clock.c $(pll_args)

clock := kx_irc.o kx_osc.o kx_fll.o kx_pll.o kx_mcg.o kc_clock.o

MODS_RULES += clock

#--------------------------------------------------- PORT GPIO module --------------------------------------------------

#The port source file takes four arguments, addresses and spacings of port and gpio register areas;
port_args := -DPORT_REG=0x40049000 -DPORT_SPACING=0x1000 -DGPIO_REG=0x400FF000 -DGPIO_SPACING=0x40

port:

#The port object is compiled providing the chip directory, so that appropriate pins get generated;
	@$(MKCC) -o $(MODS_D)/kx_port.o -c $(K_DIR)/kx_port.c $(port_args) -I$(KC_DIR)

port := kx_port.o

MODS_RULES += port

#----------------------------------------------------- PITs module -----------------------------------------------------

#The PIT manager takes two arguments, the address of the MCR register, and the number of channels;
pit_args := -DMODULE_NAME=pit -DNB_CHANNELS=4 -DMCR_REG=0x40037000

#Each PIT channel takes three arguments, its index, the interrupt channel, and its register area address;
pitx_args = $(pit_args) -DCHANNEL_ID=$(1) -DCHANNEL_NAME=$(2) -DINT_CHANNEL=$(3) -DREG=$(4)

#Generate args sequences for each channel;
pit0_args := $(call pitx_args,0,pit_0,48,0x40037100)
pit1_args := $(call pitx_args,1,pit_1,49,0x40037110)
pit2_args := $(call pitx_args,2,pit_2,50,0x40037120)
pit3_args := $(call pitx_args,3,pit_3,51,0x40037130)

#This rule will build the pit archive;
pit:

#Compile all 4 channels with their respective args;
	@$(MKCC) -o $(MODS_D)/pit0.o -c $(K_DIR)/kx_pit_n.c $(pit0_args)
	@$(MKCC) -o $(MODS_D)/pit1.o -c $(K_DIR)/kx_pit_n.c $(pit1_args)
	@$(MKCC) -o $(MODS_D)/pit2.o -c $(K_DIR)/kx_pit_n.c $(pit2_args)
	@$(MKCC) -o $(MODS_D)/pit3.o -c $(K_DIR)/kx_pit_n.c $(pit3_args)

#Compile the pit manager; It takes two arguments, the mcr address and the number of channels;
	@$(MKCC) -o $(MODS_D)/pit.o -c $(K_DIR)/kx_pit.c $(pit_args)

pit := pit.o pit0.o pit1.o pit2.o pit3.o

MODS_RULES += pit