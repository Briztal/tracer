
#--------------------------------------------------- build env config --------------------------------------------------

#Set the memory map script search dir;
LDSCRIPT_MMAP_DIR ?= chip/kinetis/chips/mk64fx512/


#--------------------------------------------------- khal env config ---------------------------------------------------

#Only required if the khal must be built;
ifdef BUILD_KHAL

#The mk64fx512 comprises 256 interrupts TODO WRONG !!
NVIC_NB_EXCEPTIONS = 256

endif


#------------------------------------------------- arch mods env config ------------------------------------------------


#------------------------------------------------------ proc build -----------------------------------------------------

include arch/proc/arm/armv7m/cortex_m4f/cortex_m4f.mk


#----------------------------------------------------- chip family -----------------------------------------------------

#The mk64fx512 is part of the kinetxi k family, and is built with the related makefile;
include arch/chip/kinetis/kx.mk

