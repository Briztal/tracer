#
# This makefile is the root make file of the project;
#

#----------------------------------------------------------------- build options
#
# BDIR : The directory where build files must be generated;
#
# EXECUTABLE_TYPE : The type of executable that should be generated :
#
#  - 0 : standalone : the executable is the only piece of code present.
#   the kernel executable resides in ROM;
#   the linker script concerns ROM and RAM;
#   the the executable must contain architecture startup code;
#
#  - 1 : (ROM-RAM) app : the kernel resides is executed by another program;
#   the kernel executable is made to be executed from ROM or RAM;
#   the linker script concerns ROM or RAM;
#   the executable is not supposed to contain architecture startup code;
#
#
# EXECUTION_MODE : The build execution mode :
#
#  - 0 : static : no code can be dynamically modified, and executed;
#   this mode is relevant when the RAM cannot be used to contain executable
#   code, or when efficiency is primary;
#   In this mode, running applications and dynamic modules is not possible,
#   static modules are pre-linked to the kernel;
#
#  -  1 : dynamic : code can be modified and executed dynamically;
#   this mode is relevant when code can be executed in RAM, and when execution
#   efficiency is not a major objective;
#   This mode supports dynamic modules and applications, memory protection for
#   these executables;
#   In this mode, static modules are embedded in the executable as elf files,
#   that the kernel loads at run-time;
#

#----------------------------------------------------------------- options check

ifndef BDIR
$(error the build directory has not been provided)
endif

ifndef TR_EXECUTABLE_TYPE
$(error the executable type has not been provided)
endif

ifndef TR_EXECUTION_MODE
$(error the execution mode has not been provided)
endif


#The path to arch_builder directory;
BOPS_ARCH_BUILDER_PATH := ../arch_builder


# The architecture executables should be built for;
#  The architecture must be supported by arch_builder;
BOPS_ARCHITECTURE := teensy_35


#-------------------------------------------------------------------- khal build

#If the kernel hardware abstraction layer must be built :
ifeq ($(MAKECMDGOALS),khal)

#The list of rules that are required to build the khal correctly;
# sub make files must fill this list;
KHAL_RULES :=

# arch_builder call


##TODO MAKE ANOTHER ARCH BUILDER MODULE, EXTERNAL DIR, TO INCLUDE ONLY A SET OF FILES, WITH NO TREATMENT;
#Provide the external directory;
__AB_EDIR__ := ./khal

#Include arch_entry to initialise variables;
include $(__AB_RDIR__)/arch_entry.mk


#The directory where khal source files can be found;
KHAL_SDIR = khal

#The directory where khal files must be generated;
KHAL_BDIR = $(BDIR)/khal

#The directory where to generate khal object files;
KHAL_OBJS_BDIR = $(KHAL_BDIR)/objs


#Compilation flags proper to the khal;
KHAL_FLAGS := -DBOPS_EXECUTABLE_TYPE=$(BOPS_EXECUTABLE_TYPE)

#The khal compilation shortcut;
KHAL_CC = $(TC_CC) -Iinclude/ -Ikhal/_inc $(TC_CFLAGS) $(KHAL_FLAGS)


# khal build

#arch_init : pre-build tasks
khal_dirs :

#Create the directoy tree;
	mkdir -p $(KHAL_BDIR)
	mkdir -p $(KHAL_OBJS_BDIR)


#khal : in charge of merging all khal object files into one object file,
# and to modify the symbols table;
khal : khal_dirs $(KHAL_RULES)

#Merge all khal objects in one object;
	$(TC_LD) -r $(wildcard $(KHAL_OBJS_BDIR)/*.o) -o $(KHAL_BDIR)/khal_unhidden.o

#Hide non global symbols;
	$(TC_OC) -w -G __* $(KHAL_BDIR)/khal_unhidden.o $(KHAL_BDIR)/khal.o

#Copy headers in the build folder for external use;
	cp -r $(KHAL_SDIR)/_inc/ $(KHAL_BDIR)/include/



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

clean_all :
	-rm -rf build











