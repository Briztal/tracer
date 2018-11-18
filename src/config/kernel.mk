
#------------------------------------------------- Kernel Flags -------------------------------------------------

#A special set of flags, to configure the kernel's behaviour.
KFLAGS :=


#------------------------------------------------- Coprocessors -------------------------------------------------

#The number of coprocessors. Likely to be changed by the hardware lib;
KERNEL_NB_COPROCESSORS = 0


#------------------------------------------------- Dynamic memory size -------------------------------------------------

#The size allocated to the kernel dynamic memory;
KDM_SIZE = 8000


#------------------------------------------------- Scheduler lib config ------------------------------------------------

#Following lines are used only if the scheduler mode is enabled;

#The kernel exception stack_data size;
KEX_STACK_SIZE = 2048

#The kernel first process parameters;
KFP_RAM_SIZE = 4096
KFP_STACK_SIZE = 2048
KFP_ACTIVITY_TIME = 2000