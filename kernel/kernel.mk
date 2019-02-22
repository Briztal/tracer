#
# This file contains build code for the generic part of the kenrel; It is included if required only by the
#

#Directories where kernel sources are located;
KRNL_SDIR := kernel
KRNL_CORE_SDIR := $(KRNL_SDIR)/core

#Directories where to generate the merges khal object file;
KRNL_BDIR = $(BDIR)/kernel

#The directory where to generate khal object files;
KRNL_OBJS_BDIR = $(KRNL_BDIR)/objs
KRNL_CORE_BDIR := $(KRNL_OBJS_BDIR)/core

#Compilation flags proper to the khal;
KRNL_FLAGS :=

#The khal compilation shortcut;
KRNL_CC = $(CC) -Iinclude/ -Iinclude/nostd $(CFLAGS) $(KRNL_FLAGS)


#------------------------------------------- Components configuration flags --------------------------------------------

#Add the exception stacks size requirements;
#KRNL_FLAGS += -DKEX_STACK_SIZE=$(KEX_STACK_SIZE)
#Add first process hardware requirements;
#KRNL_FLAGS += -DKFP_RAM_SIZE=$(KFP_RAM_SIZE)
#KRNL_FLAGS += -DKFP_STACK_SIZE=$(KFP_STACK_SIZE)
#KRNL_FLAGS += -DKFP_ACTIVITY_TIME=$(KFP_ACTIVITY_TIME)
#KRNL_FLAGS += -DKDM_SIZE=$(KDM_SIZE)
#KRNL_FLAGS += -DNB_COPROCESSORS=$(KERNEL_NB_COPROCESSORS)


#---------------------------------------------------- kernel build -----------------------------------------------------

kernel_dirs :
	mkdir -p $(KRNL_BDIR)
	mkdir -p $(KRNL_CORE_BDIR)

kernel_core :

	$(KRNL_CC) -c $(KRNL_CORE_SDIR)/debug.c -o $(KRNL_CORE_BDIR)/debug.o
	$(KRNL_CC) -c $(KRNL_CORE_SDIR)/except.c -o $(KRNL_CORE_BDIR)/except.o
	$(KRNL_CC) -c $(KRNL_CORE_SDIR)/fault.c -o $(KRNL_CORE_BDIR)/fault.o
	$(KRNL_CC) -c $(KRNL_CORE_SDIR)/ram.c -o $(KRNL_CORE_BDIR)/ram.o


kernel_res :


kernel_exec :



kernel_build :

#Build the common code object;
	$(KRNL_CC) -c $(KRNL_SDIR)/common.c -o $(KRNL_OBJS_BDIR)/common.o
	$(KRNL_CC) -c $(KRNL_SDIR)/kinit.c -o $(KRNL_OBJS_BDIR)/kinit.o


#---------------------------------------------------- kernel pack -----------------------------------------------------

kernel : build/khal/khal.o kernel_dirs kernel_core kernel_res kernel_exec kernel_build

#Merge kernel objects;
	$(LD) -r $(wildcard $(KRNL_OBJS_BDIR)/**/*.o) $(wildcard $(KRNL_OBJS_BDIR)/*.o) -o $(KRNL_BDIR)/kernel_generic_unhidden.o

#Hide non global symbols;
	$(OBJCOPY) -w -G __* $(KRNL_BDIR)/kernel_generic_unhidden.o $(KRNL_BDIR)/kernel_generic.o

#Merge kernel and khal;
	$(LD) -r build/khal/khal.o $(KRNL_BDIR)/kernel_generic.o -o $(KRNL_BDIR)/kernel.o




