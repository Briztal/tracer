
include src/hardware/proc/cortex_m4f/Makefile

#include src/drivers/K64/Makefile


#define the current dir here;
MK64_DIR :=  src/hardware/chip/nxp/kinetis/mk64fx512

#add the K64 directory to the include path;
COMPIL_INC += -I$(MK64_DIR)

#Add the chip source file to the hw sources list;
CORE_SOURCES +=  $(MK64_DIR)/mk64fx512.c

#Add compilation flags related to the chip;
CFLAGS += -D__MK64FX512__ -DF_CPU=120000000

#Set the memory map script;
LDSCRIPT_MMAP_DIR := src/hardware/chip/nxp/kinetis/mk64fx512/
