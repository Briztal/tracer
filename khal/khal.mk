
#---------------------------------------------------------------------- khal env

#The directory where to generate the merges khal object file
KHAL_BDIR = $(BDIR)/khal

#The directory where to generate khal object files;
KHAL_OBJS_BDIR = $(KHAL_BDIR)/objs

#The list of rules that are required to build the khal correctly;
# sub make files must fill this list;
KHAL_RULES :=

#Compilation flags proper to the khal;
KHAL_FLAGS := -DBOPS_EXECUTABLE_TYPE=$(BOPS_EXECUTABLE_TYPE)

#The khal compilation shortcut;
KHAL_CC = $(TC_CC) -Iinclude/ $(TC_CFLAGS) $(KHAL_FLAGS)

#------------------------------------------------------------- arch_builder call

#Provide the external directory;
__AB_EDIR__ := ./khal

#Include arch_entry to initialise variables;
include $(__AB_RDIR__)/arch_entry.mk


#-------------------------------------------------------------------- khal build

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

