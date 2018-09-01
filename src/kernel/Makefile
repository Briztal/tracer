
#Cache the current directory;
KERNEL_DIR := kernel

#Provide access to the kernel directory for include;
KERNEL_PRIV_DIR += $(KERNEL_DIR)

#The syscall source file can be linked to exernal libs;
KERNEL_PUB_SRCS += $(KERNEL_DIR)/syscall.c

#Add all files of the kernel dir;
KERNEL_SRCS += $(KERNEL_DIR)/krnl.c
KERNEL_SRCS += $(KERNEL_DIR)/mem.c
KERNEL_SRCS += $(KERNEL_DIR)/panic.c
KERNEL_SRCS += $(KERNEL_DIR)/systick.c

#Add the scheduler folder
KERNEL_SRCS += $(KERNEL_DIR)/scheduler/prc.c
KERNEL_SRCS += $(KERNEL_DIR)/scheduler/sched.c
