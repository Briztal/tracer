#
# This makefile is the root make file of the project;
#


#----------------------------------------------------------------- build options

#Include the general project option file;
include build_options.mk


#------------------------------------------------------------- build environment

#Architecture specific build environment is initialised by arch_builder;

#Initialise the target architecture;
__AB_TARGET_ARCH__ := $(BOPS_ARCHITECTURE)

#Enable toolchain selection;
__AB_SCRIPT_BUILD_ENV__ := 1

#Include the arch builder makefile, to init the build environment;
include $(BOPS_ARCH_BUILDER_PATH)/arch_builder.mk

#Update generated flags;
TC_CFLAGS += -Wall -O3 -std=c89 -pedantic -ffreestanding
TC_LDFLAGS += -O3 -nostdlib

#The build directory;
BDIR := build



#-------------------------------------------------------------------- khal build

#If the kernel hardware abstraction layer must be built :
ifeq ($(MAKECMDGOALS),khal)

#Initialise the target architecture;
__AB_TARGET_ARCH__ := $(BOPS_ARCHITECTURE)

#Enable custom script execution
__AB_SCRIPT_EXTERNAL__ := khal/khal.mk

#Include the arch builder makefile, to execute the khal build script;
include $(BOPS_ARCH_BUILDER_PATH)/arch_builder.mk

endif


#------------------------------------------------------------------ kernel build

#If the kernel must be built :
ifeq ($(MAKECMDGOALS),kernel)

#Include the kernel's build code;
include kernel/kernel.mk

endif


#----------------------------------------------------------------------- cleanup


clean :
	rm -rf build











