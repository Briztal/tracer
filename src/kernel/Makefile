

#------------------------------------------- Components configuration flags --------------------------------------------

#if the kernel is not compiled in scheduling mode :
ifdef KERNEL_EXECUTION_LEVEL

#Add the loop flag to the compilation flags set;
CFLAGS += -DEXEC_LEVEL=$(KERNEL_EXECUTION_LEVEL)

endif #KERNEL_EXECUTION_LEVEL

#-------------------------------------------------- Compilation rules --------------------------------------------------

KS := src/kernel
KB := build/krnl

_CC_ = $(CC) $(CFLAGS) $(KFLAGS) $(INC)

kernel_std :

	@$(_CC) -o $(KB)/fault.o 	-c $(KS)/async/fault.c
	@$(_CC) -o $(KB)/except.o 	-c $(KS)/async/except.c

	@$(_CC) -o $(KB)/clock.o 	-c $(KS)/clock/clock.c
	@$(_CC) -o $(KB)/sysclock.o -c $(KS)/clock/sysclock.c

	@$(_CC) -o $(KB)/probe.o 	-c $(KS)/debug/probe.c
	@$(_CC) -o $(KB)/debug.o 	-c $(KS)/debug/debug.c
	@$(_CC) -o $(KB)/log.o 		-c $(KS)/debug/log.c

	@$(_CC) -o $(KB)/inode.o 	-c $(KS)/fs/inode.c

	@$(_CC) -o $(KB)/cmd.o 		-c $(KS)/interface/cmd.c
	@$(_CC) -o $(KB)/gpio.o 	-c $(KS)/interface/gpio.c
	@$(_CC) -o $(KB)/timer.o 	-c $(KS)/interface/timer.c

	@$(_CC) -o $(KB)/heap.o 	-c $(KS)/mem/heap.c
	@$(_CC) -o $(KB)/kdmem.o 	-c $(KS)/mem/kdmem.c -DKDM_SIZE=$(KDM_SIZE)
	@$(_CC) -o $(KB)/ram.o 		-c $(KS)/mem/ram.c

	@$(_CC) -o $(KB)/mod.o 		-c $(KS)/mod/mod.c

	@$(_CC) -o $(KB)/syscall.o -c $(KS)/public/syscall.c

#Struct sources;
	@$(_CC) -o $(KB)/nlist.o 	-c $(KS)/struct/nlist.c
	@$(_CC) -o $(KB)/shared_fifo.o -c $(KS)/struct/shared_fifo.c


#Add all files of the kernel dir;
	@$(_CC) -o $(KB)/kinit.o 	-c $(KS)/kinit.c
	@$(_CC) -o $(KB)/panic.o 	-c $(KS)/panic.c



#Add the exception stacks size requirements;
run_flags := -DKEX_STACK_SIZE=$(KEX_STACK_SIZE)

#Add first process hardware requirements;
run_flags += -DKFP_RAM_SIZE=$(KFP_RAM_SIZE)
run_flags += -DKFP_STACK_SIZE=$(KFP_STACK_SIZE)
run_flags += -DKFP_ACTIVITY_TIME=$(KFP_ACTIVITY_TIME)

KFLAGS += -DNB_COPROCESSORS=$(KERNEL_NB_COPROCESSORS)

kernel_run :

#Add the run folder
	@$(_CC_) -o $(KB)/pmem.o 	-c $(KS)/run/pmem.c $(run_flags)
	@$(_CC_) -o $(KB)/coproc.o 	-c $(KS)/run/coproc.c $(run_flags)
	@$(_CC_) -o $(KB)/proc.o 	-c $(KS)/run/proc.c $(run_flags)
	@$(_CC_) -o $(KB)/sched.o 	-c $(KS)/run/sched.c $(run_flags)



#-------------------------------------------------- Components Config --------------------------------------------------

KRNL_RULES += kernel_std


#if the kernel execution level is not zero :
ifneq (KERNEL_EXEC_MODE, 0)

KRNL_RULES += kernel_run

endif #(KERNEL_EXEC_MODE, 0)

