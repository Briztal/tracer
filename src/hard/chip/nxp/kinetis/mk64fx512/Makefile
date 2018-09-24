
#Set the memory map script;
LDSCRIPT_MMAP_DIR := src/hard/chip/nxp/kinetis/mk64fx512/



#add both directories to the core include path;
CORE_INC += -Isrc/hard/chip/nxp/kinetis

#define the current dir here;
K_DIR	:=  hard/chip/nxp/kinetis


include src/hard/proc/cortex_m4f/Makefile


#Macros to abbreviate;
SK_DIR	:=  src/hard/chip/nxp/kinetis

#---------------------------------------------- System files and modules ----------------------------------------------

#TODO PRESIQUISITES : OSC GENERAL ARGS, OSC CRYSTAL ARGS

osc_gp_args := -DOSC0_CONN_ID=1 -DOSC0_FREQUENCY=16000000 -DSTOP_MODE_EN=1 -DLOC0_RESET=0

osc_crystal_args := -DLOW_POWER=1 -DCAPACITORS=5

pll_args := -DSTOP_MODE_EN=1 -DLOL0_INT=0 -DLOL0_RESET=0


mk64fx512: cortex_m4f

	@echo "compiling mk64fx512 module\n"

#Compile the init source;
	@$(CC) $(CFLAGS) $(INC) -o build/hard/mk64fx512.o -c $(SK_DIR)/mk64fx512/mk64fx512.c

#Init function must stop watchdog at init. Compile it;
	@$(CC) $(CFLAGS) $(INC) -o build/hard/kx_wdog.o -c $(SK_DIR)/kx_wdog.c

#compile the IRC;
	@$(CC) $(CFLAGS) $(INC) -o build/hard/kx_irc.o -c $(SK_DIR)/kx_mcg/kx_irc.c

#compile the OSC;
	@$(CC) $(CFLAGS) $(INC) $(osc_gp_args) $(osc_crystal_args) -o build/hard/kx_osc.o -c $(SK_DIR)/kx_mcg/kx_osc.c

#compile the FLL;
	@$(CC) $(CFLAGS) $(INC) -o build/hard/kx_fll.o -c $(SK_DIR)/kx_mcg/kx_fll.c

#compile the PLL;
	@$(CC) $(CFLAGS) $(INC) $(pll_args) -o build/hard/kx_pll.o -c $(SK_DIR)/kx_mcg/kx_pll.c

#Compile the MCG;
	@$(CC) $(CFLAGS) $(INC) $(pll_args) -o build/hard/kx_mcg.o -c $(SK_DIR)/kx_mcg/kx_mcg.c



#---------------------------------------------- Peripheral Modules ----------------------------------------------



#------------------ Port + gpio module

#The port source file takes four arguments, addresses and spacings of port and gpio register areas;
port_args := -DPORT_REG=0x40049000 -DPORT_SPACING=0x1000 -DGPIO_REG=0x400FF000 -DGPIO_SPACING=0x40

port:

#Log the compilation of the manager;
	@echo "[CC]\t$(SK_DIR)/kx_port.c"

#Compile the pit manager;
	@$(CC) $(CFLAGS) $(INC) $(port_args) -o build/mod/kx_port.o -c $(SK_DIR)/kx_port.c


#Add the port rule to the module dependencies list;
MODULES += port


#------------------ Periodic Interrupt Timer

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

	 echo $(call pitx_args,0,pit_0,48,0x40037100)

#Log the compilation of the channel;
	@echo "[CC]\t$(SK_DIR)/kx_pit_n.c"

#Compile all 4 channels with their respective args;
	$(CC) $(CFLAGS) $(INC) $(pit0_args) -o build/mod/pit0.o -c $(SK_DIR)/kx_pit_n.c
	@$(CC) $(CFLAGS) $(INC) $(pit1_args) -o build/mod/pit1.o -c $(SK_DIR)/kx_pit_n.c
	@$(CC) $(CFLAGS) $(INC) $(pit2_args) -o build/mod/pit2.o -c $(SK_DIR)/kx_pit_n.c
	@$(CC) $(CFLAGS) $(INC) $(pit3_args) -o build/mod/pit3.o -c $(SK_DIR)/kx_pit_n.c

#Log the compilation of the manager;
	@echo "[CC]\t$(SK_DIR)/kx_pit.c"

#Compile the pit manager; It takes two arguments, the mcr address and the number of channels;
	@$(CC) $(CFLAGS) $(INC) $(pit_args) -o build/mod/pit.o -c $(SK_DIR)/kx_pit.c



#Add the pit rule to the module dependencies list;
MODULES += pit
