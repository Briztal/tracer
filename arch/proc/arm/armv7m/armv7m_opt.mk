
#
# This file contains all options that the armv7m supports, and their default values;
# Those options can be set by any build code to adapt the armv7m build to the related implementation;
#

#---------------------------------------------------- khal options -----------------------------------------------------

ifdef BUILD_KHAL

#-------- NVIC --------

#The number of exceptions that the NVIC supports; Must be implemented by the hardware;
ARMV7M_NVIC_NB_EXCEPTIONS ?= 0

#The NVIC relocation support flag; 0 for disabled, 1 for enabled
ARMV7M_NVIC_SUPPORT_RELOC ?= 0

endif


#-------------------------------------------------- arch_mods options --------------------------------------------------

ifdef BUILD_ARCH_MODS


endif