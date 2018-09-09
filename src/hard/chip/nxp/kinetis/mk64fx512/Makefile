
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


#------------------ Periodic Interrupt Timer ------------------


SK_DIR	:=  src/hard/chip/nxp/kinetis
BK_DIR	:=  build/hard/chip/nxp/kinetis

#Add the pit module to the kernel dependencies
KERNEL_DEP += pit

#This rule will build the pit archive;
pit:

	#Create a directory for the PIT library;
	@mkdir -p build/mod/PIT

	#Compile four channels; Each channel takes 4 arguments :
	#	channel identifier, interrupt channel,start of the registers area;
	@$(CC) $(CFLAGS) $(KERNEL_INC) -DMAKE_ARGS=0,48,0x40037100, -o $(BK_DIR)/pit0.o -c $(SK_DIR)/kx_pit_channel.c
	@$(CC) $(CFLAGS) $(KERNEL_INC) -DMAKE_ARGS=1,49,0x40037110, -o $(BK_DIR)/pit1.o -c $(SK_DIR)/kx_pit_channel.c
	@$(CC) $(CFLAGS) $(KERNEL_INC) -DMAKE_ARGS=2,50,0x40037120, -o $(BK_DIR)/pit2.o -c $(SK_DIR)/kx_pit_channel.c
	@$(CC) $(CFLAGS) $(KERNEL_INC) -DMAKE_ARGS=3,51,0x40037120, -o $(BK_DIR)/pit3.o -c $(SK_DIR)/kx_pit_channel.c

	#Compile the pit manager; It takes two arguments, the mcr address and the number of channels;
	@$(CC) $(CFLAGS) $(KERNEL_INC) -DMAKE_ARGS=0x40037000,4 -o $(BK_DIR)/pit.o -c $(SK_DIR)/kx_pit.c

    #Link the pit module;
	@$(AR) cr build/mod/pit.a $(BK_DIR)/pit.o $(BK_DIR)/pit0.o $(BK_DIR)/pit1.o $(BK_DIR)/pit2.o $(BK_DIR)/pit3.o


#Add the PIT module to the modules list;
MODULES += build/mod/pit.a

