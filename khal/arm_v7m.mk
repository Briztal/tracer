

#------------------------------------------------------------------------ checks

ifndef ARM_V7M_NVIC_NB_EXCEPTIONS
$(error ARM_V7M_NVIC_NB_EXCEPTIONS not provided)
endif

ifndef ARM_V7M_NVIC_SUPPORT_RELOCATION
$(error ARM_V7M_NVIC_SUPPORT_RELOCATION not provided)
endif


#------------------------------------------------------------------------- flags

#Provide arm_v7m info to khal files;
KHAL_FLAGS += -DNVIC_NB_EXCEPTIONS=$(ARM_V7M_NVIC_NB_EXCEPTIONS)
KHAL_FLAGS += -DNVIC_SUPPORT_RELOC=$(ARM_V7M_NVIC_SUPPORT_RELOCATION)


#--------------------------------------------------------------------------- env

ARM_V7M_SDIR :=  khal/arm_v7m


#------------------------------------------------------------------------- rules

arm_v7m_khal :

#Compile the armv7 base;
	$(KHAL_CC) -o $(KHAL_OBJS_BDIR)/arm_v7m_khal.o -c $(ARM_V7M_SDIR)/arm_v7m_khal.c


#Add the khal base to khal rules;
KHAL_RULES += arm_v7m_khal



#--TODO--------------------------------------------------------------------- FPU

#
#fpu_flags := -DFPU_COPROC_ID=0
#fpu_flags += -DAUTO_STACKING=$(CM4F_FPU_AUTO_STACKING)
#fpu_flags += -DLAZY_STACKING=$(CM4F_FPU_LAZY_STACKING)
#
#
#cortex_m4f :
#	@$(KCC) -o $(KRNL_D)/arm_v7m_vfp.o -c src/hard/arch/arm/arm_v7m/arm_v7m_vfp.c $(fpu_flags)
#
#KRNL_RULES += cortex_m4f
#

#If the FPU active stacking is disabled, the FPU context is 128 bytes long;
#ifeq (CM4F_FPU_AUTO_STACKING, 0)
#
#KFLAGS += -DCOPROC_0_SIZE=128
#
##If the FPU active stacking is disabled, the FPU context is 64 bytes long;
#else
#
#KFLAGS += -DCOPROC_0_SIZE=64
#
#endif
#
#1 for auto stacking enabled, 0 for disabled;
#CM4F_FPU_AUTO_STACKING = 1
#1 for lazy stacking enabled, 0 for disabled;
#CM4F_FPU_LAZY_STACKING = 1
#
