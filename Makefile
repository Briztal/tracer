#
# This makefile is the root make file of the project;
#


#Include the general project option file;
include build_options.mk


#--------------------------------------------------------------------- toolchain

#Architecture specific build environment is initialised by arch_builder;

#Initialise the target architecture;
__AB_TARGET_ARCH__ := $(BOPS_ARCHITECTURE)

#Enable toolchain selection;
__AB_SCRIPT_BUILD_ENV__ := 1

#Include the arch builder makefile, to init the build environment;
include $(BOPS_ARCH_BUILDER_PATH)/arch_builder.mk


#------------------------------------------------------------------------- flags

#Update generated flags;
TC_CFLAGS += -Wall -O3 -std=c89 -pedantic -ffreestanding
TC_LDFLAGS += -O3 -nostdlib

#The build directory;
BDIR := build









