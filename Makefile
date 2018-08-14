
TARGET = tracer

#All sources will be built in ./build.
BUILDDIR := $(abspath $(CURDIR)/build)


# additional libraries to link TODO REMOVE
LINK_LIBS := -lm


#include paths for libraries
L_INC := src/data_structures
L_INC += src/kernel

#initialise compilation and linking flags;
CFLAGS := -Wall -Os -g
LDFLAGS := -Wl,--gc-sections,--defsym=__rtc_localtime=0 -Os -T$(abspath $(CURDIR)/src/hardware/proc/cortex_m4f/rlink.ld)
#TODO MAKE A GENERAL LINKER SCRIPT AND LEAVE VECTOR SECTION AND FLASH/RAM TO HARDWARE LIBS;

#Initialise hardware source files;
HW_SOURCES :=

#Initialise code source files;
SW_SOURCES :=


#include the teensy35 makefile that will initialise the toolchain environment and define basic rules;
include src/hardware/board/teensy35/Makefile


