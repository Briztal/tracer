
include src/core/proc/cortex_m4f/Makefile


#Add compilation flags related to the chip;
CFLAGS += -D__MK64FX512__ -DF_CPU=120000000

#define the current dir here;
K64_DIR :=  src/core/chip/nxp/kinetis/mk64fx512
K_DIR :=  src/core/chip/nxp/kinetis

#add the K64 directory to the core include path;
CORE_INC += -I$(MK64_DIR)

#Add the chip source file to the hw sources list;
CORE_SRCS +=  $(K64_DIR)/mk64fx512.c
CORE_SRCS +=  $(K_DIR)/kx_mcg.c
CORE_SRCS +=  $(K_DIR)/kx_sim.c
CORE_SRCS +=  $(K_DIR)/kx_wdog.c

#Set the memory map script;
LDSCRIPT_MMAP_DIR := src/core/chip/nxp/kinetis/mk64fx512/
