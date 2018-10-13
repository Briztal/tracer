
#----------------------------------------------------- Kernel Flags ----------------------------------------------------

#The kernel requires the following set of flags to be initialised by the hardware lib :
#HARD_NB_THREADS : the number of hardware threads;
ifdef HARD_NB_THREADS



#----------------------------------------------------- Kernel Flags ----------------------------------------------------

#Cache the current directory;
KERNEL_DIR := kernel

#Provide access to the kernel directory for include;
KERNEL_PRIV_DIR += $(KERNEL_DIR)

#The syscall source file can be linked to exernal libs;

KERNEL_SRCS += $(KERNEL_DIR)/async/fault.c
KERNEL_SRCS += $(KERNEL_DIR)/async/interrupt.c


#Add the clock lib;
KERNEL_SRCS +=  $(KERNEL_DIR)/clock/clock.c
KERNEL_SRCS += $(KERNEL_DIR)/clock/sysclock.c


#Add the communication protocol;
KERNEL_SRCS +=  $(KERNEL_DIR)/debug/probe.c
KERNEL_SRCS +=  $(KERNEL_DIR)/debug/debug.c


#Add the communication protocol;
KERNEL_SRCS +=  $(KERNEL_DIR)/fs/inode.c

#Add timer structs
KERNEL_SRCS +=  $(KERNEL_DIR)/interface/command.c
KERNEL_SRCS +=  $(KERNEL_DIR)/interface/gpio.c
KERNEL_SRCS +=  $(KERNEL_DIR)/interface/timer.c


#Add memory management sources;
KERNEL_SRCS += $(KERNEL_DIR)/mem/ram.c
KERNEL_SRCS += $(KERNEL_DIR)/mem/heap.c
KERNEL_SRCS += $(KERNEL_DIR)/mem/mem.c


#Add the run folder
KERNEL_SRCS += $(KERNEL_DIR)/proc/proc.c

#Mod sources;
KERNEL_SRCS += $(KERNEL_DIR)/mod/mod.c

#Add the scheduler folder
KERNEL_SRCS += $(KERNEL_DIR)/scheduler/prc.c
KERNEL_SRCS += $(KERNEL_DIR)/scheduler/sched.c

#Struct sources;
KERNEL_SRCS += $(KERNEL_DIR)/struct/nlist.c
KERNEL_SRCS += $(KERNEL_DIR)/struct/shared_fifo.c


#Add all files of the kernel dir;
KERNEL_SRCS += $(KERNEL_DIR)/krnl.c
KERNEL_SRCS += $(KERNEL_DIR)/log.c
KERNEL_SRCS += $(KERNEL_DIR)/panic.c

KERNEL_PUB_SRCS += $(KERNEL_DIR)/syscall.c

