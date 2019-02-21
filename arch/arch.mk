#
# This makefile builds architecture dependent code :
#
#	- The kernel hardware abstraction layer (khal) : linked statically against the kernel, all functions that are
#		required to make the kernel work properly on the target architecture;
#		It will be composed of a single object file, khal.o, generated in a specific directory;
#
#	- The kernel arch modules set : a set of modules to drive specific peripherals, that will be embedded in the
#		kernel executable;
#		It will be composed of a set of object files, one for each module (modules that comprise several .o files must
#		be aggregated with ld), that will be generated in a specific directory;
#
#	It also initializes the build environment (CC, LD, SIZE etc...), for all other make files;
#
#


#------------------------------------------------------ khal env -------------------------------------------------------

ifdef BUILD_KHAL

#The directory where to generate the merges khal object file
KHAL_BDIR = $(BDIR)

#The directory where to generate khal object files;
KHAL_OBJS_BDIR = $(BDIR)/khal_tmp

#The list of rules that are required to build the khal correctly; sub make files must fill this list;
KHAL_RULES :=

#Compilation flags proper to the khal;
KHAL_FLAGS := -DBOPS_EXECUTABLE_TYPE=$(BOPS_EXECUTABLE_TYPE)

#The khal compilation shortcut;
KHAL_CC = $(CC) -Iarch/ -Iinclude/ $(CFLAGS) $(KHAL_FLAGS)

endif


#---------------------------------------------------- arch mods env ----------------------------------------------------

#This build section is only usefull if arch_mods must be built;
ifdef BUILD_ARCH_MODS

#The directory where to build architecture modules objects;
AMOD_BDIR = $(BDIR)/arch_mods

#The directory where to build temporary modules objects;
AMOD_TMP_BDIR = $(BDIR)/arch_mods_tmp

#The list of rules that are required to build arch modules; sub make files must fill this list;
AMOD_RULES :=

endif


#------------------------------------------------- arch dependent code -------------------------------------------------

#Include the arch build makefile;
include $(BOPS_ARCHITECTURE)


#----------------------------------------------------- khal build ------------------------------------------------------

ifdef BUILD_KHAL

#arch_init : pre-build tasks
khal_init :

#Create the directoy tree;
	mkdir -p $(KHAL_BDIR)
	mkdir -p $(KHAL_OBJS_BDIR)


#arch_khal : in charge of merging all khal object files into one object file, and to modify the symbols table;
khal : khal_init $(KHAL_RULES)

#Merge all khal objects in one object;
	$(LD) -r $(wildcard $(KHAL_OBJS_BDIR)/*.o) -o $(KHAL_BDIR)/khal.o

endif

#--------------------------------------------------- arch_mods build ---------------------------------------------------

ifdef BUILD_ARCH_MODS

arch_mods_init :

#Create the directoy tree;
	mkdir -p $(AMOD_BDIR)
	mkdir -p $(AMOD_TMP_BDIR)


#arch_khal : in charge of merging all khal object files into one object file, and to modify the symbols table;
arch_mods : arch_mods_init $(AMOD_RULES)

endif