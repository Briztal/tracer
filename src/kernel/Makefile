
#Cache the current directory;
KERNEL_DIR := src/kernel

#Provide access to the kernel directory for include;
KERNEL_PRIV_DIR += $(KERNEL_DIR)

#The kernel source file can be linked to exernal libs;
KERNEL_EXT_SRC += $(KERNEL_DIR)/syscall.c

#Register all files of the kernel dir;
KERNEL_INT_SRC += $(KERNEL_DIR)/krnl.c
KERNEL_INT_SRC += $(KERNEL_DIR)/mem.c
KERNEL_INT_SRC += $(KERNEL_DIR)/panic.c
KERNEL_INT_SRC += $(KERNEL_DIR)/systick.c
