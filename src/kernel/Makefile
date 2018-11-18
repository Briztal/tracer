

#------------------------------------------- Components configuration flags --------------------------------------------

#Add the exception stacks size requirements;
KFLAGS += -DKEX_STACK_SIZE=$(KEX_STACK_SIZE)

#Add first process hardware requirements;
KFLAGS += -DKFP_RAM_SIZE=$(KFP_RAM_SIZE)
KFLAGS += -DKFP_STACK_SIZE=$(KFP_STACK_SIZE)
KFLAGS += -DKFP_ACTIVITY_TIME=$(KFP_ACTIVITY_TIME)

KFLAGS += -DKDM_SIZE=$(KDM_SIZE)

KFLAGS += -DNB_COPROCESSORS=$(KERNEL_NB_COPROCESSORS)


#-------------------------------------------------- Compilation rules --------------------------------------------------

KS := src/kernel
KR_BL := build/krnl

kernel :

	@$(KCC) -o $(KR_BL)/fault.o 		-c $(KS)/async/fault.c
	@$(KCC) -o $(KR_BL)/except.o 		-c $(KS)/async/except.c


	@$(KCC) -o $(KR_BL)/clock.o 		-c $(KS)/clock/clock.c
	@$(KCC) -o $(KR_BL)/sysclock.o 	-c $(KS)/clock/sysclock.c


	@$(KCC) -o $(KR_BL)/probe.o 		-c $(KS)/debug/probe.c
	@$(KCC) -o $(KR_BL)/debug.o 		-c $(KS)/debug/debug.c
	@$(KCC) -o $(KR_BL)/printk.o 			-c $(KS)/debug/printk.c


	@$(KCC) -o $(KR_BL)/inode.o 		-c $(KS)/fs/inode.c


	@$(KCC) -o $(KR_BL)/heap.o 		-c $(KS)/mem/heap.c
	@$(KCC) -o $(KR_BL)/kdmem.o 		-c $(KS)/mem/kdmem.c
	@$(KCC) -o $(KR_BL)/ram.o 			-c $(KS)/mem/ram.c


	@$(KCC) -o $(KR_BL)/mod.o 			-c $(KS)/mod/mod.c


	@$(KCC) -o $(KR_BL)/apps.o 			-c $(KS)/run/apps.c
	@$(KCC) -o $(KR_BL)/pmem.o 			-c $(KS)/run/pmem.c
	@$(KCC) -o $(KR_BL)/coproc.o 		-c $(KS)/run/coproc.c
	@$(KCC) -o $(KR_BL)/proc.o 		-c $(KS)/run/proc.c
	@$(KCC) -o $(KR_BL)/sched.o 		-c $(KS)/run/sched.c
	@$(KCC) -o $(KR_BL)/ksyscall.o 	-c $(KS)/ksyscall.c


	@$(KCC) -o $(KR_BL)/nlist.o 		-c $(KS)/struct/nlist.c
	@$(KCC) -o $(KR_BL)/shared_fifo.o 	-c $(KS)/struct/shared_fifo.c


	@$(KCC) -o $(KR_BL)/kinit.o 		-c $(KS)/kinit.c
	@$(KCC) -o $(KR_BL)/panic.o 		-c $(KS)/panic.c



#-------------------------------------------------- Components Config --------------------------------------------------

KRNL_RULES += kernel


