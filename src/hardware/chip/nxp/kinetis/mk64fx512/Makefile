
include src/hardware/proc/cortex_m4f/Makefile

#define the current dir here;
MK64_DIR :=  src/harware/chip/nxp/kinetis/mk64fx512

#add the k64 directory to the include path;
L_INC += $(MK64_DIR)

#Add the chip source file to the hw sources list;
HW_SOURCES +=  $(MK64_DIR)/mk64fx512.c

#Add compilation flags related to the chip;
CFLAGS += -D__MK64FX512__ -DF_CPU=120000000