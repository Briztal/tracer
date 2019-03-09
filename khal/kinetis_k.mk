#
# This file is a generic build file for chips of the kinetis k family;
#
#	To build a chip of this family, create a makefile in which are included :
#	- the appropriate processor make file;
#	- the configuration file related to the chip; this file should be based on the kx_cfg_tmpl.mk template file,
#		with values updated accordingly to the chip;
#	- this make file;
#
#	This make file builds or not k family components based on the content of the configuration file;


#----------------------------------------------------- local env -----------------------------------------------------

KX_DIR := khal/kinetis_k

KHAL_FLAGS += -DBACKDOOR_CMP_KEY=$(FLASH_CFG_BACKDOOR_COMPARISON_KEY)
KHAL_FLAGS += -DF_OPT=$(FLASH_CFG_FLASH_OPTIONS)
KHAL_FLAGS += -DF_SEC=$(FLASH_CFG_FLASH_SECURITY)


#----------------------------------------------------- khal rules -----------------------------------------------------

#------------ kernel code ------------

#Kinetis k kernel code;
kinetis_k_khal:

#Compile the init source;
	$(KHAL_CC) -o $(KHAL_OBJS_BDIR)/kx_khal.o -c $(KX_DIR)/kx_khal.c

#The kinetis k rule must be executed in all cases;
KHAL_RULES += kinetis_k_khal


#------------ Flash config ------------


flash_cfg:
	$(MKCC) -o $(KRNL_D)/kx_flash_cfg.o -c $(K_DIR)/kx_flash_cfg.c $(fcfg_flags)

#TODO The flash configuration code must be included only if the executable is standalone; TODO
#TODO The flash configuration code must be included only if the executable is standalone; TODO
#TODO The flash configuration code must be included only if the executable is standalone; TODO
#TODO The flash configuration code must be included only if the executable is standalone; TODO
#TODO The flash configuration code must be included only if the executable is standalone; TODO
#TODO The flash configuration code must be included only if the executable is standalone; TODO
#AMOD_RULES += flash_cfg

