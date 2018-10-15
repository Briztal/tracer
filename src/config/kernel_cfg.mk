
#------------------------------------------------- Dynamic memory size -------------------------------------------------

#The size allocated to the kernel dynamic memory;
KDM_SIZE = 8000

#-------------------------------------------------- Run mode selection -------------------------------------------------

#Comment this line to disable the scheduler mode;
KERNEL_SCHED_MODE = 1


#------------------------------------------------- Scheduler lib config ------------------------------------------------

#Following lines are used only if the scheduler mode is enabled;

#The kernel exception stack size;
KEX_STACK_SIZE = 2048

#The kernel first process parameters;
KFP_RAM_SIZE = 4096
KFP_STACK_SIZE = 2048
KFP_ACTIVITY_TIME = 2000