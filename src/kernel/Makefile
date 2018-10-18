
#----------------------------------------------- Kernel compilation flags ----------------------------------------------

#if the kernel is not compiled in scheduling mode :
ifdef KERNEL_EXECUTION_LEVEL

#Add the loop flag to the compilation flags set;
CFLAGS += -DEXEC_LEVEL=$(KERNEL_EXECUTION_LEVEL)

endif #KERNEL_SCHED_MODE

#Add the dynamic memory size;
CFLAGS += -DKDM_SIZE=$(KDM_SIZE)

#Add the exception stacks size requirements;
CFLAGS += -DKEX_STACK_SIZE=$(KEX_STACK_SIZE)

#Add first process hardware requirements;
CFLAGS += -DKFP_RAM_SIZE=$(KFP_RAM_SIZE) -DKFP_STACK_SIZE=$(KFP_STACK_SIZE) -DKFP_ACTIVITY_TIME=$(KFP_ACTIVITY_TIME)


#----------------------------------------------------- Files ----------------------------------------------------


#-------------------------------------------------- Implicit make rule -------------------------------------------------

#TODO NO IMPLICIT RULE

#The general compilation rule. Compiles a source file in src/, saves the output in the corresponding path in build/;
#Requires the compiler (CC) to be up to date, and all C flags to be appropriately set;
build/%.o: src/%.c

#Create a dir if necessary;TODO NOPE, TAKES WAY TOO MUCH TIME !
	@mkdir -p "$(dir $@)"

#Display the name of the source we compile;
	@echo "[CC]\t$@"

#Compile, providing only src in include path, and all C flags;
	@$(CC) $(CFLAGS) $(INC) -o $@ -c $<




#Cache the current directory;
KERNEL_DIR := kernel

#The syscall source file can be linked to exernal libs;

KERNEL_SRCS += $(KERNEL_DIR)/async/fault.c
KERNEL_SRCS += $(KERNEL_DIR)/async/except.c


#Add the clock lib;
KERNEL_SRCS += $(KERNEL_DIR)/clock/clock.c
KERNEL_SRCS += $(KERNEL_DIR)/clock/sysclock.c


#Add the communication protocol;
KERNEL_SRCS += $(KERNEL_DIR)/debug/probe.c
KERNEL_SRCS += $(KERNEL_DIR)/debug/debug.c
KERNEL_SRCS += $(KERNEL_DIR)/debug/log.c


#Add the communication protocol;
KERNEL_SRCS += $(KERNEL_DIR)/fs/inode.c


#Add timer structs
KERNEL_SRCS += $(KERNEL_DIR)/interface/cmd.c
KERNEL_SRCS += $(KERNEL_DIR)/interface/gpio.c
KERNEL_SRCS += $(KERNEL_DIR)/interface/timer.c


#Add memory management sources;
KERNEL_SRCS += $(KERNEL_DIR)/mem/heap.c
KERNEL_SRCS += $(KERNEL_DIR)/mem/kdmem.c
KERNEL_SRCS += $(KERNEL_DIR)/mem/ram.c


#Mod sources;
KERNEL_SRCS += $(KERNEL_DIR)/mod/mod.c


#if the kernel execution level is not zero :
ifneq (KERNEL_EXEC_MODE, 0)

#Add the run folder
KERNEL_SRCS += $(KERNEL_DIR)/run/pmem.c
KERNEL_SRCS += $(KERNEL_DIR)/run/proc.c
KERNEL_SRCS += $(KERNEL_DIR)/run/sched.c

endif #(KERNEL_EXEC_MODE, 0)

#Struct sources;
KERNEL_SRCS += $(KERNEL_DIR)/struct/nlist.c
KERNEL_SRCS += $(KERNEL_DIR)/struct/shared_fifo.c


#Add all files of the kernel dir;
KERNEL_SRCS += $(KERNEL_DIR)/kinit.c
KERNEL_SRCS += $(KERNEL_DIR)/panic.c
KERNEL_SRCS += $(KERNEL_DIR)/syscall.c


KERNEL_PUB_SRCS += $(KERNEL_DIR)/syscall.c

