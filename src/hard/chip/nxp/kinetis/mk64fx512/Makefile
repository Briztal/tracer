
include src/hard/proc/cortex_m4f/Makefile

#---------------------------------------------- Core files ----------------------------------------------

#add both directories to the core include path;
CORE_INC += -Isrc/hard/chip/nxp/kinetis

#define the current dir here;
K_DIR	:=  hard/chip/nxp/kinetis

#Add the chip source file to the hw sources list;
HARD_SRCS +=  $(K_DIR)/mk64fx512/mk64fx512.c
HARD_SRCS +=  $(K_DIR)/kx_mcg.c
HARD_SRCS +=  $(K_DIR)/kx_wdog.c

#Set the memory map script;
LDSCRIPT_MMAP_DIR := src/hard/chip/nxp/kinetis/mk64fx512/


#---------------------------------------------- Peripheral Modules ----------------------------------------------

#Macros to abbreviate;
SK_DIR	:=  src/hard/chip/nxp/kinetis
BK_DIR	:=  build/hard/chip/nxp/kinetis



#------------------ Port + gpio module

#The port source file takes four arguments, addresses and spacings of port and gpio register areas;
port_args = -DPORT_REG=0x40049000 -DPORT_SPACING=0x1000 -DGPIO_REG=0x400FF000 -DGPIO_SPACING=0x40

port:

#Create a directory for the PORT library;
	@mkdir -p build/mod/PORT

#Log the compilation of the manager;
	@echo "[CC]\t$(SK_DIR)/kx_port.c"

#Compile the pit manager;
	@$(CC) $(CFLAGS) $(INC) $(port_args) -o $(BK_DIR)/port.o -c $(SK_DIR)/kx_port.c


#Add the port rule to the module dependencies list;
MODULES += port

#Add the PORT objects to the modules list;
MODULES_OBJS += $(BK_DIR)/port.o


#------------------ Periodic Interrupt Timer

#The PIT manager takes two arguments, the address of the MCR register, and the number of channels;
pit_args =  -DMODULE_NAME=pit -DNB_CHANNELS=4 -DMCR_REG=0x40037000

#Each PIT channel takes three arguments, its index, the interrupt channel, and its register area address;
pitx_args = $(pit_args) -DCHANNEL_ID=$(1) -DCHANNEL_NAME=$(2) -DINT_CHANNEL=$(3) -DREG=$(4)

#Generate args sequences for each channel;
pit0_args := $(call pitx_args,0,pit_0,48,0x40037100)
pit1_args := $(call pitx_args,1,pit_1,49,0x40037110)
pit2_args := $(call pitx_args,2,pit_2,50,0x40037120)
pit3_args := $(call pitx_args,3,pit_3,51,0x40037120)


#This rule will build the pit archive;
pit:

#Create a directory for the PIT library;
	@mkdir -p build/mod/PIT

#Log the compilation of the channel;
	@echo "[CC]\t$(SK_DIR)/kx_pit_channel.c"

#Compile all 4 channels with their respective args;
	@$(CC) $(CFLAGS) $(INC) $(pit0_args) -o $(BK_DIR)/pit0.o -c $(SK_DIR)/kx_pit_n.c
	@$(CC) $(CFLAGS) $(INC) $(pit1_args) -o $(BK_DIR)/pit1.o -c $(SK_DIR)/kx_pit_n.c
	@$(CC) $(CFLAGS) $(INC) $(pit2_args) -o $(BK_DIR)/pit2.o -c $(SK_DIR)/kx_pit_n.c
	@$(CC) $(CFLAGS) $(INC) $(pit3_args) -o $(BK_DIR)/pit3.o -c $(SK_DIR)/kx_pit_n.c

#Log the compilation of the manager;
	@echo "[CC]\t$(SK_DIR)/kx_pit.c"

#Compile the pit manager; It takes two arguments, the mcr address and the number of channels;
	@$(CC) $(CFLAGS) $(INC) $(pit_args) -o $(BK_DIR)/pit.o -c $(SK_DIR)/kx_pit.c



#Add the pit rule to the module dependencies list;
MODULES += pit

#Add the PIT objects to the modules list;
MODULES_OBJS += $(BK_DIR)/pit.o $(BK_DIR)/pit0.o $(BK_DIR)/pit1.o $(BK_DIR)/pit2.o $(BK_DIR)/pit3.o

