
include src/core/proc/cortex_m4f/Makefile


#Add compilation flags related to the chip;
CFLAGS += -D__MK64FX512__ -DF_CPU=120000000 -DCHIP_HEADER=<mk64fx512.h>

#define the current dir here;
MK64_DIR :=  src/core/chip/nxp/kinetis/mk64fx512

#add the K64 directory to the core include path;
CORE_INC += -I$(MK64_DIR)

#Add the chip source file to the hw sources list;
CORE_SRCS +=  $(MK64_DIR)/mk64fx512.c

#Set the memory map script;
LDSCRIPT_MMAP_DIR := src/core/chip/nxp/kinetis/mk64fx512/
