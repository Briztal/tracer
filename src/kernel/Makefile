
#Cache the current directory;
KERNEL_DIR := kernel

#Provide access to the kernel directory for include;
KERNEL_PRIV_DIR += $(KERNEL_DIR)

#The syscall source file can be linked to exernal libs;
KERNEL_PUB_SRCS += $(KERNEL_DIR)/syscall.c

#Add general core sources;
KERNEL_SRCS +=  $(KERNEL_DIR)/core.c

#Add memory management sources;
KERNEL_SRCS += $(KERNEL_DIR)/mem/ram.c
KERNEL_SRCS += $(KERNEL_DIR)/mem/heap.c
KERNEL_SRCS += $(KERNEL_DIR)/mem/mem.c


#Add the communication protocol;
KERNEL_SRCS +=  $(KERNEL_DIR)/debug/probe.c
KERNEL_SRCS +=  $(KERNEL_DIR)/debug/debug.c

#Struct sources;
KERNEL_SRCS += $(KERNEL_DIR)/struct/nlist.c
KERNEL_SRCS += $(KERNEL_DIR)/struct/shared_fifo.c


#Add all files of the kernel dir;
KERNEL_SRCS += $(KERNEL_DIR)/krnl.c
KERNEL_SRCS += $(KERNEL_DIR)/panic.c
KERNEL_SRCS += $(KERNEL_DIR)/systick.c

#Mod sources;
KERNEL_SRCS += $(KERNEL_DIR)/mod/mod.c


#Add the scheduler folder
KERNEL_SRCS += $(KERNEL_DIR)/scheduler/prc.c
KERNEL_SRCS += $(KERNEL_DIR)/scheduler/sched.c
