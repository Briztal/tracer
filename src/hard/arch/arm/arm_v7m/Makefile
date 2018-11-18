
# ARMV7M cortex_m4f.mk. Including this makefile will result in the compilation of the kernel base for
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

V7M_D :=  src/hard/arch/arm/arm_v7m

v7m_args :=

V7M_CC = $(KCC) $(v7m_args)


#If the number of interrupts is provided :
ifdef NVIC_NB_EXCEPTIONS

#Provide the number of interrupts to the source;
v7m_args += -DNB_EXCEPTIONS=$(NVIC_NB_EXCEPTIONS)

endif


#If nvic relocation is enabled :
ifdef NVIC_RELOCATION

#Compile the kernel_base with an inactive flash vector table;
v7m_args += -DNVIC_RELOC=$(NVIC_RELOCATION)

endif


#-------------------------------------------------- Compilation Rules --------------------------------------------------

arm_v7m_kernel_base :

#Compile the armv7 base;
	@$(V7M_CC) -o $(KRNL_D)/arm_v7m_boot.o -c $(V7M_D)/arm_v7m_boot.c

#Compile the exceptions lib;
	@$(V7M_CC) -o $(KRNL_D)/arm_v7m_xcpt.o -c $(V7M_D)/arm_v7m_xcpt.c

#Compile the processor lib;
	@$(V7M_CC) -o $(KRNL_D)/arm_v7m_proc.o -c $(V7M_D)/arm_v7m_proc.c

#Compile the systick lib;
	@$(V7M_CC) -o $(KRNL_D)/arm_v7m_syst.o -c $(V7M_D)/arm_v7m_syst.c


#Add the kernel base to the kernel rules;
KRNL_RULES += arm_v7m_kernel_base



arm_v7m_syscl :

#Compile the armv7 base;
	@$(V7M_CC) -o $(STDL_D)/arm_v7m_syscl.o -c $(V7M_D)/arm_v7m_stdl.c


#Add the kernel base to the kernel rules;
STDL_RULES += arm_v7m_syscl



#The nvic relocation module;
arm_v7m_nvic :
	@$(V7M_CC) $(CFLAGS) $(INC) -o $(MODS_D)/arm_v7m_nvic.o -c $(V7M_D)/arm_v7m_nvic.c


#If the nvic relocation module must be added :
ifndef NO_NVIC_RELOCATION

#Add the nvic relocation rule to the modules list;
MODS_RULES += arm_v7m_nvic

arm_v7m_nvic := arm_v7m_nvic.o

endif



#The fault module;
arm_v7m_fault :

	@$(V7M_CC) $(CFLAGS) $(INC) -o $(MODS_D)/arm_v7m_fault.o -c $(V7M_D)/arm_v7m_fault.c


#If the systick module must be added :
ifndef NO_FAULT

#Add the systick rule to the modules list;
MODS_RULES += arm_v7m_fault

arm_v7m_fault := arm_v7m_fault.o

endif


