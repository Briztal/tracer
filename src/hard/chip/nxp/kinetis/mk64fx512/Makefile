

#-------------------------------------------------- Components config --------------------------------------------------

#The k64fx512 comprises 256 interrupts
NVIC_NB_EXCEPTIONS = 256

#Set the memory map script search dir;
LDSCRIPT_MMAP_DIR := src/hard/chip/nxp/kinetis/mk64fx512/


#---------------------------------------------------- Proc make unit ---------------------------------------------------

include src/hard/proc/cortex_m4f/Makefile


#----------------------------------------------------- Abreviations ----------------------------------------------------

#define the current dir here;
K_DIR :=  hard/chip/nxp/kinetis

#Macros to abbreviate;
SK_DIR :=  src/hard/chip/nxp/kinetis


#-------------------------------------------------- Compilation Rules --------------------------------------------------

#Startup code for mk64fx512
mk64fx512:

#Compile the init source;
	@$(_CC) -o $(KRNL_D)/mk64fx512.o -c $(SK_DIR)/mk64fx512/mk64fx512.c

#Init function must stop watchdog at init. Compile it;
	@$(_CC) -o $(KRNL_D)/kx_wdog.o -c $(SK_DIR)/kx_wdog.c



#Multipurpose clock generator

osc_gp_args := -DOSC0_CONN_ID=1 -DOSC0_FREQUENCY=16000000 -DSTOP_MODE_EN=1 -DLOC0_RESET=0

osc_crystal_args := -DLOW_POWER=1 -DCAPACITORS=5

pll_args := -DSTOP_MODE_EN=1 -DLOL0_INT=0 -DLOL0_RESET=0

clock:

#compile the IRC;
	@$(_CC) -o $(MODS_D)/kx_irc.o -c $(SK_DIR)/kx_clock/kx_irc.c

#compile the OSC;
	@$(_CC) -o $(MODS_D)/kx_osc.o -c $(SK_DIR)/kx_clock/kx_osc.c $(osc_gp_args) $(osc_crystal_args)

#compile the FLL;
	@$(_CC) -o $(MODS_D)/kx_fll.o -c $(SK_DIR)/kx_clock/kx_fll.c

#compile the PLL;
	@$(_CC) -o $(MODS_D)/kx_pll.o -c $(SK_DIR)/kx_clock/kx_pll.c $(pll_args)

#Compile the MCG;
	@$(_CC) -o $(MODS_D)/kx_mcg.o -c $(SK_DIR)/kx_clock/kx_mcg.c $(pll_args)

#Compile the clock module;
	@$(_CC) -o $(MODS_D)/kc_clock.o -c $(SK_DIR)/kx_clock/kx_clock.c $(pll_args)



#Port + gpio module

#The port source file takes four arguments, addresses and spacings of port and gpio register areas;
port_args := -DPORT_REG=0x40049000 -DPORT_SPACING=0x1000 -DGPIO_REG=0x400FF000 -DGPIO_SPACING=0x40

port:

#Compile the pit manager;
	@$(_CC) -o $(MODS_D)/kx_port.o -c $(SK_DIR)/kx_port.c $(port_args)



#Periodic Interrupt Timers

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
	@$(_CC) -o $(MODS_D)/pit0.o -c $(SK_DIR)/kx_pit_n.c $(pit0_args)
	@$(_CC) -o $(MODS_D)/pit1.o -c $(SK_DIR)/kx_pit_n.c $(pit1_args)
	@$(_CC) -o $(MODS_D)/pit2.o -c $(SK_DIR)/kx_pit_n.c $(pit2_args)
	@$(_CC) -o $(MODS_D)/pit3.o -c $(SK_DIR)/kx_pit_n.c $(pit3_args)

#Compile the pit manager; It takes two arguments, the mcr address and the number of channels;
	@$(_CC) -o $(MODS_D)/pit.o -c $(SK_DIR)/kx_pit.c $(pit_args)



#-------------------------------------------------- Components Config --------------------------------------------------

#Add the port rule to the module dependencies list;
MODS_RULES += mk64fx512

#Add the port rule to the module dependencies list;
MODS_RULES += clock

#Add the port rule to the module dependencies list;
MODS_RULES += port

#Add the pit rule to the module dependencies list;
MODS_RULES += pit
