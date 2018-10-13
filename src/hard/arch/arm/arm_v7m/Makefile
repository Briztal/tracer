
# ARMV7M Makefile. Including this makefile will result in the compilation of the kernel base for
#	the ARMV7M architecture.
#
#	Two modules are also added to the modules list, the systick and the NVIC relocation module;
#
#	Both of them can be skipped, by defining non empty values for NO_SYSTICK and NO_NVIC_RELOCATION
#
#	If NO_NVIC_RELOCATION has an empty value, the kernel base is compiled with option NO_VTABLE, and flash
#	based vector table is inactive. This produces a smaller executable;
#


#The armv7m uses the general ARM acrhitecture build;
include src/hard/arch/arm/Makefile

V7M_DIR :=  src/hard/arch/arm/arm_v7m



v7m_args :=


#If the number of interrupts is provided :
ifdef NVIC_NB_INTERRUPTS

#Provide the number of interrupts to the source;
v7m_args += -DNB_INTERRUPTS=$(NVIC_NB_INTERRUPTS)

endif


#If nvic relocation is enabled :
ifndef NO_NVIC_RELOCATION

#Compile the kernel_base with an inactive flash vector table;
v7m_args += -DNO_VTABLE

endif


#The kernel base for arm_v7m
arm_v7m :
	@echo "compiling ARM_V7M module\n"

#Compile the armv7 base;
	@$(CC) $(CFLAGS) $(INC) $(v7m_args) -o build/hard/arm_v7m_base.o -c $(V7M_DIR)/arm_v7m_base.c

#if the preemption base must be compiled :
ifndef KERNEL_LOOP_MODE

#Compile the preemption base;
	@$(CC) $(CFLAGS) $(INC) -o build/hard/arm_v7m_adv.o -c $(V7M_DIR)/arm_v7m_adv.c

endif #KERNEL_LOOP_MODE



#The nvic relocation module;
arm_v7m_nvic_reloc :
	@$(CC) $(CFLAGS) $(INC) -o build/hard/arm_v7m_nvic.o -c $(V7M_DIR)/arm_v7m_nvic.c


#If the nvic relocation module must be added :
ifndef NO_NVIC_RELOCATION

#Add the nvic relocation rule to the module dependencies list;
MODULES += arm_v7m_nvic_reloc

endif


#The systick module;
arm_v7m_systick :

	@$(CC) $(CFLAGS) $(INC) -o build/hard/arm_v7m_systick.o -c $(V7M_DIR)/arm_v7m_systick.c


#If the systick module must be added :
ifndef NO_SYSTICK

#Add the systick rule to the module dependencies list;
MODULES += arm_v7m_systick

endif



#The fault module;
arm_v7m_fault :

	@$(CC) $(CFLAGS) $(INC) -o build/hard/arm_v7m_fault.o -c $(V7M_DIR)/arm_v7m_fault.c


#If the systick module must be added :
ifndef NO_FAULT

#Add the systick rule to the module dependencies list;
MODULES += arm_v7m_fault

endif


