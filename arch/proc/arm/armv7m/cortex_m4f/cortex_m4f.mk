

#--------------------------------------------------- build env config --------------------------------------------------


#Proc compiler options;
CFLAGS += -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16

#Proc linker options
LDFLAGS += --specs=nano.specs -mfloat-abi=hard -mfpu=fpv4-sp-d16


#--------------------------------------------------- khal env config ---------------------------------------------------

ifdef BUILD_KHAL

#NVIC relocation is possible on the cortex M4F
ARM_V7M_NVIC_SUPPORT_RELOC := 1

endif


#------------------------------------------------- arch mods env config ------------------------------------------------



#----------------------------------------------------- proc family -----------------------------------------------------

#The cortex M4 is based on the armv7 architecture;
include arch/proc/arm/armv7m/armv7m.mk

