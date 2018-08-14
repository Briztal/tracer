
#The armv7m uses the general ARM acrhitecture build;
include src/hardware/arch/arm/Makefile

ARCH_DIR :=  src/harware/arch/arm/arm_v7m

#add the armv7 directory to the include path;
L_INC += $(ARCH_DIR)

#Add armv7 and nvic files to core source files;
SW_SOURCES +=  $(ARCH_DIR)/arm_v7m.c
SW_SOURCES +=  $(ARCH_DIR)/nvic.cw




