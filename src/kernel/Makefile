
#----------------------------------------------- Kernel compilation flags ----------------------------------------------

#if the kernel is not compiled in scheduling mode :
ifndef KERNEL_SCHED_MODE

#Add the loop flag to the compilation flags set;
CFLAGS += -DKERNEL_LOOP_MODE

endif #KERNEL_SCHED_MODE

#Add the dynamic memory size;
CFLAGS += -DKDM_SIZE=$(KDM_SIZE)

#Add the exception stacks size requirements;
CFLAGS += -DKEX_STACK_SIZE=$(KEX_STACK_SIZE)

#Add first process hardware requirements;
CFLAGS += -DKFP_RAM_SIZE=$(KFP_RAM_SIZE) -DKFP_STACK_SIZE=$(KFP_STACK_SIZE) -DKFP_ACTIVITY_TIME=$(KFP_ACTIVITY_TIME)

#----------------------------------------------------- Files ----------------------------------------------------

#Cache the current directory;
KERNEL_DIR := kernel

#The syscall source file can be linked to exernal libs;

KERNEL_SRCS += $(KERNEL_DIR)/async/fault.c
KERNEL_SRCS += $(KERNEL_DIR)/async/interrupt.c
KERNEL_SRCS += $(KERNEL_DIR)/async/preempt.c


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
KERNEL_SRCS += $(KERNEL_DIR)/interface/command.c
KERNEL_SRCS += $(KERNEL_DIR)/interface/gpio.c
KERNEL_SRCS += $(KERNEL_DIR)/interface/timer.c


#Add memory management sources;
KERNEL_SRCS += $(KERNEL_DIR)/mem/heap.c
KERNEL_SRCS += $(KERNEL_DIR)/mem/kdmem.c
KERNEL_SRCS += $(KERNEL_DIR)/mem/ram.c


#Mod sources;
KERNEL_SRCS += $(KERNEL_DIR)/mod/mod.c


#if the kernel is compiled in scheduling mode :
ifdef KERNEL_SCHED_MODE

#Add the sched folder
KERNEL_SRCS += $(KERNEL_DIR)/sched/prc_mem.c
KERNEL_SRCS += $(KERNEL_DIR)/sched/proc.c
KERNEL_SRCS += $(KERNEL_DIR)/sched/sched.c

endif #KERNEL_SCHED_MODE

#Struct sources;
KERNEL_SRCS += $(KERNEL_DIR)/struct/nlist.c
KERNEL_SRCS += $(KERNEL_DIR)/struct/shared_fifo.c


#Add all files of the kernel dir;
KERNEL_SRCS += $(KERNEL_DIR)/kinit.c
KERNEL_SRCS += $(KERNEL_DIR)/panic.c

KERNEL_PUB_SRCS += $(KERNEL_DIR)/syscall.c

