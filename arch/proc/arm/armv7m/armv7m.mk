
#--------------------------------------------------- build env config --------------------------------------------------

#The armv7m uses the general ARM architecture build;
include arch/proc/arm/arm_env.mk


#--------------------------------------------------- khal env config ---------------------------------------------------

ifdef BUILD_KHAL

ARMV7M_SDIR :=  arch/proc/arm/armv7m

#Provide the number of interrupts to the source;
KHAL_FLAGS += -DNVIC_NB_EXCEPTIONS=$(ARMV7M_NVIC_NB_EXCEPTIONS)

#Compile the kernel_base with an inactive flash vector table;
KHAL_FLAGS += -DNVIC_SUPPORT_RELOC=$(ARMV7M_NVIC_SUPPORT_RELOCATION)

endif

#------------------------------------------------- arch mods env config ------------------------------------------------


#------------------------------------------------ armv7m default options -----------------------------------------------

#include the armv7m options file, to set undefined variables to their default value;
include arch/proc/arm/armv7m/armv7m_opt.mk


#------------------------------------------------------ khal rules -----------------------------------------------------

ifdef BUILD_KHAL

armv7m_khal :

#Compile the armv7 base;
	@$(KHAL_CC) -o $(KHAL_OBJS_BDIR)/arm_v7m_khal.o -c $(ARMV7M_SDIR)/armv7m_khal.c


#Add the khal base to khal rules;
KHAL_RULES += armv7m_khal


endif

#--------------------------------------------------- arch_mods rules ---------------------------------------------------


#--TODO------------------------------------------------------ FPU  --------------------------------------------------------

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
