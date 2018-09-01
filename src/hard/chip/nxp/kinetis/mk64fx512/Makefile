
include src/hard/proc/cortex_m4f/Makefile


#add both directories to the core include path;
HARD_INC += -Isrc/hard/chip/nxp/kinetis


#define the current dir here;
K64_DIR :=  hard/chip/nxp/kinetis/mk64fx512
K_DIR	:=  hard/chip/nxp/kinetis


#Add the chip source file to the hw sources list;
SYS_SRCS +=  $(K64_DIR)/mk64fx512.c
SYS_SRCS +=  $(K_DIR)/kx_mcg.c
SYS_SRCS +=  $(K_DIR)/kx_sim.c
SYS_SRCS +=  $(K_DIR)/kx_wdog.c

#Set the memory map script;
LDSCRIPT_MMAP_DIR := src/hard/chip/nxp/kinetis/mk64fx512/
