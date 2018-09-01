
include src/core/proc/cortex_m4f/Makefile


#add both directories to the core include path;
CORE_INC += -Isrc/core/chip/nxp/kinetis
#CORE_INC += -Isrc/core/chip/nxp/kinetis/mk64fx512


#define the current dir here;
K64_DIR :=  core/chip/nxp/kinetis/mk64fx512
K_DIR	:=  core/chip/nxp/kinetis


#Add the chip source file to the hw sources list;
CORE_SRCS +=  $(K64_DIR)/mk64fx512.c
CORE_SRCS +=  $(K_DIR)/kx_mcg.c
CORE_SRCS +=  $(K_DIR)/kx_sim.c
CORE_SRCS +=  $(K_DIR)/kx_wdog.c

#Set the memory map script;
LDSCRIPT_MMAP_DIR := src/core/chip/nxp/kinetis/mk64fx512/
