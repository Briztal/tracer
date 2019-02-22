#
# This makefile is the root make file of the project;
#
#	It  the following targets :
#
#	- arch : sets the build environment, builds the kernel hardware abstraction layer and the arch modules set; see arch.mk
#
#
#
# TODO A WORD ON FILE NAMES IN MAKEFILES;
#
#
#
#
#
#
#
#
#
#
#
#
#
#
#
#


#--------------------------------------------------- authorised goals --------------------------------------------------

# The project is built in several exclusive steps, that each must be called successively


#If the goal target is the kernel hardware abstraction layer, enable the inclusion of khal build code;
ifeq ($(MAKECMDGOALS),khal)
BUILD_KHAL := 1
endif

#If the goal target is the set of arch modules, enable the inclusion of arch_mods build code;
ifeq ($(MAKECMDGOALS),arch_mods)
BUILD_ARCH_MODS := 1
endif

#If the goal target is the kernel builder, enable the inclusion of kernel build code;
ifeq ($(MAKECMDGOALS),kernel)
BUILD_KERNEL := 1
endif

#If the goal target is the kernel packer, enable the inclusion of kernel pack code;
ifeq ($(MAKECMDGOALS),kernel_pack)
PACK_KERNEL := 1
endif


#------------------------------------------------------ build env ------------------------------------------------------

#The build environment allows the abstract manipulation of source, objects and executable files.
# Architecture dependent variables are  updated by the arch make unit;

#The preprocessor, compiler, assembler;
CC =

#The linker;
LD =

#The archiver;
AR :=

#The elf manipulator;
OBJCOPY :=

#The elf analyzer;
SIZE :=

#General compilation flags;
CFLAGS := -Wall -O3 -g -std=c89 -pedantic -ffreestanding

#General linking flags;
LDFLAGS := -Wall -Wl,--gc-sections -O3 -std=c89 -pedantic -nostdlib

#The link script for the target platform;
LDSCRIPT_MMAP_DIR :=

#The build directory;
BDIR := build


#---------------------------------------------------- build options ----------------------------------------------------

#Include the general project option file; these options are required for all targets;
include build_options.mk


#---------------------------------------------------- arch make unit ---------------------------------------------------

#The arch make unit is in charge of :
#	- updating the build environment according to the target architecture;
#	- providing khal build code, if required;
#	- providing arch_mods build code, if required;

include arch/arch.mk


#--------------------------------------------------- kernel make unit --------------------------------------------------

#The kernel make unit is in charge of building the generic part of the kernel, if requied;

#If the kernel must be built, include its build code;
ifdef BUILD_KERNEL
include kernel/kernel.mk
endif


#------------------------------------------------------- cleanup -------------------------------------------------------


clean :
	rm -rf build











