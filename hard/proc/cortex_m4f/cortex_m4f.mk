
#-------------------------------------------------------- Config -------------------------------------------------------

#Include the configuration makefile;
include src/hard/proc/cortex_m4f/cortex_m4f_cfg.mk


#-------------------------------------------------- Components config --------------------------------------------------

#Proc compiler options;
CFLAGS += -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16

#Proc linker options
#LDFLAGS += --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard
#--specs=nano.specs

#NVIC relocation is possible on the cortex M4F
NVIC_RELOCATION = 1



#The Cortex M4f has one coprocessor, the FPU;
KERNEL_NB_COPROCESSORS = 1

#If the FPU active stacking is disabled, the FPU context is 128 bytes long;
ifeq (CM4F_FPU_AUTO_STACKING, 0)

KFLAGS += -DCOPROC_0_SIZE=128

#If the FPU active stacking is disabled, the FPU context is 64 bytes long;
else

KFLAGS += -DCOPROC_0_SIZE=64

endif



#The cortex M4 is based on the armv7 architecture;
include src/hard/arch/arm/arm_v7m/Makefile


#-------------------------------------------------------- Rules --------------------------------------------------------

fpu_flags := -DFPU_COPROC_ID=0
fpu_flags += -DAUTO_STACKING=$(CM4F_FPU_AUTO_STACKING)
fpu_flags += -DLAZY_STACKING=$(CM4F_FPU_LAZY_STACKING)


cortex_m4f :
	@$(KCC) -o $(KRNL_D)/arm_v7m_vfp.o -c src/hard/arch/arm/arm_v7m/arm_v7m_vfp.c $(fpu_flags)

KRNL_RULES += cortex_m4f