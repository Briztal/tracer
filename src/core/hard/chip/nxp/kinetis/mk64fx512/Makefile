
include src/core/hard/proc/cortex_m4f/Makefile


#---------------------------------------------- Core files ----------------------------------------------

#add both directories to the core include path;
CORE_INC += -Isrc/core/hard/chip/nxp/kinetis

#define the current dir here;
K_DIR	:=  core/hard/chip/nxp/kinetis

#Add the chip source file to the hw sources list;
CORE_SRCS +=  $(K_DIR)/mk64fx512/mk64fx512.c
CORE_SRCS +=  $(K_DIR)/kx_mcg.c
CORE_SRCS +=  $(K_DIR)/kx_wdog.c

#Set the memory map script;
LDSCRIPT_MMAP_DIR := src/core/hard/chip/nxp/kinetis/mk64fx512/


#---------------------------------------------- Peripheral Modules ----------------------------------------------


#------------------ Periodi Interrupt Timer ------------------

#Add the pit module to the kernel dependencies
KERNEL_DEP += pit

#Hardware specs are required to build the PIT
PIT_FLAGS = -D__MCR__=0x40037000 -D__REGISTERS_START__=0x40037100 -D__REGISTERS_SPACING__=0x10 -D__NB_CHANNELS__=4

#This rule will build the pit archive;
pit:

	#Create a directory for the PIT library;
	@mkdir -p build/mod/PIT

	#Compile four channels;
	@$(CC) $(CFLAGS) $(PIT_FLAGS) $(CORE_INC) -D__INT_CHANNEL__=48 -D__CHANNEL_ID__=0 -o build/mod/PIT/pit_channel_0 -c src/$(K_DIR)/kx_pit_channel.c
	@$(CC) $(CFLAGS) $(PIT_FLAGS) $(CORE_INC) -D__INT_CHANNEL__=49 -D__CHANNEL_ID__=0 -o build/mod/PIT/pit_channel_0 -c src/$(K_DIR)/kx_pit_channel.c
	@$(CC) $(CFLAGS) $(PIT_FLAGS) $(CORE_INC) -D__INT_CHANNEL__=50 -D__CHANNEL_ID__=0 -o build/mod/PIT/pit_channel_0 -c src/$(K_DIR)/kx_pit_channel.c
	@$(CC) $(CFLAGS) $(PIT_FLAGS) $(CORE_INC) -D__INT_CHANNEL__=51 -D__CHANNEL_ID__=0 -o build/mod/PIT/pit_channel_0 -c src/$(K_DIR)/kx_pit_channel.c

	#Compile four channels;
	@$(CC) $(CFLAGS) $(PIT_FLAGS) -o build/mod/PIT/pit -c $(K_DIR)/kx_pit.c

    #Archive the module to hide symbols;
	@$(AR) cr build/mod/PIT/libpit.ar


#Add the PIT module to the modules list;
MODULES += buld/mod/PIT/libpit.ar

